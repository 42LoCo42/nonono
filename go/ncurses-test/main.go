package main

import (
	// #cgo CFLAGS:  -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600
	// #cgo LDFLAGS: -lncursesw
	// #include <locale.h>
	// #include <ncurses.h>
	"C"

	"fmt"
	"strings"
)

func wrect(win *C.WINDOW, y int, x int, lines int, cols int) {
	C.wcolor_set(win, 1, nil)

	// first line
	C.mvwaddch(win, C.int(y), C.int(x), C.ACS_ULCORNER)
	C.waddstr(win, C.CString(strings.Repeat("─", cols - 2)))
	C.waddch(win, C.ACS_URCORNER)

	// middle lines
	for i := y + 1; i <= y + lines - 2; i++ {
		C.mvwaddch(win, C.int(i), C.int(x), C.ACS_VLINE)
		C.mvwaddch(win, C.int(i), C.int(x + cols - 1), C.ACS_VLINE)
	}

	// last line
	C.mvwaddch(win, C.int(y + lines - 1), C.int(x), C.ACS_LLCORNER)
	C.waddstr(win, C.CString(strings.Repeat("─", cols - 2)))
	C.waddch(win, C.ACS_LRCORNER)

	C.wcolor_set(win, 0, nil)
}

// idea for ControlC: default params at any position
// match via remaining types?
// also we should totally implement &rest and such things
func rect(y int, x int, lines int, cols int) {
	wrect(C.stdscr, y, x, lines, cols)
}

func cursesInit() {
	C.setlocale(C.LC_ALL, C.CString(""))
	C.initscr()
	C.start_color()
	C.use_default_colors()
	C.init_pair(1, C.COLOR_RED, -1)
	C.cbreak()
	C.noecho()
	C.intrflush(C.stdscr, false)
	C.keypad(C.stdscr, true)
}


func main() {
	cursesInit()
	defer C.endwin()

	for {
		var ch C.wint_t
		C.get_wch(&ch)
		if ch == 4 {
			break // 4 == EOF == C-d
		}

		C.erase()
		C.box(C.stdscr, 0, 0)
		rect(10, int(ch), 5, 5)
		C.mvaddstr(12, C.int(ch + 1), C.CString(fmt.Sprintf("%d", ch)))
		C.mvaddstr(1, 1, C.CString(fmt.Sprintf("value for %c is %d", rune(ch), ch)))
		C.refresh()
	}
}
