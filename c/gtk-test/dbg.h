#ifndef DBG_H
#define DBG_H

#define warnx(fmt, ...) {       \
	fprintf(                    \
		stderr,                 \
		"%s:%d: warning: " fmt, \
		__FILE__,               \
		__LINE__,               \
		##__VA_ARGS__           \
	);                          \
	fprintf(stderr, "\n");      \
}                               \

#define warn(fmt, ...) \
	warnx(fmt ": %s", ##__VA_ARGS__, strerror(errno));

#define errx(fmt, ...) {      \
	fprintf(                  \
		stderr,               \
		"%s:%d: error: " fmt, \
		__FILE__,             \
		__LINE__,             \
		##__VA_ARGS__         \
	);                        \
	fprintf(stderr, "\n");    \
	exit(1);                  \
}                             \

#define err(fmt, ...) \
	errx(fmt ": %s", ##__VA_ARGS__, strerror(errno));

#endif // DBG_H
