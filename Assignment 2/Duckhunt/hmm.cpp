#include <iostream>
#include <limits.h>		/* log */
#include <random>
#include <time.h>
#include <stdlib.h>
#include <float.h>
#include <vector>
#include "hmm.hpp"

using namespace std;


// *****************************************************************
// 1. alphaPass
void HMMFunction::HMM::alphaPass() {

	// 1.1. Initialize alphaForward
	// 1.1.1. Compute a0
	c[0] = 0.0;
	for (int i = 0; i < N; i++) {
		int index = sequence[0];
		alphaForward[0][i] = B[i][index] * Pi[0][i];
		c[0] += alphaForward[0][i];
	}

	// 1.1.2. Scale a0
	c[0] = (c[0] == 0) ? 0 : 1 / c[0];
	for (int i = 0; i < N; i++) {
		alphaForward[0][i] *= c[0];
	}

	// 1.2. Iterate alphaForward
	// 1.2.1. Compute at
	// through time:
	for (int t = 1; t < T; t++) {
		c[t] = 0;
		// through states
		for (int i = 0; i < N; i++) {
			// again through states
			float alphaSum = 0.0;
			for (int j = 0; j < N; j++) {
				alphaSum += A[j][i] * alphaForward[t - 1][j];
			}
			int index = sequence[t];
			alphaForward[t][i] = B[i][index] * alphaSum;
			c[t] += alphaForward[t][i];
		}

		// 1.2.2. Scale at
		c[t] = (c[t] == 0) ? 0 : 1 / c[t];
		for (int i = 0; i < N; i++) {
			alphaForward[t][i] *= c[t];
		}
	}
}

// *****************************************************************
// 1.3. runAlpha
void HMMFunction::HMM::runAlpha(int* birdSeq, int SeqLength) {

	// Initialize alpha:
	// alpha = beta * Pi
	for (int i = 0; i < N; i++) {
		int index = birdSeq[0];
		alphaForward[0][i] = B[i][index] * Pi[0][i];
	}

	// Iterate alpha:
	// through time:
	for (int t = 1; t < SeqLength; t++) {
		// through states
		for (int i = 0; i < N; i++) {
			// again through states
			alphaForward[t][i] = 0.0;
			for (int j = 0; j < N; j++) {
				alphaForward[t][i] += A[j][i] * alphaForward[t - 1][j];
			}
			int index = birdSeq[t];
			alphaForward[t][i] = B[i][index] * alphaForward[t][i];
		}
	}
}


// *****************************************************************
// 1.4. alphaRun
double HMMFunction::HMM::alphaRun(int* birdSeq, int birdSeqLength) {
	runAlpha(birdSeq, birdSeqLength);
	// Calculate Probability:
	double Prob = 0;
	for (int j = 0; j < N; j++) {
		Prob += alphaForward[birdSeqLength - 1][j] / c[birdSeqLength - 1];
	}
	return Prob;
}


// ************************************************************
// 2. Calculate beta
void HMMFunction::HMM::betaPass() {

	// 2.1. Initialize beta, scaled by c
	for (int i = 0; i < N; i++) {
		beta[T - 1][i] = c[T - 1];
	}
	// 2.2. Iterate beta
	// through time:
	for (int t = T - 2; t >= 0; t--) {
		int indexEm = sequence[t + 1];
		// through states
		for (int i = 0; i < N; i++) {
			// again through states
			beta[t][i] = 0.0;
			for (int j = 0; j < N; j++) {
				beta[t][i] += beta[t + 1][j] * B[j][indexEm] * A[i][j];
			}
			beta[t][i] *= c[t];
		}
	}
}


// ************************************************************
// 3. Calculate gamma
void HMMFunction::HMM::calcGamma() {

	// 3.1. Di-gamma function
	double denom;
	for (int t = 0; t < T - 1; t++) {
		int indexEm = sequence[t + 1];
		denom = 0;
		// through states
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				denom += alphaForward[t][i] * A[i][j] * B[j][indexEm] * beta[t + 1][j];
			}
		}
		for (int i = 0; i < N; i++) {
			// again through states
			gamma[t][i] = 0;
			for (int j = 0; j < N; j++) {
				digamma[t][i][j] = alphaForward[t][i] * A[i][j] * B[j][indexEm] * beta[t + 1][j] / denom;
				// 3.2. Gamma
				gamma[t][i] += digamma[t][i][j];
			}
		}
	}
	denom = 0;
	for (int i = 0; i < N; i++) {
		denom += alphaForward[T - 1][i];
	}
	for (int i = 0; i < N; i++) {
		gamma[T - 1][i] = alphaForward[T - 1][i] / denom;
	}
}




// ************************************************************
// 4. Calculate new parameters
void HMMFunction::HMM::updateParams() {

	// 4.1. Transmission matrix A
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			double numer = 0.0;
			double denom = 0.0;
			for (int t = 0; t < T - 1; t++) {
				numer += digamma[t][i][j];
				denom += gamma[t][i];
			}
			A[i][j] = numer / denom;
		}
	}

	// 4.2. Emission matrix B
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			double numer = 0.0;
			double denom = 0.0;
			for (int t = 0; t < T; t++) {
				if (sequence[t] == j) {
					numer += gamma[t][i];
				}
				denom += gamma[t][i];
			}
			B[i][j] = numer / denom;
		}
	}

	// 4.3. Initial probability
	for (int i = 0; i < N; i++) {
		Pi[0][i] = gamma[0][i];
	}
}



// ************************************************************
// 5. hmmRun -> Baum Welch algorithm
void HMMFunction::HMM::run(int maxIters) {
	int counter = 0;
	double logProb = INT_MIN + 1;
	double prevLogProb = INT_MIN;

	while (logProb > prevLogProb) {

		counter++;

		alphaPass();

		betaPass();

		calcGamma();

		updateParams();

		prevLogProb = logProb;
		logProb = 0;
		for (int t = 0; t < T; t++) {
			logProb += log(c[t]);
		}
		logProb = -logProb;
		if (counter > maxIters) {
			boolConverge = -1;

			break;
		}
	}
	boolConverge = 1;
}




// ************************************************************
// 6. Calculate next move
int HMMFunction::HMM::birdNextMove() {
	double maxProb = 0;
	double sum;
	int pState = -1;
	for (int i = 0; i < 9; i++) {
		sum = 0;
		for (int j = 0; j < N; j++) {
			sum += alphaForward[T - 1][j] * B[j][i];
		}
		if (sum > maxProb) {
			maxProb = sum;
			pState = i;
		}
	}
	if (maxProb < 0.6)
		return -1;
	return pState;
}



// ************************************************************
// 7. Initialize matrix
void HMMFunction::HMM::initMatrix() {
	srand(time(NULL));
	double rowSum, sign, ran;
	for (int i = 0; i < N; i++) {

		rowSum = 0.0;
		for (int j = 0; j < N; j++) {
			if (j == N - 1) {
				A[i][j] = 1 - rowSum;
			}
			else {

				ran = ((rand() % 5) * 0.05);
				sign = (rand() % 2 != 1) ? 1 : -1;
				A[i][j] = 1 / (double)N + sign * ran / (double)N;
				rowSum += A[i][j];
			}
		}
	}
	for (int i = 0; i < N; i++) {

		rowSum = 0.0;
		for (int j = 0; j < M; j++) {
			if (j == M - 1) {
				B[i][j] = 1 - rowSum;
			}
			else {

				ran = ((rand() % 5) * 0.05);
				sign = (rand() % 2 != 1) ? 1 : -1;
				B[i][j] = 1 / (double)M + sign * ran / (double)M;
				rowSum += B[i][j];
			}
		}
	}

	rowSum = 0.0;
	for (int j = 0; j < N; j++) {
		if (j == N - 1) {
			Pi[0][j] = 1 - rowSum;
		}
		else {

			ran = ((rand() % 5) * 0.05);
			sign = (rand() % 2 != 1) ? 1 : -1;
			Pi[0][j] = 1 / (double)N + sign * ran / (double)N;
			rowSum += Pi[0][j];
		}
	}

}


// ************************************************************
// 8. Initialize matrix
void HMMFunction::HMM::generateMatrix() {
	initMatrix();
}


// ************************************************************
// 9. Print Lambda
//void HMMFunction::HMM::printLambda() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++)
//			cerr << A[i][j] << " ";
//		cerr << endl;
//	}
//	cerr << "A MATRIX DONE" << endl << endl;
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < M; j++)
//			cerr << B[i][j] << " ";
//		cerr << endl;
//	}
//	cerr << "B MATRIX DONE" << endl << endl;
//	for (int i = 0; i < 1; i++) {
//		for (int j = 0; j < N; j++)
//			cerr << Pi[i][j] << " ";
//		cerr << endl;
//	}
//	cerr << "Pi MATRIX DONE" << endl << endl;
//}
