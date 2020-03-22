#include "player.hpp"
#include "gamestate.hpp"
#include "move.hpp"
#include <cstdlib>
#include<bits/stdc++.h> 

struct states {
	checkers::GameState state;
	double points;
};


namespace checkers
{

// ********************************************************************
// Define the functions
bool compareStatesMax(struct states st1, struct states st2) { return (st1.points > st2.points); }
bool compareStatesMin(struct states st1, struct states st2) { return (st1.points < st2.points); }
int alphaBeta(const GameState& pState, int depth, int alpha, int beta, uint8_t player, const Deadline& pDue);
int	heuristic(const GameState& pState, uint8_t player);

//*********************************************************************
// Heuristic function: It gives big rewards to winning states
int	heuristic(const GameState& pState, uint8_t player){
	int h = 0;
	int jumpOver=12;
	int countOpp;

	for (int j = 0; j <= 31; j++) {
		if (player == CELL_RED){
			countOpp++;
		}
	}

	// If it's a jump move
	if (jumpOver != countOpp) {
		// Then assign big reward if he's white and king
		if (player == CELL_WHITE && player == CELL_KING) {
			h += 4;
		}
		// Assign smaller reward if he's just white 
		else if (player == CELL_WHITE){
			h += 3;
		}
		// Assign big negative reward if he's king and red
		if (player == CELL_RED && player == CELL_KING) {
			h -= 4;
		}
		// Assign smaller negative reward if he's just red 
		else if (player == CELL_RED) {
			h -= 3;
		}
	}
	else { // If it's a normal move
		// Then assign big reward if he's white and king
		if (player == CELL_WHITE && player == CELL_KING) {
			h += 2;
		}
		// Assign smaller reward if he's just white 
		else if (player == CELL_WHITE) {
			h += 1;
		}
		// Assign big negative reward if he's king and red
		if (player == CELL_RED && player == CELL_KING) {
			h -= 2;
		}
		// Assign smaller negative reward if he's just red 
		else if (player == CELL_RED) {
			h -= 1;
		}
	}
	jumpOver = countOpp;

	
	return h;
}

//*********************************************************************
// AlphaBeta function
int	alphaBeta(const GameState& pState, int depth, int alpha, int beta, uint8_t player, const Deadline& pDue) {

	int v = 0;
	std::vector<GameState> nextStates;
	pState.findPossibleMoves(nextStates);

	if (depth == 0 || nextStates.size() == 0 || pState.isEOG() || pDue.getSeconds() - pDue.now().getSeconds() < 0.01){
		if (pState.isWhiteWin()) return INT32_MAX;
		else if (pState.isRedWin()) return INT32_MIN;
		else if (pState.isDraw()) return 0;
		else return heuristic(pState, player);
	}
	else if (player == CELL_WHITE) {
		v = INT_MIN;
		for (auto child : nextStates) {
			int temp = alphaBeta(child, depth - 1, alpha, beta, CELL_RED, pDue);
			v = (v > temp) ? v : temp;
			alpha = (alpha > v) ? alpha : v;
			if (beta <= alpha) {
				break;
			}
		}
		return v;
	}
	else { //if (player == CELL_RED ) 
		v = INT_MAX;
		for (auto child : nextStates) {
			int temp = alphaBeta(child, depth - 1, alpha, beta, CELL_WHITE, pDue);
			v = (v < temp) ? v : temp;
			beta = (beta < v) ? beta : v;
			if (beta <= alpha) {
				break;
			}
		}
		return v;
	}
}

//*********************************************************************
// Play function
GameState Player::play(const GameState& pState, const Deadline& pDue)
{
	std::vector<GameState> lNextStates;
	pState.findPossibleMoves(lNextStates);
	if (lNextStates.size() == 0) return GameState(pState, Move());
	if (lNextStates.size() == 1) return lNextStates[rand() % lNextStates.size()];
	std::vector<states> nextStates(lNextStates.size());

	for (int i = 0; i < (int)lNextStates.size(); i++) {
		nextStates[i].state = lNextStates[i];
		nextStates[i].points = 0;
	}
	int depth = 1;
	GameState optimumState = lNextStates[0];  // The optimum State
	uint8_t nextPlayer = pState.getNextPlayer();	// The next player

	double optimumStateIndex;	// The state corresponding to the optimum value for v

	while ((pDue.getSeconds() - pDue.now().getSeconds()) > 0.00001 && depth <= 15) {

		double alpha = INT_MIN;	  // alpha
		double beta = INT_MAX;	// beta
		double optValue = 0;	// The current optimum value for v
		optimumStateIndex = 0;

		if (nextPlayer == CELL_WHITE) {
			optValue = INT_MIN;
			std::sort(nextStates.begin(), nextStates.end(), compareStatesMax);
			for (int i = 0; i < (int)nextStates.size(); i++) {
				nextStates[i].points = alphaBeta(nextStates[i].state, depth, alpha, beta, CELL_RED, pDue);
				if (nextStates[i].points > optValue) {
					optValue = nextStates[i].points;
					optimumStateIndex = i;
				}
				alpha = std::max(alpha, nextStates[i].points);
				if (beta <= alpha) break;
			}
		}
		else {
			optValue = INT_MAX;
			std::sort(nextStates.begin(), nextStates.end(), compareStatesMin);
			for (int i = 0; i < (int)nextStates.size(); i++) {
				nextStates[i].points = alphaBeta(nextStates[i].state, depth, alpha, beta, CELL_WHITE, pDue);
				if (nextStates[i].points < optValue) {
					optValue = nextStates[i].points;
					optimumStateIndex = i;
				}
				beta = std::min(beta, nextStates[i].points);
				if (beta <= alpha) break;
			}
		}
		optimumState = nextStates[optimumStateIndex].state;
		depth++;
	}
	return optimumState; // Return the best possible state
}


/*namespace checkers*/ }
