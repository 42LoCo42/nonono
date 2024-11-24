#ifndef _UTILS_H
#define _UTILS_H

// create typ sym = gen, run body if sym is not NULL, and lastly post
#define with(typ, sym, gen, body, post) {\
	typ sym = gen;\
	if(sym != NULL) {body;}\
	post;\
}

// like with(), but sym is free()'ed after block ends
#define withF(typ, sym, gen, body) with(typ, sym, gen, body, free(sym));

#endif // _UTILS_H
