#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <ixp.h>

void _attach(Ixp9Req*);
void  attach(Ixp9Req*);

void _clunk (Ixp9Req*);
void  clunk (Ixp9Req*);

void _create(Ixp9Req*);
void  create(Ixp9Req*);

void _flush (Ixp9Req*);
void  flush (Ixp9Req*);

void _open  (Ixp9Req*);
void  open  (Ixp9Req*);

void _read  (Ixp9Req*);
void  read  (Ixp9Req*);

void _remove(Ixp9Req*);
void  remove(Ixp9Req*);

void _stat  (Ixp9Req*);
void  stat  (Ixp9Req*);

void _walk  (Ixp9Req*);
void  walk  (Ixp9Req*);

void _write (Ixp9Req*);
void  write (Ixp9Req*);

void _wstat (Ixp9Req*);
void  wstat (Ixp9Req*);

void _freefid(IxpFid*);
void  freefid(IxpFid*);

#endif // WRAPPERS_H
