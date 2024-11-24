#ifndef DEFER_H
#define DEFER_H
#define __PCC__

#ifndef DEFER_MAXIMUM_COUNT
#define DEFER_MAXIMUM_COUNT 32
#endif

#define _concat(a, b) a ## b

#define _deferGo if(_defer_count > 0) goto *_defer_locations[--_defer_count];

#define deferral(...)                                   \
	size_t _defer_count = 0;                            \
	void*  _defer_locations[DEFER_MAXIMUM_COUNT] = {0}; \
	do {                                                \
		{__VA_ARGS__;}                                  \
		_deferGo;                                       \
	} while(0);                                         \

#define _deferOnLine(line, ...)                                            \
	_defer_locations[_defer_count++] = && _concat(_defer_location_, line); \
	if(0) {                                                                \
	_concat(_defer_location_, line):                                       \
		__VA_ARGS__;                                                       \
		_deferGo;                                                          \
		else break;                                                        \
	}                                                                      \

#define defer(block) _deferOnLine(__LINE__, block)

#endif // DEFER_H
