#include <X11/Xlib.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>

#define fail(...) errx(1, __VA_ARGS__)

int main(int argc, char** argv) {
	Display* display = XOpenDisplay(NULL);
	if(display == NULL) fail("Could not open display");

	XIM xim = XOpenIM(display, NULL, NULL, NULL);
	if(xim == NULL) fail("Could not open XIM");

	XIC xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, NULL);
	if(xic == NULL) fail("Could not create input context");

	XKeyPressedEvent event = {};
	event.type = KeyPress;
	event.display = display;

	char buffer[32];
	KeySym ignore;
	Status return_status;

	for(int i = 1; i < argc; ++i) {
		event.state = 0;

		char* arg = argv[i];
		for(;;) {
			switch(*arg) {
				case 0:  fail("Invalid %d. argument: %s", i, argv[i]); break;
				case 'S': event.state |= ShiftMask; break;
				case 'M': event.state |= Mod5Mask; break;
				default: goto output;
			}
			arg++;
		}
	
output:
		event.keycode = atoi(arg);
		Xutf8LookupString(xic, &event, buffer, 32, &ignore, &return_status);
		printf("%s\n", buffer);
	}

	XCloseDisplay(display);
	return 0;
}
