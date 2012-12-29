#ifndef _fx_runtime_h_
#define _fx_runtime_h

#include <forkix/value.h>

extern VALUE Object_bp;
extern VALUE Integer_bp;
extern VALUE String_bp;
extern VALUE Vector_bp;
extern VALUE Map_bp;
extern VALUE Closure_bp;

extern VALUE TrueObject;
extern VALUE FalseObject;
extern VALUE NilObject;

void Runtime_init();
void Runtime_destroy();

#endif
