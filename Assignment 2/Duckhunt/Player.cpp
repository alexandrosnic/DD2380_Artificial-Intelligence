#include "Player.hpp"
#include "hmm.hpp"
#include <cstdlib>
#include <iostream>
#include <queue>

using namespace std;


namespace ducks
{

	ESpecies getBirdSpecies(int i) {
		switch (i)
		{
		case 0:
			return SPECIES_PIGEON;
		case 1:
			return SPECIES_RAVEN;
		case 2:
			return SPECIES_SKYLARK;
		case 3:
			return SPECIES_SWALLOW;
		case 4:
			return SPECIES_SNIPE;
		case 5:
			return SPECIES_BLACK_STORK;
		default:
			return SPECIES_UNKNOWN;
		}
	}

	EMovement getMovementType(int i) {
		switch (i)
		{
		case -1:
			return MOVE_DEAD;
		case 0:
			return MOVE_UP_LEFT;
		case 1:
			return  MOVE_UP;
		case 2:
			return MOVE_UP_RIGHT;
		case 3:
			return MOVE_LEFT;
		case 4:
			return MOVE_STOPPED;
		case 5:
			return MOVE_RIGHT;
		case 6:
			return MOVE_DOWN_LEFT;
		case 7:
			return MOVE_DOWN;
		case 8:
			return MOVE_DOWN_RIGHT;
		default:
			return MOVE_DEAD;
		}
	}

	class BirdData {
	public:
		ducks::Bird bird;
		ducks::ESpecies species;
	};


	Player::Player()
	{
	}

	int numOfShots = 0;
	volatile int birdIndex = 0;
	vector<HMMFunction::HMM> birdModel;


	Action Player::shoot(const GameState& pState, const Deadline& pDue)
	{

		 /*
		 ***********************************************************************************
											DESCRIPTION
		 There will be up to 10 rounds in each environment. Each round contains 1-20 birds and goes on
		 for 100 time steps. The round will end when all birds are shot or when time runs out.
		 Every birdIndex in the sky makes a move for every discrete time step that the game runs.
		 No moves are reported for birds that have been shot.
		 ***********************************************************************************
		 */

		//**********************************************************************************
		// Wait for data generation before decision-making
		// 1. Wait 1 rounds to get more data
		if (pState.getRound() < 1) {
			return cDontShoot;
		}
		// 2. Check if the bird is dead or alive
		while (pState.getBird(birdIndex).isDead()) {
			birdIndex++;
			birdIndex = birdIndex % pState.getNumBirds();
		}
		// 3. Wait till the birdIndex movement sequence is large enough to increase prob
		if (pState.getBird(birdIndex).getSeqLength() < 70) {
			birdIndex++;
			birdIndex = birdIndex % pState.getNumBirds();
			return cDontShoot;
		}
		if (pDue.remainingMs() < 30) return cDontShoot;

		// ******************************************************************************************
		// Birdindex becomes get assigned to class bird
		Bird currentBird = pState.getBird(birdIndex);
		// The sequence of the current bird
		int currentObs[currentBird.getSeqLength()];

		// ******************************************************************************************
		// Build the sequence for the bird we are focusing on, that includes its movements
		// We just check one bird at a time.
		// Check wether invalid movements
		for (int j = 0; j < currentBird.getSeqLength(); j++) {
			if (currentBird.getObservation(j) > 8 || currentBird.getObservation(j) < -1) {
				cerr << "INVALID MOVEMENT: " << (int)currentBird.getObservation(j) << endl;
				currentObs[j] = 0;
				return cDontShoot;
			}
			else {
				// Here we build the sequence
				currentObs[j] = (int)currentBird.getObservation(j);
			}
		}

		// ******************************************************************************************
		// Iterate through the bird models that we builded so far, calc the prob of bird seq  (hmm1)
		// and assign to it the model which gives the highest prob. 
		// we compare through the first birdModel 0 and the last one.
		double prob = 0;
		int maxModelIndex = 0;
		for (int k = 0; k < birdModel.size(); k++) {
			double tempProb = birdModel[k].alphaRun(currentObs, (int)currentBird.getSeqLength());
			if (prob < tempProb) {
				prob = tempProb;
				// Assign to the bird the species with the highest identification prob
				maxModelIndex = k;
			}
		}
		// Get the species of the bird
		ESpecies birdSpecies = birdModel[maxModelIndex].bird;

		// ******************************************************************************************
		// Run a Baum-Welch to learn the bird's model
		HMMFunction::HMM testHMM;
		testHMM.T = pState.getBird(birdIndex).getSeqLength();
		for (int j = 0; j < pState.getBird(birdIndex).getSeqLength(); j++) {
			testHMM.sequence[j] = (int)pState.getBird(birdIndex).getObservation(j);
		}
		testHMM.run(5000);

		// ******************************************************************************************
		// Get the bird's next move with the (HMM0 - Probability distribution) algorithm
		int move = testHMM.birdNextMove();

		int currentB = birdIndex;
		birdIndex++;;
		birdIndex = birdIndex % pState.getNumBirds();
		// ******************************************************************************************
		// If that's a valid bird: not black stork, and we're sure about its identity and move, KILL IT
		if (birdSpecies != SPECIES_BLACK_STORK && testHMM.boolConverge == 1 && move != -1) {
			cerr << "Shooting" << "   bird: " << birdIndex << ", seqLength: " << pState.getBird(birdIndex).getSeqLength() << endl;
			numOfShots++;
			return Action(currentB, (EMovement)move);
		}
		else {
			return cDontShoot;
		}
	}


	// For FIFO
	queue<BirdData> birdQueue;
	int moveObs[20][100];
	// ******************************************************************************************
	// Guess the bird
	vector<ESpecies> Player::guess(const GameState& pState, const Deadline& pDue)
	{
		/*
		 * Here you should write your clever algorithms to guess the species of each bird.
		 * This skeleton makes no guesses, better safe than sorry!
		 */

		cerr << "*********GUESS THE BIRD""*********"<< endl;
		// ******************************************************************************************
		// Get more trained data on the birds that have still to be revealed from the previous round
		for (int i = 0; i < pState.getNumBirds() && pDue.remainingMs() > 400 && !birdQueue.empty(); i++) {

			// Get the next bird from the queue (that has been added because we didn't revealed it)
			BirdData tempBird = birdQueue.front();
			// and remove it from the queue
			birdQueue.pop();

			// Get the bird's observation
			HMMFunction::HMM testHMM;
			testHMM.T = tempBird.bird.getSeqLength();
			for (int j = 0; j < pState.getBird(i).getSeqLength(); j++) {
				testHMM.sequence[j] = (int)tempBird.bird.getObservation(j);
			}
			// Run a Baum-Welch on the bird model to get more trained data
			testHMM.run(5000);
			// Assign the species you revealed in the previous round for that bird
			testHMM.bird = tempBird.species;

			// If converged add the bird model to our HMM bird models vector
			if (testHMM.boolConverge == 1) {
				birdModel.push_back(testHMM);
			}
		}

		// ******************************************************************************************
		// Guess the species of the birds that ranked higher in the probability for the next move
		vector<ESpecies> spiecesGuess(pState.getNumBirds());

		int maxModelIndex = 0;
		double tempProb = 0.0;
		double prob = 0;
		// For every bird
		for (int i = 0; i < pState.getNumBirds() && pDue.remainingMs() > 400; i++) {
			prob = 0;
			// Get its previous observations
			for (int j = 0; j < pState.getBird(i).getSeqLength(); j++) {
				moveObs[i][j] = (int)pState.getBird(i).getObservation(j);
			}
			// And find the HMM model that gives the most probable next move,
			// by running an alpha-forward algorithm
			for (int j = 0; j < birdModel.size(); j++) {
				tempProb = birdModel[j].alphaRun(moveObs[i], pState.getBird(i).getSeqLength());
				if (tempProb > prob) {
					prob = tempProb;
					maxModelIndex = j;
				}
			}
			// Assign the bird species with the highest probability to our spieces vector
			if (!birdModel.empty()) {
				spiecesGuess[i] = birdModel[maxModelIndex].bird;
			}
		}
		return spiecesGuess;
	}


	// ******************************************************************************************
	// Did we hit the bird?
	int killBird = 0;
	void Player::hit(const GameState& pState, int pBird, const Deadline& pDue)
	{
		/*
		 * If you hit the bird you are trying to shoot, you will be notified through this function.
		 */
		killBird++;
		cerr << "HIT BIRD!!!" << endl;
	}



	// ******************************************************************************************
	// Reveal if we guessed the bird right
	void Player::reveal(const GameState& pState, const vector<ESpecies>& pSpecies, const Deadline& pDue)
	{
		/*
		 * If you made any guesses, you will find out the true species of those birds in this function.
		 */
		cerr << "*********REVEAL THE BIRD""*********"<< endl;
		int lastBird = 0;
		int i = 0;

		// ******************************************************************************************
		// Reveal the species of the birds of the current round and build again their models
		while (i < pState.getNumBirds() && pDue.remainingMs() > 400) {
			if (pSpecies[i] != SPECIES_UNKNOWN) {

				// Get the current bird and its observation and store it to our model
				Bird currentBird = pState.getBird(i);
				HMMFunction::HMM testHMM; 
				testHMM.T = currentBird.getSeqLength();
				for (int j = 0; j < currentBird.getSeqLength(); j++) {
					testHMM.sequence[j] = (int)currentBird.getObservation(j);
				}

				// Train the bird's model (with known species) with the Baum Welch algorithm to get trained data
				testHMM.bird = pSpecies[i];
				testHMM.run(5000);
				if (testHMM.boolConverge == 1) {
					birdModel.push_back(testHMM);
				}
				lastBird++;
			}
			i++;
		}
		// Fill up the rest of the birds' species and add them to the queue
		while (lastBird < (int)pState.getNumBirds() && pDue.remainingMs() > 100) {
			BirdData currentBird;
			currentBird.species = pSpecies[lastBird];
			currentBird.bird = pState.getBird(lastBird);
			birdQueue.push(currentBird);
			lastBird++;
		}
		cerr << "Num. of shots: " << numOfShots << ", num. of hits: " << killBird << endl;
	}
} /*namespace ducks*/
