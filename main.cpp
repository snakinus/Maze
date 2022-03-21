#include "Game.h"
#include "Menu.h"
#include <iostream>
#include <string>

using namespace std;

// menu's used in the game
Menu modeMenu = Menu("menu_mode.txt", 4);
Menu mainMenu = Menu("menu_main.txt", 3);

// handles help tab
void printHelp() {
	// change color for text
	attron(COLOR_PAIR(NORMAL_TEXT_PAIR));

	// display help until user decides to go back to menu
	while (true) {
		clear();
		for (int i = 0; i < helpString.length(); i++) {
			addch(helpString[i]);
		}
		refresh();

		int key = getch();
		switch (key) {
		case 'm': // Go to menu.
			return;
		default:
			break;
		}
	}

	// change back to the standard color 
	attron(COLOR_PAIR(STANDARD_PAIR));
}

// handles mode menu
void handleMenuMode() {
	modeMenu.print();

	while (true) {
		int option = modeMenu.changeOption();
		clear();
		modeMenu.print();

		// if user didn't select option then continue
		if (option == -1) continue;

		if (option == 3) {
			break;
		}
		else {
			Game game(option);
			game.play();
		}
	}
	modeMenu.resetOption();
}

// handles main menu
void handleMenuMain() {
	mainMenu.print();

	while (true) {
		int option = mainMenu.changeOption();
		clear();
		mainMenu.print();

		if (option == 0) {
			handleMenuMode();
		}
		else if (option == 1) {
			printHelp();
		}
		else if (option == 2) {
			endwin();
			break;
		}
	}
}

int main() {
	 // set necessary settings for pdcurses  
	setlocale(LC_ALL, "");
	initscr();
	start_color();
	raw();
	nonl();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
	nodelay(stdscr, TRUE);
	timeout(0);

	// init colors
	init_colors();

	// start main menu
	handleMenuMain();

	// restore the saved shell terminal mode
	endwin();
}