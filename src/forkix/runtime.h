#ifndef _fx_runtime_h_
#define _fx_runtime_h

#include <forkix/value.h>

extern VALUE TrueObject;
extern VALUE FalseObject;
extern VALUE NilObject;

void Runtime_init();
void Runtime_destroy();

#endif
