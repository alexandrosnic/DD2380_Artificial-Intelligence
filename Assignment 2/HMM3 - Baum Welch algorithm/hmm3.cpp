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

// main function
int main() {
	// Create Matrices:
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



	/*
	Question 6:
	Di-gamma function is an application of the Bayes theorem. So the denominator of the Bayes is in this case the
	sum of the final values of alpha 
	*/
}




