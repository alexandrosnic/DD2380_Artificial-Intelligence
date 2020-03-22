#include <fstream>
#include <iostream>
#include <bits/stdc++.h> 

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

	double* alphaForward = new double[Nobs*Na]();

	// Initialize alpha:
	// alpha = beta * Pi
	for (int i = 0; i < Na; i++) {
		int index = Emissions[0];
		alphaForward[i] = B[i * Kb + index] * Pi[i];
	}

	// Iterate alpha:
	// through time:
	for (int i = 1; i < Nobs; i++) {
		// through states
		for (int k = 0; k < Na; k++) {
			// again through states
			float alphaSum = 0.0;
			for (int j = 0; j < Na; j++) {
				alphaSum += A[j * Na + k] * alphaForward[(i-1)*Nb+j];
			}
			int index = Emissions[i];
			alphaForward[i*Nb +k] = B[k * Kb + index] * alphaSum;
		}

	}


	// Calculate Probability:
	// It's the probability of observing the specific sequence of observations
	double Prob =0;
	for (int j = 0; j < Na; j++) {
		Prob += alphaForward[(Nobs-1)*Na + j];
	}

	cout << Prob << endl;


	/*
	Question 4:
	As of Markov's assumption: every observation is independent of all else, given the current state.
	So it is only dependent on the current state.*/

}



