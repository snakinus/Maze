#pragma once

#include<string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(__linux__)
#include <sys/ioctl.h>
#endif // Windows/Linux

// gets terminal size and saves it in parameters of function 
// works on Windows and on Linux
void get_terminal_size(int& width, int& height) {
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	width = (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
	height = (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
#elif defined(__linux__)
	struct winsize w;
	ioctl(fileno(stdout), TIOCGWINSZ, &w);
	width = (int)(w.ws_col);
	height = (int)(w.ws_row);
#endif // Windows/Linux
}

// enum for ncurses color pairs
enum ColorPair { BOARD_PAIR = 2, PATH_PAIR, ACTIVE_OPTION_PAIR, STANDARD_PAIR, NORMAL_TEXT_PAIR, PLAYER_PAIR, HUMAN_PAIR, AI_PAIR };

// inits colors used in application
void init_colors() {
	init_pair(BOARD_PAIR, COLOR_CYAN, COLOR_BLACK);
	init_pair(PATH_PAIR, COLOR_BLACK, COLOR_GRAYTEXT);
	init_pair(ACTIVE_OPTION_PAIR, COLOR_WHITE, COLOR_RED);
	init_pair(STANDARD_PAIR, COLOR_WHITE, COLOR_BLACK);
	init_pair(NORMAL_TEXT_PAIR, COLOR_CYAN, COLOR_BLACK);
}

// string containing the instructions of the game
std::string helpString = R"(
	
	INSTRUCTIONS

	solve the maze by going to the lower right corner of the map

	CONTROLS

	move by using arrow keys
	press 'q' to quit
	press 'm' to go to menu

	MODE

	solo   - solve maze by yourself
	ai     - watch how ai solves maze
	battle - play against an ai


	




	press 'm' to go to main menu
)";