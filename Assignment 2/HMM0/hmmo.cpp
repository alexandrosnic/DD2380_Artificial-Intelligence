#include <fstream>
#include <iostream>
#include <bits/stdc++.h> 

using namespace std;


/* Question 1, 2 and 3:
In papers
*/

// function for doing multiplication of A and B matrices
double* matrMult(double* A, double* B, int N, int M, int L) {
	double* result = new double[M*L]();
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			for (int k = 0; k < L; k++) {
				result[i*L+j] += A[i * L + k] * B[k * M + j];
			}
		} 
	}
	return result;
}

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
	int Na, Ka, Nb, Kb, Npi, Kpi;
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
	
	// *************************************************************************
	// Do the multiplication
	double* Api = matrMult(Pi, A, 1, Na, Na);
	double* Obs = matrMult(Api, B, 1, Kb, Nb);

	// *************************************************************************
	// Show the result

	cout << Npi << " " << Kb << " ";
	for (int i = 0; i < Kb; i++) {
		std::cout << Obs[i] << " ";
	}
	












	/*

	APPENDIX


	 If we are not given the matrices:
	// dimensions of the matrices
	int N;
	int K;
	// N dimension of matrices
	cout << "Insert N number" <<endl;
	cin >> N;
	// K dimension of matrices
	cout << "Insert K number" <<endl;
	cin >> K;
	// ************************************************************************
	// Define the A - transition matrix
	double* A = new double[N * N]();
	cout << "Insert the elements of A matrix" << endl;
	for (int i = 0; i < N * N; i++) {
		cin >> A[i];
	}
	// Define the B - observation matrix
	double* B = new double[N * K]();
	cout << "Insert the elements of B matrix" << endl;
	for (int i = 0; i < N * K; i++) {
		cin >> B[i];
	}
	// Define the Pi - initial vector
	double* Pi = new double[N]();
	cout << "Insert the elements of Pi matrix" << endl;
	for (int i = 0; i < N; i++) {
		cin >> Pi[i];
	}


	****************************************************************************
	// Show the matrices
	//cout << "The A matrix is: " << " " << Na << " " << Ka << endl ;
	//for (int i = 0; i < Na*Ka; i++) {
	//	std::cout << A[i] << " " << endl;
	//}
	//cout << "The B matrix is: " << " " << Nb << " " << Kb << endl;
	//for (int i = 0; i < Nb * Kb; i++) {
	//	std::cout << B[i] << " " << endl;
	//}
	//cout << "The Pii matrix is: " << " " << Npi << " " << Kpi << endl;
	//for (int i = 0; i < Npi * Kpi; i++) {
	//	std::cout << Pi[i] << " " << endl;
	//}


	****************************************************************************
	// Show the result
	//cout << "The Api matrix is: " << endl;
	//cout << Npi << " " << Na << " ";
	//for (int i = 0; i < Na; i++) {
	//	std::cout << Api[i] << " ";
	//}
	//cout << "The Obs matrix is: " << endl;
	*/
}





