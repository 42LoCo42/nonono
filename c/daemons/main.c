#include <ev.h>
#include <stdio.h>
#include <unistd.h>

ev_io    stdin_watcher;
ev_timer timer_watcher;


static void stdin_cb(EV_P_ ev_io* w, int revents) {
	puts("stdin ready");
	ev_io_stop(EV_A_ w);
	ev_break(EV_A_ EVBREAK_ALL);
}

static void timer_cb(EV_P_ ev_timer* w, int revents) {
	puts("timeout");
	ev_break(EV_A_ EVBREAK_ONE);
}

int main(int argc, char** argv) {
	struct ev_loop* loop = EV_DEFAULT;

	ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
	ev_io_start(loop, &stdin_watcher);

	ev_timer_init(&timer_watcher, timer_cb, 5.5, 0);
	ev_timer_start(loop, &timer_watcher);

	ev_run(loop, 0);
	return 0;
}
