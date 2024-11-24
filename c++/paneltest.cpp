#include <panel.h>
#include <vector>
#include <string>
using namespace std;

constexpr static size_t WIN_HEIGHT = 10;
constexpr static size_t WIN_WIDTH = 40;
constexpr static size_t WIN_COUNT = 10;

void init_wins(vector<WINDOW*>& wins, size_t win_count);
void win_show(WINDOW* win, const string& label, int label_color_id);
void print_label(WINDOW* win, const string& label, chtype color);

auto main() -> int {
	vector<WINDOW*> my_wins;
	vector<PANEL*> my_panels;
	PANEL* top;
	int input;

	// Init ncurses
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, true);

	// Init colors, one more than WIN_COUNT for message
	for(size_t i=1; i<=WIN_COUNT+1; ++i) {
		init_pair(i, i, COLOR_BLACK);
	}

	init_wins(my_wins, WIN_COUNT);

	// Attach a panel to each window, order is bottom-up
	for(auto& win : my_wins) {
		my_panels.push_back(new_panel(win));
	}

	// Set up the user pointers to the next panel
	for(size_t i=0; i<my_panels.size(); ++i) {
		set_panel_userptr(my_panels[i], my_panels[(i+1) % my_panels.size()]);
	}

	// Update the stacking order
	update_panels();

	// Show panels on the screen
	attron(COLOR_PAIR(WIN_COUNT + 1));
	mvprintw(LINES - 2, 0, "Use tab to browse through the windows (F1 to exit)");
	attroff(COLOR_PAIR(WIN_COUNT + 1));
	doupdate();

	top = my_panels[my_panels.size()-1];
	while((input = getch()) != KEY_F(1)) {
		switch(input) {
			case 9:
				top = reinterpret_cast<PANEL*>(const_cast<void*>(panel_userptr(top)));
				top_panel(top);
				break;
		}
		update_panels();
		doupdate();
	}

	endwin();
	return 0;
}

void init_wins(vector<WINDOW*>& wins, size_t win_count) {
	int x = 1;
	int y = 1;

	for(size_t i=0; i<win_count; ++i) {
		wins.push_back(newwin(WIN_HEIGHT, WIN_WIDTH, y, x));
		win_show(wins[i], string("Window Number ") + to_string(i+1), i+1);
		x += 7;
		y += 3;
	}
}

void win_show(WINDOW* win, const string& label, int label_color_id) {
	int width = getmaxx(win);

	box(win, 0, 0);
	mvwaddch(win, 2, 0, ACS_LTEE);
	mvwaddch(win, 2, width - 1, ACS_RTEE);
	mvwhline(win, 2, 1, ACS_HLINE, width - 2);

	print_label(win, label, COLOR_PAIR(label_color_id));

}

void print_label(WINDOW* win, const string& label, chtype color) {
	if(win == nullptr) {
		win = stdscr;
	}

	int width = getmaxx(win);
	wattron(win, color);
	mvwprintw(win, 1, (width - label.length())/2, label.c_str());
	wattroff(win, color);
	refresh();
}
