#include "termcontrol.h"

#include <termios.h>
#include <unistd.h>

struct termios settings;
struct termios original_settings;

int tctl_load() {
	// get & save current terminal settings
	if(tcgetattr(STDIN_FILENO, &settings) == -1) {
		return -1;
	}
	original_settings = settings;

	return 0;
}

int tctl_apply() {
	// apply the new settings
	return tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

void tctl_reset() {
	settings = original_settings;
}

void tctl_enable_raw_term() {
	// disable buffered IO, echoing, control signals, ctrl-v literal bytes
	settings.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
	// disable output transmission control, carriage return translation
	settings.c_iflag &= ~(IXON | ICRNL);
}

void tctl_set_vtime(int vtime) {
	settings.c_cc[VMIN] = 0;
	settings.c_cc[VTIME] = vtime;
}
