#include <fstream>
#include <iostream>
#include <bits/stdc++.h> 
#include <math.h>       /* sqrt */
#include <limits.h>		/* log */



using namespace std;

double* createMatrix(int endLine) {
	double* Matrix = new double[endLine]();
	for (int i = 0; i < endLine; i++) {
		cin >> Matrix[i];
	}
	return Matrix;
}

double errorEstimate(double* A, double* B, int N) {
	double result = .0;
	for (int i = 0; i < N; i++) {
		result += pow(abs(B[i] - A[i]), 2);
	}
	return sqrt(result);
}

// main function
int main() {
	// Goal Matrices:
	double AGoal[9] = { .7, .05, .25, .1, .8, .1, .2, .3, .5 };
	double BGoal[12] = { .7, .2, .1, 0, .1, .4, .3, .2, 0, .1, .2, .7 };
	double PiGoal[3] = { 1, 0, 0 };

	// Create Initial Matrices:
	int Na, Ka, Nb, Kb, Npi, Kpi, Nobs;
	cin >> Na;
	cin >> Ka;
	int endLineA = Na * Ka;
	double* A = createMatrix(endLineA);
	cin >> Nb;
	cin >> Kb;
	int endLineB = Nb * Kb;
	double* B = createMatrix(endLineB);
	cin >> Npi;
	cin >> Kpi;
	int endLinePi = Npi * Kpi;
	double* Pi = createMatrix(endLinePi);
	cin >> Nobs;
	double* Emissions = createMatrix(Nobs);

	// Initial error
	cout << "Initial error for A is: " << errorEstimate(A, AGoal, Na * Ka) << endl;
	cout << "Initial error for B is: " << errorEstimate(B, BGoal, Nb * Kb) << endl;
	cout << "Initial error for Pi is: " << errorEstimate(Pi, PiGoal, Npi * Kpi) << endl;

	// Variables
	double* c = new double[Nobs]();
	double* alphaForward = new double[Nobs*Na]();
	double* beta = new double[Nobs * Na]();
	double* digamma = new double[Nobs * Na * Na]();
	double* gamma = new double[Nobs * Na]();
	int counter = 0;
	double logProb = INT_MIN + 1;
	double prevLogProb = INT_MIN;

	while (logProb > prevLogProb && counter < 99999) {

		// ************************************************************
		// 1. Calculate alpha
		// 1.1. Initialize alpha
		// 1.1.1. Compute a0
		c[0] = 0;
		for (int i = 0; i < Na; i++) {
			int index = Emissions[0];
			alphaForward[i] = B[i * Kb + index] * Pi[i];
			c[0] += alphaForward[i];
		}

		// 1.1.2. Scale a0
		c[0] = 1 / c[0];
		for (int i = 0; i < Na; i++) {
			alphaForward[i] *= c[0];
		}

		// 1.2. Iterate alpha
		// 1.2.1. Compute at
		// through time:
		for (int t = 1; t < Nobs; t++) {
			c[t] = 0;
			// through states
			for (int i = 0; i < Na; i++) {
				// again through states
				float alphaSum = 0.0;
				for (int j = 0; j < Na; j++) {
					alphaSum += A[j * Na + i] * alphaForward[(t - 1) * Nb + j];
				}
				int index = Emissions[t];
				alphaForward[t * Nb + i] = B[i * Kb + index] * alphaSum;
				c[t] += alphaForward[t * Nb + i];
			}

			// 1.2.2. Scale at
			c[t] = 1 / c[t];
			for (int i = 0; i < Na; i++) {
				alphaForward[t * Nb + i] *= c[t];
			}
		}

		// ************************************************************
		// 2. Calculate beta
		// 2.1. Initialize beta, scaled by c
		for (int i = 0; i < Na; i++) {
			beta[(Nobs - 1)*Na + i] = c[Nobs-1];
		}
		// 2.2. Iterate beta
		// through time:
		for (int t = Nobs - 2; t >= 0; t--) {
			int indexEm = Emissions[t + 1];
			// through states
			for (int i = 0; i < Na; i++) {
				// again through states
				beta[t * Nb + i] = 0.0;
				for (int j = 0; j < Na; j++) {
					beta[t * Nb + i] += beta[(t + 1) * Nb + j] * B[j * Kb + indexEm] * A[i * Na + j];
				}
				beta[t * Nb + i] *= c[t];
			}
		}

		// ************************************************************
		// 3. Calculate gamma
		// 3.1. Di-gamma function

		// No need for alphaSum since we already took it into account when we generalized in alpha with c

		for (int t = 0; t < Nobs-1; t++) {
			int indexEm = Emissions[t+1];
			// through states
			for (int i = 0; i < Na; i++) {
				// again through states
				double num = 0.0;
				gamma[t * Na + i] = 0;
				for (int j = 0; j < Na; j++) {
					num = alphaForward[t * Na + i] * A[i * Na + j] * B[j * Kb + indexEm] * beta[(t + 1) * Nb + j];
					digamma[t * Na * Na + i * Na + j] = num; 
					gamma[t * Na + i] += digamma[t * Na * Na + i * Na + j];
				}
			}
		}
		for (int i = 0; i < Na; i++) {
			gamma[(Nobs - 1) * Na + i] = alphaForward[(Nobs - 1) * Na + i];
		}

		// ************************************************************
		// 4. Calculate new parameters
		// 4.1. Transmission matrix A
		for (int i = 0; i < Na; i++) {
			for (int j = 0; j < Na; j++) {
				double digammaTimeSum = 0.0;
				double gammaTimeSum = 0.0;
				for (int t = 0; t < Nobs-1; t++) {
					digammaTimeSum += digamma[t * Na * Na + i * Na + j];
					gammaTimeSum += gamma[t * Na + i];
				}
				A[i * Na + j] = digammaTimeSum / gammaTimeSum;
			}
		}

		// 4.2. Emission matrix B
		for (int i = 0; i < Na; i++) {
			for (int k = 0; k < Kb; k++) {
				double gammaTimeSumIndicator = 0.0;
				double gammaTimeSum = 0.0;
				for (int t = 0; t < Nobs; t++) {
					if (Emissions[t] == k) {
						gammaTimeSumIndicator += gamma[t * Na + i];
					}
					gammaTimeSum += gamma[t * Na + i];
				}
				B[i * Kb + k] = gammaTimeSumIndicator / gammaTimeSum;
			}
		}

		// 4.3. Initial probability
		for (int i = 0; i < Na; i++) {
			Pi[i] = gamma[i];
		}

		// 5. Calculate log
		prevLogProb = logProb;
		logProb = 0;
		for (int t = 0; t < Nobs; t++) {
			logProb += log(c[t]);
		}
		logProb = -logProb;
		counter++;
		}

	// *******************************************************
	// Show results
	cout << Na << " " << Na << " ";
	for (int i = 0; i < Na * Ka; i++) {
		cout << A[i] << " ";
	}
	cout << endl;
	cout << Nb << " " << Kb << " ";
	for (int i = 0; i < Nb * Kb; i++) {
		cout << B[i] << " ";
	}
	cout << endl;
	cout << Npi << " " << Kpi << " ";
	for (int i = 0; i < Npi * Kpi; i++) {
		cout << Pi[i] << " ";
	}

	cout << endl;
	cout << "The dataset converges after " << counter << " repetitions." << endl;
	cout << "Final error for A is: " << errorEstimate(A, AGoal, Na*Ka) << endl;
	cout << "Final error for B is: " << errorEstimate(B, BGoal, Nb* Kb) << endl;
	cout << "Final error for Pi is: " << errorEstimate(Pi, PiGoal, Npi* Kpi) << endl;

	/*
	Question 7:
	It did converged after 647 repetitions.
	It is reasonable since the initial matrices were close to our goal matrices

	Question 8:
	Matrices similar to the goal, are more probable to converge, and also can converge sooner rather than later.
	It will always converge, but depending on the values you give, it may take more time, and the more distant
	have the initial from the goal matrices, the more will distinguish after they converged.

	Question 9:
	More states: Perhaps more iterations to converge. Also overfitting may be a problem in that case
	Few states: Generally few iterations to converge. But may dont have enough info for the model to work.
	But really depends on our dataset. If we have small dataset, then the algorithm needs more iterations
	to say for sure that it converged. 
	Pi generally strange.

	Question 10:
	- Uniform distribution doen't work because it will kinda always converge on their initial uniform value.
	- Diagonal don't work at all (NaN). Probably because at some point we need to divide by zero. But if we change it
	just a bit it works. Pi is wrong, unless if the elements are not zero.
	- With matrices similar to the goals, they converge closer to our initial values than our goal values.
	Or they all converge to specific values but not on our goal matrices. This is perhaps overfitting. 
	(The results converge to specific values - low variance, but are still a bit different than our goal - high bias.)

	*/

}




