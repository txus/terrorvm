#ifndef _fx_gc_h_
#define _fx_gc_h_

#include <stdlib.h>
#include <forkix/value.h>

VALUE gc_alloc(size_t size);
void gc_dealloc(VALUE obj);

#endif

