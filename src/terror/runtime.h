#ifndef _tvm_runtime_h_
#define _tvm_runtime_h

#include <terror/value.h>

struct state_s;

extern VALUE Object_bp;
extern VALUE Number_bp;
extern VALUE String_bp;
extern VALUE Vector_bp;
extern VALUE Map_bp;
extern VALUE Closure_bp;

extern VALUE TrueObject;
extern VALUE FalseObject;
extern VALUE NilObject;

void Runtime_init(struct state_s*);

#endif
