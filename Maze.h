#pragma once

#include"Player.h"
#include"Utilities.h"
#include<cstdlib>
#include<ctime>
#include<iostream>
#include<algorithm>
#include<chrono>
#include<curses.h>
#include<vector>

using namespace std;

// enum for tiles in maze
enum Tiles { BOARD_TILE = 97 };

class Maze {
	static const int N = 20;	// number of rows in a board
	static const int M = 30;	// number of cols in a board
	char** board;				// board content

	// array of direction on X and Y axes
	int dirX[4] = { 0, 0, 1, -1 };
	int dirY[4] = { 1, -1, 0, 0 };

	// array of visited nodes
	bool visited[N][M] = { 0 };
	
	// indentation
	int tc; // number of spaces required to center the object
	int tr; // number of new lines required to center the object

	// returns true if given coards are inside the board (not on the borders)
	bool validCoords(int y, int x) {
		return (1 <= y && y < N - 1) && (1 <= x && x < M - 1);
	}

	// 
	bool validSurroundings(int y, int x, int prevY, int prevX) {
		if (visited[y][x]) return false;

		for (int i = 0; i < 4; i++) {
			int _y = y + dirY[i];
			int _x = x + dirX[i];

			if (_y == prevY && _x == prevX) continue;
			if (!validCoords(_y, _x)) continue;
				
			if(visited[_y][_x]) return false;
		}

		return true;
	}

	// creates board using modified dfs algorithm
	void dfs(int y, int x) {
		// set node as visited
		visited[y][x] = true;

		// set node symbol as space
		board[y][x] = ' ';

		// randomize order of searches
		int idx[4] = { 0, 1, 2, 3 };
		int direction = rand() % 24; // 4!;
		while (direction--) next_permutation(idx, idx + 4);

		// check neighbours
		for (int i = 0; i < 4; i++) {
			int _y = y + dirY[idx[i]];
			int _x = x + dirX[idx[i]];

			if (!validCoords(_y, _x)) continue;
			if (visited[_y][_x]) continue;
			if (!validSurroundings(_y, _x, y, x)) continue;

			// if neighbour has valid coords, wasn't visited, and does not have neighbours
			// which were already visited beside his father, then recursively invoke dfs in his coords
			// (because we want to make a maze - in each valid tile there should be two or three borders)
			dfs(_y, _x);
		}
	}

	// initializes pseudo-number generator and creates board
	void createBoard() {
		srand(time(NULL));
		dfs(1, 1);
	}

public:
	// constructor for a Maze
	Maze() {
		// allocate board array
		board = new char* [N];
		for (int i = 0; i < N; ++i)
			board[i] = new char[M];

		// fill it with borders
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				board[i][j] = BOARD_TILE;
			}
		}

		// empty some tiles near starting and ending tile
		board[1][1] = ' ';
		board[1][2] = ' ';
		board[N - 2][M - 3] = ' ';
		board[N - 2][M - 2] = ' ';
		board[N - 1][M - 2] = ' ';

		// create board
		createBoard();

		// read terminal size and set the variables for indetation  
		get_terminal_size(tc, tr);
		tc = tc / 2 - M;
		tr = tr / 2 - N / 2;
	}

	// deletes dynamically allocated board
	~Maze() {
		for (int i = 0; i < N; ++i)
			delete board[i];
		delete [] board;
	}

	// returns a board
	char** getBoard() {
		return board;
	}

	// prints a board
	void printBoard(vector<Player*> players) {
		// change text color
		attron(COLOR_PAIR(NORMAL_TEXT_PAIR));

		// add hints for the player
		addch('\n');
		for (int j = 0; j < (tc + M) * 2 - 20; j++) addch(' ');
		addstr("q - quit game\n");
		for (int j = 0; j < (tc + M) * 2 - 20; j++) addch(' ');
		addstr("m - go to menu\n");

		// return text color
		attron(COLOR_PAIR(STANDARD_PAIR));

		// add new line indentation
		for (int j = 0; j < tr - 3; j++) addch('\n');

		//display board - displays characters twice for better visibility
		for (int i = 0; i < N; i++) {
			
			// add space indentation with standard color
			attron(COLOR_PAIR(STANDARD_PAIR));
			for (int j = 0; j < tc; j++) addch(' ');
			attron(COLOR_PAIR(BOARD_PAIR));

			// display row of a board
			for (int j = 0; j < M; j++) {

				// check if any player is on the board
				bool player_here = false;
				for (auto p : players) {
					// if there is, display him with his color,
					// change boolean to true and break
					if (p->y == i && p->x == j) {
						attron(COLOR_PAIR(p->pairNumber));
						addch(p->symbol), addch(p->symbol);
						player_here = true;
						break;
					}
				}
				// if there was a player continue, becuase he covers a tile
				if (player_here) continue;

				// if current tile is a space, display it twice with PATH_PAIR color
				if (board[i][j] == ' ') {
					attron(COLOR_PAIR(PATH_PAIR));
					addch(board[i][j]), addch(board[i][j]);
				}
				// if not, display it twice with BOARD_PAIR color
				else {
					attron(COLOR_PAIR(BOARD_PAIR));
					addch(board[i][j] | A_ALTCHARSET), addch(board[i][j] | A_ALTCHARSET);
				}
			}
			addch('\n');
		}

	}

	// return number of cols
	int getColumns() {
		return M;
	}

	// return number of rows
	int getRows() {
		return N;
	}

};