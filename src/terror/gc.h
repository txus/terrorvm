#ifndef _tvm_gc_h_
#define _tvm_gc_h_

#include <terror/value.h>
#include <treadmill/gc.h>
#include <treadmill/darray.h>

void GC_release(void *value);
void GC_scan_pointers(TmHeap *heap, TmObjectHeader *object, TmCallbackFn callback);
Tm_DArray* GC_rootset(TmStateHeader *state_header);

#endif
