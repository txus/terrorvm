#ifndef _tvm_gc_h_
#define _tvm_gc_h_

#include <stdlib.h>
#include <stdint.h>
#include <terror/darray.h>

struct state_s;
struct val_s;
struct cell_s;

typedef struct cell_s {
  struct cell_s *next;
  struct cell_s *prev;
  void *value;
  char ecru;
} Cell;

typedef struct {
  Cell *cell;
} GCHeader;

typedef struct {
  Cell *bottom;
  Cell *top;
  Cell *free;
  Cell *scan;
  int growth_rate;
  int allocs;
  int scan_every;
  DArray *chunks;
} GCHeap;

typedef struct {
  Cell *head;
  Cell *tail;
} MemoryChunk;

MemoryChunk MemoryChunk_allocate(int size);

struct val_s* gc_alloc(struct state_s*, GCHeap *heap);
void gc_prepare_to_scan(GCHeap *heap, DArray *rootset);
void gc_flip(struct state_s*, GCHeap *heap);
void gc_scan(GCHeap *heap);
void gc_scan_cell(GCHeap *heap, Cell*);
GCHeap* GCHeap_new(int, int);
void GCHeap_destroy(GCHeap*);
void GCHeap_grow(GCHeap*, int);

void gc_unsnap(GCHeap*, Cell*);
void gc_insert_in(GCHeap*, Cell*, Cell*);
void gc_release(Cell*);

#define BOTTOM heap->bottom
#define TOP    heap->top
#define SCAN   heap->scan
#define FREE   heap->free

#define GC_PUSH_BOTTOM() (BOTTOM = BOTTOM->next)
#define GC_PUSH_TOP() (TOP = TOP->next)
#define GC_PUSH_SCAN() (SCAN = SCAN->next)
#define GC_PUSH_FREE() (FREE = FREE->next)

#define GC_PULL_BOTTOM() (BOTTOM = BOTTOM->prev)
#define GC_PULL_TOP() (TOP = TOP->prev)
#define GC_PULL_SCAN() (SCAN = SCAN->prev)
#define GC_PULL_FREE() (FREE = FREE->prev)

int GCHeap_size(GCHeap*);
int GCHeap_white_size(GCHeap*);
int GCHeap_ecru_size(GCHeap*);
int GCHeap_grey_size(GCHeap*);
int GCHeap_black_size(GCHeap*);
void GCHeap_print(GCHeap*);
void GCHeap_print_detailed(GCHeap*);

#endif

