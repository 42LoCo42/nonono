#ifndef UTILS_H
#define UTILS_H

#define check(cond, msg) _check(__FILE__, __LINE__, cond, msg);

void _check(char* file, int line, int cond, char* msg);

#endif // UTILS_H
