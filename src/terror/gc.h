#ifndef _tvm_gc_h_
#define _tvm_gc_h_

#include <stdlib.h>
#include <terror/value.h>

VALUE gc_alloc(size_t size);
void gc_dealloc(VALUE obj);

#endif

