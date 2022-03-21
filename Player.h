#pragma once

#include <algorithm>
#include <curses.h>
#include <vector>
#include <cmath>
#include <string>
#include "Utilities.h"

using namespace std;

// global variable for going back to menu, 
// which is necessery for communication between Menu and Player. 
// I decided it is lesser evil to leave it than to try to refactor it
bool goBackToMenu = false;
 
class Player {
protected:

	// const variables containing info about the board
	const char** board; // board content
	const int N;		// number of rows in a board
	const int M;		// number of cols in a board

	// checks if given coords are inside of a board and if tile is empty
	bool validTile(int y, int x) {
		return (0 <= y && y <= N - 1) && (0 <= x && x <= M - 1) && board[y][x] == ' ';
	}
public:
	// information about the player
	int symbol = 9608;				// symbol displayed on a screen
	int color = COLOR_WHITE;		// color of a player
	int pairNumber = PLAYER_PAIR;	// number of a player's color pair
	int y = 1;						// y coord of a player
	int x = 1;						// x coord of a player
	virtual void move() = 0;		// abstract function for moving - inheriting classes have to implement it

	// constructor for player
	Player(const char ** board, int N, int M) : board(board), N(N), M(M) {
		init_pair(PLAYER_PAIR, color, COLOR_BLACK);
	}
};

class Human : public Player {
	// returns y + 1 if player can go down, if not returns y
	int down() {
		if (y + 1 <= N - 1) y = y + 1;
		if (board[y][x] != ' ') y = y - 1;
		return y;
	}

	// returns y + 1 if player can go up, if not returns y
	int up() {
		if (y - 1 >= 0) y = y - 1;
		if (board[y][x] != ' ') y = y + 1;
		return y;
	}

	// returns x - 1 if player can go left, if not returns x
	int left() {
		if (x - 1 >= 0) x = x - 1;
		if (board[y][x] != ' ') x = x + 1;
		return x;
	}

	// returns x + 1 if player can go right, if not returns x
	int right() {
		if (x + 1 <= M - 1) x = x + 1;
		if (board[y][x] != ' ') x = x - 1;
		return x;
	}
public:
	// constructor for human player
	Human(const char** board, int N, int M) : Player(board, N, M) {
		init_pair(HUMAN_PAIR, color, COLOR_BLACK);
		pairNumber = HUMAN_PAIR;
	}

	// changes player coords or goes back to menu or exits game
	// depending on user input
	void move() {
		int key = getch();
		switch (key) {
		case 'q':		// quit
			endwin();
			exit(0);
			break;
		case 'm':		// change goBackToMenu to true
			goBackToMenu = true;
			break;
		case KEY_UP:	// go up
			y = up();
			break;
		case KEY_DOWN:	// go down
			y = down();
			break;
		case KEY_LEFT:	// go left
			x = left();
			break;
		case KEY_RIGHT: // go right
			x = right();
			break;
		}
	}
};

class AI : public Player {
	// array of direction on X and Y axes
	int dirX[4] = { 0, 0, 1, -1 };
	int dirY[4] = { 1, -1, 0, 0 };

	// array of indexes telling in which order AI will search his neighbours
	int idx[4] = { 0, 1, 2, 3 };
	
	// 2d array of visited nodes
	bool** visited;

	// vector containing nodes which where previously visited 
	vector<pair<int, int> > before;

	// vector containing full solution to solving a maze using a dfs
	vector<pair<int, int> > solution;
public:
	AI(const const char** board, int N, int M) : Player(board, N, M) {
		// changes ai color and inits it
		color = 173;
		init_pair(AI_PAIR, color, COLOR_BLACK);
		pairNumber = AI_PAIR;


		// randomize order of AI searches
		int direction = rand() % 24; // 4!;
		while (direction--) next_permutation(idx, idx + 4);

		// allocate new array of visited nodes of dimensions N x M (same as board)
		visited = new bool* [N];
		for (int i = 0; i < N; ++i)
			visited[i] = new bool[M];

		// clear array of visited nodes
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				visited[i][j] = false;
			}
		}

		// solve the maze 
		solve();

		// clear vector of previous nodes, because now it is useless
		before.clear();

		// reverse vector of solutions so by taking elements from the back
		// we take them in chronological order of their input
		reverse(solution.begin(), solution.end());

		// set coords of AI to starting tile, because it was changed during solve function
		x = y = 1;
	}

	// destructs array of visited nodes
	~AI() {
		for (int i = 0; i < N; ++i)
			delete visited[i];
		delete[] visited;

	}

	// solves the maze
	void solve() {
		// until maze is not solved - move and push the node to the solution vector
		while (!(y == N - 1 && x == M - 2)) {
			moveSolve();
			solution.push_back({ y, x });
		}
	}

	// makes one move in actual game with the help of previously filled solution vector
	void move() {
		pair<int, int> coords = solution.back();
		y = coords.first;
		x = coords.second;
		solution.pop_back();
	}

	// makes one move before the game 
	void moveSolve() {
		// set node as visited
		visited[y][x] = true;

		// check for unvisited neighbours in previously drawn order
		for (int i = 0; i < 4; i++) {
			int _y = y + dirY[idx[i]];
			int _x = x + dirX[idx[i]];

			if (!validTile(_y, _x)) continue;

			// if the node was not visited then put in stack current coords
			// and then changes coords to those of this unvisited node 
			if (!visited[_y][_x]) {
				before.push_back({ y, x });
				y = _y;
				x = _x;
				return;
			}
		}

		// if all of the neighbours were visited, then set your coords to your father's
		// and remove him from the stack
		pair<int, int> coords = before.back();
		y = coords.first;
		x = coords.second;
		before.pop_back();
	}
};