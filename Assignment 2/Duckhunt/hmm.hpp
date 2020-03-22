#ifndef _hmm_h
#define _hmm_h
#include <iostream>
#include <vector>
#include "Player.hpp"

using namespace std;
namespace HMMFunction {
	class HMM {
	private:

		void runAlpha(int* newSeq, int newSeqLen);

		void alphaPass();

		void betaPass();

		void calcGamma();

		void updateParams();

		void initMatrix();

	public:

		ducks::ESpecies bird;

		int N = 2;
		int M = 9;
		int T;

		int sequence[100];
		int len;
		int boolConverge = 0;
		double A[2][2];
		double B[2][9];
		double Pi[2][2];
		double alphaForward[100][2];
		double beta[100][2];
		double c[100];
		double digamma[100][2][2];
		double gamma[100][2];
		double alphaRun(int* newSeq, int len);

		void run(int maxit);
		int birdNextMove();
		void generateMatrix();
		HMM() {
			generateMatrix();
		}
	};
}
#endif