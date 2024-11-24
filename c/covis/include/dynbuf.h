#ifndef _DYNBUF_H
#define _DYNBUF_H

#include <stdlib.h>

struct dynbuf {
	size_t count;
	size_t size;
	void* data;
};

typedef struct dynbuf dynbuf;

void   _dynbuf_free(dynbuf*);
int    _dynbuf_resize(dynbuf*, size_t count);

int    _dynbuf_insert(dynbuf*, size_t pos, const void* data, size_t count);
void   _dynbuf_write(dynbuf*, size_t pos, const void* data, size_t count);

int    _dynbuf_delete(dynbuf*, size_t pos, size_t count);

void*  _dynbuf_at(dynbuf*, size_t pos);

size_t _dynbuf_count(dynbuf*);

#define dynbuf_make(type, name) type* name;\
								{\
									static dynbuf _ ## name = {0, sizeof(type), NULL};\
									name = (type*) &_ ## name;\
								}

#define dynbuf_free(d) (_dynbuf_free((dynbuf*) d))
#define dynbuf_resize(d, count) (_dynbuf_resize((dynbuf*) d, count));

#define dynbuf_insert(d, pos, data, count) (_dynbuf_insert((dynbuf*) d, pos, data, count))
#define dynbuf_write(d, pos, data, count) (_dynbuf_write((dynbuf*) d, pos, data, count))
#define dynbuf_append(d, data, kount) (_dynbuf_insert((dynbuf*) d, ((dynbuf*) d)->count, data, kount))

#define dynbuf_delete(d, pos, count) (_dynbuf_delete((dynbuf*) d, pos, count))

#define dynbuf_at(d, pos) ((typeof(d)) _dynbuf_at((dynbuf*) d, pos))

#define dynbuf_count(d) (_dynbuf_count((dynbuf*) d))

#endif // _DYNBUF_H
