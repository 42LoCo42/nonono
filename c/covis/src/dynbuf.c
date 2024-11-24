#include "dynbuf.h"
#include <string.h>

#define MIN(x, y) (x < y ? x : y)

void _dynbuf_free(dynbuf* d) {
	free(d->data);
}

int _dynbuf_resize(dynbuf* d, size_t count) {
	void* new_data = reallocarray(d->data, count, d->size);
	if(new_data == NULL) {
		return -1;
	} else {
		d->data = new_data;
		d->count = count;
		return 0;
	}
}

int _dynbuf_insert(dynbuf* d, size_t pos, const void* data, size_t count) {
	size_t move_count = d->count - pos;
	size_t old_count = d->count;
	if(_dynbuf_resize(d, count + d->count) == -1) {
		return -1;
	}

	if(pos < old_count) {
		memmove(d->data + d->size * (pos + count), d->data + d->size * pos, d->size * move_count);
	}

	_dynbuf_write(d, pos, data, count);
	return 0;
}

void _dynbuf_write(dynbuf* d, size_t pos, const void* data, size_t count) {
	memcpy(d->data + d->size * pos, data, d->size * count);
}

int _dynbuf_delete(dynbuf* d, size_t pos, size_t count) {
	memmove(d->data + d->size * pos, d->data + d->size * (pos + count), d->size * (d->count - pos - count));
	d->count -= count;
	void* new_data = reallocarray(d->data, d->count, d->size);
	if(new_data == NULL) {
		return -1;
	} else {
		d->data = new_data;
		return 0;
	}
}

void* _dynbuf_at(dynbuf* d, size_t pos) {
	return d->data + d->size * pos;
}

size_t _dynbuf_count(dynbuf* d) {
	return d->count;
}
