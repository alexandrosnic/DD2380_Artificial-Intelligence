#include "player.hpp"
#include <cstdlib>
#include <cmath>
#include<bits/stdc++.h> 

namespace TICTACTOE3D
{

	int alphaBeta(const GameState& pState, int depth, int alpha, int beta, uint8_t player, const Deadline &pDue);
	int heur(const GameState& pState, uint8_t  player);

GameState Player::play(const GameState &pState,const Deadline &pDue)
{
    //std::cerr << "Processing " << pState.toMessage() << std::endl;

	// Variables
	uint8_t nextPlayer = pState.getNextPlayer();	// The next player
	int depth ;	// Depth of the tree
	int alpha = INT_MIN;	  // alpha
	int beta = INT_MAX;	// beta
	int optValue = INT_MIN;	// The current optimum value for v
	GameState optValIndex;	// The state corresponding to the optimum value for v

    std::vector<GameState> lNextStates;
    pState.findPossibleMoves(lNextStates);

	if (lNextStates.size() < 50)depth = 1;
	if (lNextStates.size() < 25)depth = 2;
	if (lNextStates.size() < 10)depth = 3;
	if (lNextStates.size() < 5)depth = 4;
	depth = 0;

    if (lNextStates.size() == 0) return GameState(pState, Move());

    /*
     * Here you should write your clever algorithms to get the best next move, ie the best
     * next state. This skeleton returns a random move instead.
     */

	 // From all the possible next states, choose the one with the highest v
	for (auto i : lNextStates) {
		//if (pDue.now().getSeconds() > pDue.getSeconds() - 0.15) {
		//	break;
		//}
		int v = INT_MIN;
		v = alphaBeta(i, depth, alpha, beta, nextPlayer, pDue);
		if (v > optValue) {
			optValue = v;
			optValIndex = i;
		}
	}
	return optValIndex; // Return the best possible state
    
    //return lNextStates[rand() % lNextStates.size()];
}


/* The heuristic function to choose the next state:
It is based on comparing how many winning possible states exist for each of the players */
int heur(const GameState& pState, uint8_t  player) {

	// Variables
	int playerPoints;
	int opponentPoints;
	int accumulPoints = 0;

	// Similar to gamestate.hpp: It checks whether X won and assigns a big reward. If X lost assigns 
	// big negative reward. Otherwise possibly means draw, so no reward
	if (pState.isEOG()) {

		if (pState.isXWin()) {
			return 1000000;
		}
		else if (pState.isOWin()) {
			return -1000000;
		}
		else {
			return 0;
		}
	}

	// In case we didn't reach the end of the game, then count all possible winning states
	else {

		/*  x: rows
			y: columns
			z: layers*/

		// For every horizontal line and every layer
		for (int z = 0; z < 4; z++) {
			for (int x = 0; x < 4; x++) {
				playerPoints = 0;
				opponentPoints = 0;
				for (int y = 0; y < 4; y++) {
					if (pState.at(y, z, x) == CELL_X) {
						playerPoints += 1;
					}
					else if (pState.at(y, z, x) == CELL_O) {
						opponentPoints += 1;
					}
				}
				if ((opponentPoints != 0) && (playerPoints == 0)) {
					accumulPoints += -pow(10, opponentPoints);
				}
				else if ((opponentPoints == 0) && (playerPoints != 0)) {
					accumulPoints += pow(10, playerPoints);
				}
				else {
					accumulPoints += 0;
				}
			}
		}


		// For every vertical line and every layer
		for (int z = 0; z < 4; z++) {
			for (int x = 0; x < 4; x++) {
				playerPoints = 0;
				opponentPoints = 0;
				for (int y = 0; y < 4; y++) {
					if (pState.at(x, y, z) == CELL_X) {
						playerPoints += 1;
					}
					else if (pState.at(x, y, z) == CELL_O) {
						opponentPoints += 1;
					}
				}
				if ((opponentPoints != 0) && (playerPoints == 0)) {
					accumulPoints += -pow(10, opponentPoints);
				}
				else if ((opponentPoints == 0) && (playerPoints != 0)) {
					accumulPoints += pow(10, playerPoints);
				}
				else {
					accumulPoints += 0;
				}
			}
		}

		// For every horizontal and vertical line 
		for (int z = 0; z < 4; z++) {
			for (int x = 0; x < 4; x++) {
				playerPoints = 0;
				opponentPoints = 0;
				for (int y = 0; y < 4; y++) {
					if (pState.at(z, x, y) == CELL_X) {
						playerPoints += 1;
					}
					else if (pState.at(z, x, y) == CELL_O) {
						opponentPoints += 1;
						;
					}
				}
				if ((opponentPoints != 0) && (playerPoints == 0)) {
					accumulPoints += -pow(10, opponentPoints);
				}
				else if ((opponentPoints == 0) && (playerPoints != 0)) {
					accumulPoints += pow(10, playerPoints);
				}
				else {
					accumulPoints += 0;
				}
			}
		}

		// ******************************************************************************
		// 2D DIAGONALS
		// For every right diagonal of every row
		for (int z = 0; z < 4; z++) {
			playerPoints = 0;
			opponentPoints = 0;
			for (int x = 0; x < 4; x++) {
				if (pState.at(z, x, x) == CELL_X) {
					playerPoints += 1;
				}
				else if (pState.at(z, x, x) == CELL_O) {
					opponentPoints += 1;
				}
			}
			if ((opponentPoints != 0) && (playerPoints == 0)) {
				accumulPoints += -pow(10, opponentPoints);
			}
			else if ((opponentPoints == 0) && (playerPoints != 0)) {
				accumulPoints += pow(10, playerPoints);
			}
			else {
				accumulPoints += 0;
			}
		}

		// For every left diagonal of every row
		for (int z = 0; z < 4; z++) {
			playerPoints = 0;
			opponentPoints = 0;
			for (int x = 0; x < 4; x++) {
				if (pState.at(z, 3-x, x) == CELL_X) {
					playerPoints += 1;
				}
				else if (pState.at(z, 3-x, x) == CELL_O) {
					opponentPoints += 1;
				}
			}
			if ((opponentPoints != 0) && (playerPoints == 0)) {
				accumulPoints += -pow(10, opponentPoints);
			}
			else if ((opponentPoints == 0) && (playerPoints != 0)) {
				accumulPoints += pow(10, playerPoints);
			}
			else {
				accumulPoints += 0;
			}
		}

		// For every right diagonal of every column
		for (int z = 0; z < 4; z++) {
			playerPoints = 0;
			opponentPoints = 0;
			for (int x = 0; x < 4; x++) {
				if (pState.at(x, z, x) == CELL_X) {
					playerPoints += 1;
				}
				else if (pState.at(x, z, x) == CELL_O) {
					opponentPoints += 1;
				}
			}
			if ((opponentPoints != 0) && (playerPoints == 0)) {
				accumulPoints += -pow(10, opponentPoints);
			}
			else if ((opponentPoints == 0) && (playerPoints != 0)) {
				accumulPoints += pow(10, playerPoints);
			}
			else {
				accumulPoints += 0;
			}
		}

		// For every left diagonal of every column
		for (int z = 0; z < 4; z++) {
			playerPoints = 0;
			opponentPoints = 0;
			for (int x = 0; x < 4; x++) {
				if (pState.at(3-x, z, x) == CELL_X) {
					playerPoints += 1;
				}
				else if (pState.at(3-x, z, x) == CELL_O) {
					opponentPoints += 1;
				}
			}
			if ((opponentPoints != 0) && (playerPoints == 0)) {
				accumulPoints += -pow(10, opponentPoints);
			}
			else if ((opponentPoints == 0) && (playerPoints != 0)) {
				accumulPoints += pow(10, playerPoints);
			}
			else {
				accumulPoints += 0;
			}
		}

		// For every right diagonal of every layer
		for (int z = 0; z < 4; z++) {
			playerPoints = 0;
			opponentPoints = 0;
			for (int x = 0; x < 4; x++) {
				if (pState.at(x, x, z) == CELL_X) {
					playerPoints += 1;
				}
				else if (pState.at(x, x, z) == CELL_O) {
					opponentPoints += 1;
				}
			}
			if ((opponentPoints != 0) && (playerPoints == 0)) {
				accumulPoints += -pow(10, opponentPoints);
			}
			else if ((opponentPoints == 0) && (playerPoints != 0)) {
				accumulPoints += pow(10, playerPoints);
			}
			else {
				accumulPoints += 0;
			}
		}

		// For every left diagonal of every layer
		for (int z = 0; z < 4; z++) {
			playerPoints = 0;
			opponentPoints = 0;
			for (int x = 0; x < 4; x++) {
				if (pState.at(x, 3 - x, z) == CELL_X) {
					playerPoints += 1;
				}
				else if (pState.at(x, 3 - x, z) == CELL_O) {
					opponentPoints += 1;
				}
			}
			if ((opponentPoints != 0) && (playerPoints == 0)) {
				accumulPoints += -pow(10, opponentPoints);
			}
			else if ((opponentPoints == 0) && (playerPoints != 0)) {
				accumulPoints += pow(10, playerPoints);
			}
			else {
				accumulPoints += 0;
			}
		}

		// ***************************************************************************
		// For the 4 3D diagonals
		opponentPoints = 0;
		playerPoints = 0;
		for (int x = 0; x < 4; x++) {
			if (pState.at(x, x, x) == CELL_X) {
				playerPoints += 1;
			}
			else if (pState.at(x, x, x) == CELL_O) {
				opponentPoints += 1;
			}
		}
		if ((opponentPoints != 0) && (playerPoints == 0)) {
			accumulPoints += -pow(10, opponentPoints);
		}
		else if ((opponentPoints == 0) && (playerPoints != 0)) {
			accumulPoints += pow(10, playerPoints);
		}
		else {
			accumulPoints += 0;
		}
		opponentPoints = 0;
		playerPoints = 0;
		for (int x = 0; x < 4; x++) {
			if (pState.at(3 - x, x, x) == CELL_X) {
				playerPoints += 1;
			}
			else if (pState.at(3 - x, x, x) == CELL_O) {
				opponentPoints += 1;
			}
		}
		if ((opponentPoints != 0) && (playerPoints == 0)) {
			accumulPoints += -pow(10, opponentPoints);
		}
		else if ((opponentPoints == 0) && (playerPoints != 0)) {
			accumulPoints += pow(10, playerPoints);
		}
		else {
			accumulPoints += 0;
		}
		opponentPoints = 0;
		playerPoints = 0;
		for (int x = 0; x < 4; x++) {
			if (pState.at(x, 3 - x, x) == CELL_X) {
				playerPoints += 1;
			}
			else if (pState.at(x, 3 - x, x) == CELL_O) {
				opponentPoints += 1;
			}
		}
		if ((opponentPoints != 0) && (playerPoints == 0)) {
			accumulPoints += -pow(10, opponentPoints);
		}
		else if ((opponentPoints == 0) && (playerPoints != 0)) {
			accumulPoints += pow(10, playerPoints);
		}
		else {
			accumulPoints += 0;
		}
		opponentPoints = 0;
		playerPoints = 0;
		for (int x = 0; x < 4; x++) {
			if (pState.at(x, x, 3 - x) == CELL_X) {
				playerPoints += 1;
			}
			else if (pState.at(x, x, 3 - x) == CELL_O) {
				opponentPoints += 1;
			}
		}
		if ((opponentPoints != 0) && (playerPoints == 0)) {
			accumulPoints += -pow(10, opponentPoints);
		}
		else if ((opponentPoints == 0) && (playerPoints != 0)) {
			accumulPoints += pow(10, playerPoints);
		}
		else {
			accumulPoints += 0;
		}

		return accumulPoints;
	}

}

// The alphaBeta algorithm
int alphaBeta(const GameState& pState, int depth, int alpha, int beta, uint8_t player, const Deadline &pDue) {

	int v = 0;

	std::vector<GameState> nextStates;
	pState.findPossibleMoves(nextStates);

	if (depth == 0 || nextStates.size() == 0) {
		v = heur(pState, player);
	}
	else if (player == CELL_O) {
		v = INT_MIN;
		for (auto child : nextStates) {
			//if (pDue.now().getSeconds() > pDue.getSeconds() - 0.15) {
			//	break;
			//}
			int temp = alphaBeta(child, depth - 1, alpha, beta, CELL_O, pDue);
			v = (v > temp) ? v : temp;
			alpha = (alpha > v) ? alpha : v;
			if (beta <= alpha) {
				break;
			}
		}
	}
	else if (player == CELL_X) {
		v = INT_MAX;
		for (auto child : nextStates) {
			int temp = alphaBeta(child, depth - 1, alpha, beta, CELL_X, pDue);
			v = (v < temp) ? v : temp;
			beta = (beta < v) ? beta : v;
			if (beta <= alpha) {
				break;
			}
		}
	}
	return v;
}


/*namespace TICTACTOE3D*/ }
