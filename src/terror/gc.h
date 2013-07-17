#ifndef _tvm_gc_h_
#define _tvm_gc_h_

#include <terror/value.h>
#include <sweeper/header.h>
#include <sweeper/sweeper.h>

#define GC_allocate swp_allocate
#define GC_disable SWPHeap_disable
#define GC_enable SWPHeap_enable
#define GC_protect(A) SWPHeader_mark((SWPHeader*) (A))
#define GC_unprotect(A) SWPHeader_unmark((SWPHeader*) (A))
void GC_release(SWPHeader *value);
void GC_add_children(SWPHeader *object, SWPArray *children);
void GC_add_roots(void *state, SWPArray *roots);

#endif
