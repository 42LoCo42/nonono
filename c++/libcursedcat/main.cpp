#include "include/CursedCat.hpp"
#include "include/CursedCat_decls.hpp"

#include <ncurses.h>

int main() {
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	raw();
	keypad(stdscr, true);
	curs_set(0);

	CursedCat::LineDataSet lds {};
	CursedCat::readFile("foo", &lds);
	CursedCat::readFile("main.cpp", &lds);
	CursedCat::write();

	refresh();
	getchar();
	endwin();
}
