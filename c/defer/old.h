#ifndef DEFER_H
#define DEFER_H
#define __PCC__

#ifndef DEFER_MAXIMUM_COUNT
#define DEFER_MAXIMUM_COUNT 32
#endif

#define _defer_concat(a, b) a ## b

#define defer(block) _defer(block, __LINE__)
#define _defer(block, line)                         \
	_defer_locations[_defer_count++] =              \
		&& _defer_concat(_defer_label_, line);      \
	if(0) {                                         \
	_defer_concat(_defer_label_, line):             \
		block;                                      \
		if(_defer_count > 0) {                      \
			goto *_defer_locations[--_defer_count]; \
		} else break;                               \
	}                                               \

#define deferral(block) {                                            \
		void* _defer_locations[DEFER_MAXIMUM_COUNT] = {};            \
		unsigned char _defer_count = 0;                              \
		do {block} while(0);                                         \
		if(_defer_count > 0) goto *_defer_locations[--_defer_count]; \
	}                                                                \

#define deferfn(type, rest, body) \
	type rest {                   \
		type retval = 0;          \
		deferral(body);           \
		return retval;            \
	}                             \

#define Return() break;

#define ReturnVal(val) \
	retval = val;      \
	break;             \

#endif // DEFER_H
