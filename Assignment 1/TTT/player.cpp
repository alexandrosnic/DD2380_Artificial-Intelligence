#include "player.hpp"
#include <cstdlib>
#include <cmath>


namespace TICTACTOE
{

int alphaBeta(const GameState& pState, int depth, int alpha, int beta, uint8_t player);

GameState Player::play(const GameState &pState,const Deadline &pDue)
{
    //std::cerr << "Processing " << pState.toMessage() << std::endl;


	// Variables
	uint8_t nextPlayer = pState.getNextPlayer();	// The next player
	int depth = 2;	// Depth of the tree
	int alpha = -1000000;	  // alpha
	int beta = 10000000;	// beta
	int optValue = -1000000;	// The current optimum value for v
	GameState optValIndex;	// The state corresponding to the optimum value for v

	// The next possible states
    std::vector<GameState> lNextStates;		
    pState.findPossibleMoves(lNextStates);

	// If we explored all the states, return the one we are in:
    if (lNextStates.size() == 0) return GameState(pState, Move());

	/*
	 * Here you should write your clever algorithms to get the best next move, ie the best
	 * next state. This skeleton returns a random move instead.
	 */


	// From all the possible next states, choose the one with the highest v
	for (auto i : lNextStates) {
		int v = 0;
		v = alphaBeta(i, depth, alpha, beta, nextPlayer);
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
	int accumulPoints;

	// Similar to gamestate.hpp: It checks whether X won and assigns a big reward. If X lost assigns 
	// big negative reward. Otherwise possibly means draw, so no reward
	if (pState.isEOG()) {

		if (pState.isXWin() ) {
			return 100000;
		}
		else if (pState.isOWin() ) {
			return -100000;
		}
		else {
			return 0;
		}
	}

	// In case we didn't reach the end of the game, then count all possible winning states
	else {

		// For every horizontal line
		for (int i = 0; i < 4; i++) {
			playerPoints = 0;
			for (int j = 0; j < 4; j++) {
				if (pState.at(i, j) == CELL_X) {
					playerPoints += 1;
				}
				else if (pState.at(i, j) == CELL_O) {
					playerPoints = 0;
					break;
				}
			}
			accumulPoints += (playerPoints == 0) ? 0 : pow(10, playerPoints);
		}

		// For every vertical line
		for (int i = 0; i < 4; i++) {
			playerPoints = 0;
			for (int j = 0; j < 4; j++) {
				if (pState.at(j, i) == CELL_X) {
					playerPoints += 1;
				}
				else if (pState.at(j, i) == CELL_O) {
					playerPoints = 0;
					break;
				}
			}
			accumulPoints += (playerPoints == 0) ? 0 : pow(10, playerPoints);
		}

		// For right diagonal line
		playerPoints = 0;
		for (int i = 0; i < 4; i++) {
			if (pState.at(i, i) == CELL_X) {
				playerPoints += 1;
			}
			else if (pState.at(i, i) == CELL_O) {
				playerPoints = 0;
				break;
			}
		}
		accumulPoints += (playerPoints == 0) ? 0 : pow(10, playerPoints);

		// For left diagonal line
		playerPoints = 0;
		for (int i = 0; i < 4; i++) {
			if (pState.at(i, 3 - i) == CELL_X) {
				playerPoints += 1;
			}
			else if (pState.at(i, 3 - i) == CELL_O) {
				playerPoints = 0;
				break;
			}
		}
		accumulPoints += (playerPoints == 0) ? 0 : pow(10, playerPoints);

		return accumulPoints;
	}

}

// The alphaBeta algorithm
int alphaBeta(const GameState& pState, int depth, int alpha, int beta, uint8_t player){

	int v = 0;

	std::vector<GameState> nextStates;
	pState.findPossibleMoves(nextStates);

	if (depth == 0 || nextStates.size() == 0) {
		v = heur(pState, player);
	}
	else if (player == CELL_O) {
		v = -10000000;
		for (auto child : nextStates) {
			int temp = alphaBeta(child, depth - 1, alpha, beta, CELL_O);
			v = (v > temp) ? v : temp;
			alpha = (alpha > v) ? alpha : v;
			if (beta <= alpha){
				break;
			}
		}
	}
	else if (player == CELL_X) {
		v = 10000000;
		for (auto child : nextStates) {
			int temp = alphaBeta(child, depth - 1, alpha, beta, CELL_X);
			v = (v < temp) ? v : temp;
			beta = (beta < v) ? beta : v;
			if (beta <= alpha){
				break;
			}
		}
	}
	return v;
}


/*namespace TICTACTOE*/ }
