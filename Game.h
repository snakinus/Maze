#pragma once

#include "Maze.h"
#include "Player.h"
#include <chrono>
#include <thread>
#include <curses.h>
#include <algorithm>
#include <cmath>

using namespace std;

// game modes
enum Mode { Solo, Computer, Battle }; 

class Game {
	int mode;					// chosen mode of game
	Maze* maze;					// maze pointer
	Human* human;				// human player pointer
	AI* ai;						// ai pointer
	vector<Player*> players;	// vector of player pointers (contains max 2 players - ai and player)
	int speed = 200;			// how many ms must pass for next AI move
public:
	// constructor for creating a game
	Game(int option) {
		// creates new maze and setting mode 
		maze = new Maze();
		mode = option;
		
		// increases speed for 'AI' mode
		if (option == Computer) speed = 50;
	}

	// deletes dynamically allocated variables 
	// (deleting players from vector is not necessary, becuase there are only ai or/and human in it)
	~Game() {
		delete human;
		delete ai;
		delete maze;
	}

	// adding human or/and AI depending on mode and starting a game
	void play() {
		switch (mode) {
		case Battle:
			addHuman();
			addAI();
			break;
		case Computer:
			addAI();
			break;
		default:
			addHuman();
			break;
		}

		start();
	}

	// initializes a human player and adding him to the vector of players
	void addHuman() {
		human = new Human((const char**)maze->getBoard(), maze->getRows(), maze->getColumns());
		players.push_back(human);
	}	

	// initializes an AI player and adding him to the vector of players
	void addAI() {
		ai = new AI((const char**)maze->getBoard(), maze->getRows(), maze->getColumns());
		players.push_back(ai);
	}

	// returns true only if any of the players has the position of lower right corner
	bool endOfGame() {
		for (auto p : players)
			if (p->y >= maze->getRows() - 1 && p->x >= maze->getColumns() - 2) 
				return true;

		return false;
	}

	// starts a game
	void start() {
		// beginning of time measurments for determining if AI can make next move
		auto start_time = std::chrono::high_resolution_clock::now();

		while (!endOfGame()) {

			// if mode is not equal to 'AI', then player can make a move
			if (mode != Computer) {
				// player makes a move or decide to quit game 
				// or go to main menu by changing the value of global 'goBackToMenu' variable
				human->move();

				// if variable was set to 'true', then it is reseted and program returns to menu
				if (goBackToMenu) {
					goBackToMenu = false;
					return;
				}
			}
			// if mode is equal to 'AI' then player cannot make a move
			// but still must be able to go to main menu or quit game
			else {
				int key = getch();
				switch (key) {
				case 'q':
					endwin();
					exit(0);
					break;
				case 'm':
					return;
				default:
					break;
				}
			}
			
			// clear screen and print board
			clear();
			maze->printBoard(players);
			refresh();

			// if AI doesn't play, then there is no need to check for timing of his next move
			if (mode == Solo) continue;

			// check how many ms passed after last move of AI
			auto end_time = std::chrono::high_resolution_clock::now();
			auto time = end_time - start_time;

			// if enough ms passed, then AI moves and beginning of time measurment is updated to current time
			if (time / std::chrono::milliseconds(1) >= speed) {
				ai->move();
				start_time = std::chrono::high_resolution_clock::now();
			}
		}
	}
};