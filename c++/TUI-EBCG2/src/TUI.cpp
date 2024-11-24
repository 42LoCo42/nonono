#include <ncurses.h>
#include <string>
#include <vector>
#include <CursedCat_decls.hpp> // actual declarations
#include <stdio.h>

#include "TUI.hpp"

using namespace std;
using namespace CursedCat;




WINDOW* boardWindow;

int main(int argv, char* argv[]) {
	if(argc < 2) {
		printf("Usage: %s <hostname> [port]", argv[0]);
	}


}

string getHeader() {
	int height, width;
	getmaxyx(stdscr, height, width);
	if(width < 83) return "assets/header_small.txt";
	else return "assets/header_big.txt";
}

void startTUI() {
	initscr();
	raw(); // for function keys
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	if(has_colors) {
		start_color();
		init_pair(1, COLOR_GREEN, COLOR_BLACK); // default cell
		init_pair(2, COLOR_RED, COLOR_BLACK); // active cell
		init_pair(3, COLOR_BLACK, COLOR_YELLOW); // blocked cell
		init_pair(4, COLOR_RED, COLOR_YELLOW); // active & blocked
	}

	box(stdscr, 0, 0);
}

void printGame() {
	
}

void printGameState(GameState& gs) {
	int height, width;
	getmaxyx(stdscr, height, width);
	erase();

	if(gs == GameState::mainMenu) {
		LineDataSet menu;

		CursedCat::readFile(getHeader().c_str());
		CursedCat::readFile("assets/mainMenu.txt", &menu);
		CursedCat::write();

		int selected = handleMenu(menu);

		switch(selected) {
			case 0: 
				gs = GameState::playing;
				boardWindow = newwin(save.board[0].size()+3, save.board.size()*2+1, 2, 0);
				break;
			case 1: gs = GameState::openMenu; break;
			case 2: gs = GameState::settingsMenu; break;
			case 3: gs = GameState::quit; break;
		}
	}
	else if(gs == GameState::openMenu) {
		LineDataSet saves;
		string savegamename = "  savegame";

		erase();
		CursedCat::readFile(getHeader().c_str());
		CursedCat::readFile("assets/openMenu.txt");

		CursedCat::readFile("assets/back.txt", &saves);
		for(int i=0; i<5; ++i) { // simulate content
			CursedCat::read(savegamename + to_string(i) + "  ", &saves);
		}

		CursedCat::write();

		int selected = handleMenu(saves);
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::settingsMenu) {
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::playing) {
		// draw everything
		box(stdscr, 0, 0);
		wborder(boardWindow, '|', '|', '-', '-', '+', '+', '+', '+');
		printBoard();
		mvprintw(1, 1, string("Score: ") + to_string(save.score).c_str()); // TODO left off here. concat string correctly!
		mvwprintw(boardWindow, save.board[0].size()+1, save.currentCol+1, to_string(save.currentNum).c_str());

		refresh();
		wrefresh(boardWindow);
		getch();

		wborder(boardWindow, ' ', ' ', ' ',' ',' ',' ',' ',' ');
		wrefresh(boardWindow);
		delwin(boardWindow);
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::pauseMenu) {
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::saveMenu) {
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::shop) {
		gs = GameState::mainMenu;
	}
	else if(gs == GameState::timeline) {
		gs = GameState::mainMenu;
	}
}

int handleMenu(const LineDataSet& lds) {
	int selected = 0;
	int c;
	
	while(true) {
		for(int i=0; i<lds.lineData.size(); ++i) {
			mvprintw(lds.lineData[i].y, lds.lineData[i].x, " ");
			mvprintw(lds.lineData[i].y, lds.lineData[i].x + lds.lineData[i].length - 1, " ");
		}

		attron(COLOR_PAIR(1));
		mvprintw(lds.lineData[selected].y, lds.lineData[selected].x, "[");
		mvprintw(lds.lineData[selected].y, lds.lineData[selected].x + lds.lineData[selected].length - 1, "]");
		attroff(COLOR_PAIR(1));

		c = getch();
		if(c == KEY_DOWN && selected < lds.lineData.size() - 1) {
			++selected;
		}
		else if(c == KEY_UP && selected > 0) {
			--selected;
		}
		else if(c == 10) { // 10 is the ENTER key
			return selected;
		}
	}
}

void printBoard() {
	for(int y=0; y<save.board[0].size(); ++y) {
		for(int x=0; x<save.board.size(); ++x) {
			int num = save.board[x][y].num;
			mvwprintw(boardWindow, y+1, x*2+1, num ? to_string(num).c_str() : "");
		}
	}
}
