#ifndef _tvm_gc_h_
#define _tvm_gc_h_

#include <terror/value.h>
#include <sweeper/header.h>
#include <sweeper/sweeper.h>

#define GC_allocate swp_allocate
#define GC_disable SWPHeap_disable
#define GC_enable SWPHeap_enable
void GC_release(SWPHeader *value);
void GC_add_children(SWPHeader *object, SWPArray *children);
void GC_add_roots(void *state, SWPArray *roots);

#endif
