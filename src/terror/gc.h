#ifndef _tvm_gc_h_
#define _tvm_gc_h_

#include <stdlib.h>
#include <terror/value.h>
#include <terror/list.h>
#include <terror/gc_header.h>

struct state_s;

typedef struct {
  GCHeader *bottom;
  GCHeader *top;
  GCHeader *free;
  GCHeader *scan;
} Heap;

VALUE gc_alloc(struct state_s*);
Heap* Heap_new();

#endif

