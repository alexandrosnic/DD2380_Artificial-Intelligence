#include <fstream>
#include <iostream>    // std::cout
#include <bits/stdc++.h> 
#include <algorithm>    // std::max


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

	double* delta = new double[Nobs*Na]();
	double* deltaIndex = new double[Nobs * Na]();

	// ************************************************************
	// VITERBI ALGORITHM
	// Calculate delta

	// Initialize delta:
	// delta = beta * Pi
	for (int i = 0; i < Na; i++) {
		int emIndex = Emissions[0];
		delta[i] = B[i * Kb + emIndex] * Pi[i];
	}
	// Iterate delta:
	// through time:
	for (int t = 1; t < Nobs; t++) {
		int emIndex = Emissions[t];
		// through states
		for (int i = 0; i < Na; i++) {
			// again through states
			float maxValue = 0;
			float tempDelta = 0;
			int maxIndex = 0;
			for (int j = 0; j < Na; j++) {
				float tempDelta = A[j * Na + i] * delta[(t - 1) * Nb + j] * B[i * Kb + emIndex];
				if (tempDelta > maxValue) {
					maxValue = tempDelta;
					maxIndex = j;
				}
			}
			delta[t*Nb +i] = maxValue;
			deltaIndex[t * Nb + i] = maxIndex;
		}
	}

	// **************************************************************
	// Calculate Probability:
	double tempProb = 0;
	double Prob = 0;
	int probIndex = 0;
	for (int j = 0; j < Na; j++) {
		tempProb = delta[(Nobs - 1) * Na + j];
		if (tempProb > Prob) {
			Prob = tempProb;
			probIndex = j;
		}
	}

	// **************************************************************
	// Backtrack the state sequence
	int* stateSeq = new int[Nobs]();
	stateSeq[Nobs - 1] = probIndex;
	for (int t = Nobs-2; t >=0 ; t--) {
		stateSeq[t] = deltaIndex[(t+1) * Na + stateSeq[t+1]];
	}

	// **************************************************************
	// Show the state sequence
	// It is the most probable state sequence, given the observations
	for (int t = 0; t < Nobs; t++) {
		cout << stateSeq[t] << " ";
	}

	/*
	Question 5:
	T (number of obs) * N (number of states) values */

}



