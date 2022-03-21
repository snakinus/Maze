#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <codecvt>
#include <io.h>
#include <fcntl.h>
#include <curses.h>

using namespace std;

class Menu {
	int activeOption;			// current option chosen by user
	int numberOfOptions;		// number of possible option in menu
	int heightOfOptions = 3;	// height in console lines of each menu option
	int heightOfHeader = 7;		// height in console lines of header

	// wstring is used because of necessity of displaying Unicode
	wstring menuStr;			// wstring for whole menu text
	wstring header;				// wstring for header
	vector<wstring> options;	// vector of wstring for options

	// indentation
	int tc; // number of spaces required to center the object
	int tr; // number of new lines required to center the object

	// returns wstring which contains content of file text 
	wstring readFile(const char* filename) {
		wifstream wif(filename);
		wif.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
		wstringstream wss;
		wss << wif.rdbuf();
		return wss.str();
	}

public:
	// menu constructor
	Menu(const char * filename, int numberOfOptions) : numberOfOptions(numberOfOptions) {
		// saves file content in menuStr
		menuStr = readFile(filename);

		// checks for newlines in menuStr and stores their indexes
		vector<int> lines;
		lines.push_back(0);
		for (int i = 0; i < menuStr.length(); i++) {
			if (menuStr[i] == '\n') lines.push_back(i + 1);
		}
		lines.push_back(menuStr.length() - 1);

		// extracts header and options from menuStr - assumes that the input data is correct
		header = menuStr.substr(0, lines[heightOfHeader]);
		for (int i = heightOfHeader; i < lines.size(); i += 3) {
			if (i + 3 >= lines.size()) break;
			options.push_back(menuStr.substr(lines[i], lines[i + 3] - lines[i]));
		}

		// reads terminal size and sets the variables for indetation  
		get_terminal_size(tc, tr);
		tc = tc / 2 - 20;
		tr = tr / 2 - 10;
	}

	// returns active option
	int getActiveOption() {
		return activeOption;
	}

	// prints wstring using addch - necessary because of unicodes and the use of ncurses
	void printWstring(wstring& ws) {
		for (int i = 0; i < ws.length(); i++) {
			addch(ws[i]);
			if (ws[i] == '\n') {
				for (int i = 0; i < tc; i++)
					addch(' ');
			}
		}
	}

	// almost exactly the same as printWstring, but colors option to 'active' color
	void printActiveOption(wstring& ws) {
		for (int i = 0; i < ws.length(); i++) {
			addch(ws[i]);
			if (ws[i] == '\n') {
				attron(COLOR_PAIR(STANDARD_PAIR));
				for (int i = 0; i < tc; i++)
					addch(' ');
				attron(COLOR_PAIR(ACTIVE_OPTION_PAIR));
			}
		}
	}

	// prints menu
	void print() {
		// prints indentation for centering menu
		for (int i = 0; i < tr; i++) addch('\n');
		for (int i = 0; i < tc; i++) addch(' ');

		// prints header
		printWstring(header);

		// prints options - uses idx to check if current option is active
		int idx = 0;
		for (auto option : options) {
			// if current option is active, colors it appropriately and after it return to normal coloring
			if (idx == activeOption) {
				attron(COLOR_PAIR(ACTIVE_OPTION_PAIR));
				printActiveOption(option);
				attron(COLOR_PAIR(STANDARD_PAIR));
				idx++;
				continue;
			}

			printWstring(option);
			idx++;
		}

		// displays menu on the screen
		refresh();
	}

	// resets option to the first one
	void resetOption() {
		activeOption = 0;
	}

	// changes active option depending on the user input 
	// and returns selected option by user - if nothing is selected, returns -1
	int changeOption() {

#define KEY_ENTER 13
		int key = getch();
		switch (key) {
		case 'q': // quits
			endwin();
			exit(0);
			break;
		case KEY_UP:	// changes to previous option (goes up)
			activeOption--;
			if (activeOption < 0) activeOption = options.size() - 1;
			break;
		case KEY_DOWN:	// changes to next option (goes down)
			activeOption = (activeOption + 1) % options.size();
			break;
		case KEY_ENTER: // selects option
			return activeOption;
		}
#undef KEY_ENTER

		// returns -1 if option was not selected
		return -1;
	}
};