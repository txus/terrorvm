#include <terror/gc.h>
#include <terror/value.h>
#include <terror/runtime.h>
#include <terror/state.h>
#include <stdlib.h>
#include <assert.h>

VALUE Object_bp;

/*
 * -(bottom)- ECRU -(top)- GREY -(scan)- BLACK -(free)- WHITE ...
 */

#define ITERATE(A, B, N) \
  (N) = (A);             \
  while((N) != (B))

static inline void
gc_make_ecru(GCHeap *heap, Cell *me)
{
  if(me == BOTTOM) {
    if (me == TOP)  TOP  = me->next;
    if (me == SCAN) SCAN = me->next;
    if (me == FREE) FREE = me->next;
  } else {
    gc_insert_in(heap, me, BOTTOM);
  }
  me->ecru = 1;
}

static inline void
gc_make_grey(GCHeap *heap, Cell *me)
{
  gc_insert_in(heap, me, TOP);
  me->ecru = 0;

  if (me == SCAN) {
    SCAN = me->next;
  }
  if (me == FREE) {
    FREE = me->next;
  }
}


void
gc_flip(STATE, GCHeap *heap) {
  debug("[GC] Collection");
  Cell *ptr = NULL;

  // Make all the ecru into white (release them)
  ITERATE(BOTTOM, TOP, ptr) {
    ptr->ecru = 0;
    gc_release(ptr);
    ptr = ptr->next;
  }
  BOTTOM = TOP;

  // Grow the heap a bit to delay flips
  GCHeap_grow(heap, heap->growth_rate);

  // Make black into ecru
  ITERATE(SCAN, FREE, ptr) {
    Cell *next = ptr->next;
    gc_make_ecru(heap, ptr);
    ptr = next;
  }

  // Swap bottom and top pointers
  /* Cell *bottom_ptr = heap->bottom; */
  /* Cell *top_ptr    = heap->top; */
  /* heap->bottom     = top_ptr; */
  /* heap->top        = bottom_ptr; */

  if(state) {
    DArray *rootset = State_rootset(state);

    // Make root set grey to be analyzed in the next collection
    debug("[GC] Adding rootset (%i) to the scan area...", DArray_count(rootset));
    gc_prepare_to_scan(heap, rootset);
  }
  /* GCHeap_print(heap); */
}

void
gc_prepare_to_scan(GCHeap *heap, DArray *rootset)
{
  for(int i=0; i < DArray_count(rootset); i++) {
    GCHeader *v = (GCHeader*)(DArray_at(rootset, i));
    Cell *cell = v->cell;
    gc_make_grey(heap, cell);
  }
}

VALUE
gc_alloc(STATE, GCHeap *heap)
{
  if(heap->allocs >= heap->scan_every) {
    debug("SCANNING!");
    heap->allocs = 0;
    gc_scan(heap);
  }

  /*
   * If there are no slots in the white list,
   * force a collection.
   */
  if(FREE->next == BOTTOM) {
    gc_flip(state, heap);
  }

  /*
   * Advance the "free" pointer,
   * swallowing a free white cell into black.
   */
  Cell *free = FREE;
  GC_PUSH_FREE();

  VALUE val = calloc(1, sizeof(val_t));
  check(val, "Out of memory.");
  val->gc.cell = free;
  free->value = val;

  heap->allocs++;

  return val;
error:
  return NULL;
}

void
gc_scan(GCHeap *heap)
{
  // If scan == top, the collection has finished
  if(SCAN == TOP) {
    debug("[GC]: No more grey objects to scan.");
    return;
  }

  Cell *toscan = SCAN->prev;

  // Move the scan pointer backwards, converting the scanned grey cell into a
  // black cell.
  GC_PULL_SCAN();
  gc_scan_cell(heap, toscan);
}

void
gc_scan_cell(GCHeap *heap, Cell *c)
{
  VALUE val = (VALUE)c->value;

  for(int i=0; i<DArray_count(val->fields); i++) {
    VALUE v = (VALUE)DArray_at(val->fields, i);
    Cell *cell = v->gc.cell;
    if(cell->ecru) {
      // Unsnap the cell from the ecru area,
      // and put it in the gray area.
      gc_make_grey(heap, cell);
    }
  }

  Value_each(val, ^ void (VALUE k, VALUE v) {
    Cell *cell = k->gc.cell;
    if(cell->ecru) {
      // Unsnap the cell from the ecru area,
      // and put it in the gray area.
      gc_make_grey(heap, cell);
    }

    cell = v->gc.cell;
    if(cell->ecru) {
      // Unsnap the cell from the ecru area,
      // and put it in the gray area.
      gc_make_grey(heap, cell);
    }
  });
}

GCHeap*
GCHeap_new(int size, int growth_rate)
{
  GCHeap *heap = calloc(1, sizeof(GCHeap));

  heap->chunks = DArray_create(sizeof(Cell*), 10);

  MemoryChunk chunk = MemoryChunk_allocate(size);
  Cell *head = chunk.head;
  Cell *tail = chunk.tail;

  // Save a reference to the chunk to deallocate it later
  DArray_push(heap->chunks, head);

  heap->growth_rate = growth_rate;
  heap->allocs      = 0;
  heap->scan_every  = 5;

  FREE   = head;
  BOTTOM = head;
  TOP    = head;
  SCAN   = head;

  // Close the circle.
  tail->next = head;
  head->prev = tail;

  return heap;
}

void
GCHeap_destroy(GCHeap *heap)
{
  Cell *ptr = NULL;

  ITERATE(BOTTOM, FREE, ptr) {
    /* gc_release(ptr); */
    ptr = ptr->next;
  }

  for(int i=0; i < DArray_count(heap->chunks); i++) {
    Cell *chunk = (Cell*)DArray_at(heap->chunks, i);
    free(chunk);
  }

  DArray_destroy(heap->chunks);
}

MemoryChunk
MemoryChunk_allocate(int size)
{
  Cell *memory = calloc(size, sizeof(Cell));
  Cell *ptr    = memory;

  for(int i=0; i < size; i++) {
    if(i>0) {
      Cell *prev = ptr; prev--;
      ptr->prev = prev;
    }
    if(i<size) {
      Cell *next = ptr; next++;
      ptr->next = next;
    }
    ptr++;
  }

  Cell *tail = --ptr;

  memory->prev = NULL;
  tail->next   = NULL;

  MemoryChunk chunk = { .head = memory, .tail = tail };
  return chunk;
}

void
GCHeap_grow(GCHeap *heap, int size)
{
  MemoryChunk chunk = MemoryChunk_allocate(size);
  Cell *head = chunk.head;
  Cell *tail = chunk.tail;

  // Save a reference to the chunk to deallocate it later
  DArray_push(heap->chunks, head);

  // Put the new chunk before the current free.
  Cell *oldfree  = FREE;
  Cell *previous = oldfree->prev;

  // Attach tail
  oldfree->prev = tail;
  tail->next    = oldfree;

  // Attach head
  previous->next = head;
  head->prev     = previous;

  FREE = head;
}


int
GCHeap_size(GCHeap *heap)
{
  int count = 1;
  Cell *ptr = TOP;
  while((ptr = ptr->next) && ptr != TOP) count++;

  return count;
}

static inline int
GCHeap_distance_between(Cell *a, Cell *b)
{
  if (a == b) return 0;
  int count = 1;
  Cell *ptr = a;
  while((ptr = ptr->next) && ptr != b) count++;

  return count;
}

static inline void
GCHeap_print_between(Cell *a, Cell *b)
{
  if (a == b) {
    printf("No cells.\n");
    return;
  }
  Cell *ptr = a;
  printf("    * %p (ecru? %i)", a, a->ecru);
  if(a->value) {
    VALUE v = (VALUE)a->value;
    if(v->type == StringType) {
      printf(" %s", VAL2STR(v));
    } else if (v->type == NumberType) {
      printf(" %i", VAL2INT(v));
    } else {
      printf(" type %i", v->type);
    }
  }
  printf("\n");

  while((ptr = ptr->next) && ptr != b) {
   printf("    * %p (ecru? %i)", ptr, ptr->ecru);
   if(ptr->value) {
     VALUE v = (VALUE)ptr->value;
     if(v->type == StringType) {
       printf(" %s", VAL2STR(v));
     } else if (v->type == NumberType) {
       printf(" %i", VAL2INT(v));
     } else {
       printf(" type %i", v->type);
     }
   }

   printf("\n");
  }
}

int
GCHeap_white_size(GCHeap *heap)
{
  if(FREE == BOTTOM &&
     FREE == TOP &&
     FREE == SCAN) {
    return GCHeap_size(heap);
  }

  return GCHeap_distance_between(FREE, BOTTOM);
}

int
GCHeap_ecru_size(GCHeap *heap)
{
  return GCHeap_distance_between(BOTTOM, TOP);
}

int
GCHeap_grey_size(GCHeap *heap)
{
  return GCHeap_distance_between(TOP, SCAN);
}

int
GCHeap_black_size(GCHeap *heap)
{
  return GCHeap_distance_between(SCAN, FREE);
}

void
GCHeap_print_detailed(GCHeap *heap)
{
  printf("  bottom: %p\n", BOTTOM);
  printf("  top: %p\n", TOP);
  printf("  scan: %p\n", SCAN);
  printf("  free: %p\n", FREE);

  printf("GC Heap (%i)\n", GCHeap_size(heap));
  printf("  bottom: %p\n", BOTTOM);
  printf("    ecru: %i\n", GCHeap_ecru_size(heap));
  GCHeap_print_between(BOTTOM, TOP);
  printf("  top: %p\n", TOP);
  printf("    grey: %i\n", GCHeap_grey_size(heap));
  GCHeap_print_between(TOP, SCAN);
  printf("  scan: %p\n", SCAN);
  printf("    black: %i\n", GCHeap_black_size(heap));
  GCHeap_print_between(SCAN, FREE);
  printf("  free: %p\n", FREE);
  printf("    white: %i\n\n", GCHeap_white_size(heap));
  GCHeap_print_between(FREE, BOTTOM);
}

void
GCHeap_print(GCHeap *heap)
{
  printf("  bottom: %p\n", BOTTOM);
  printf("  top: %p\n", TOP);
  printf("  scan: %p\n", SCAN);
  printf("  free: %p\n", FREE);

  printf("GC Heap (%i)\n", GCHeap_size(heap));
  printf("  bottom: %p\n", BOTTOM);
  printf("    ecru: %i\n", GCHeap_ecru_size(heap));
  printf("  top: %p\n", TOP);
  printf("    grey: %i\n", GCHeap_grey_size(heap));
  printf("  scan: %p\n", SCAN);
  printf("    black: %i\n", GCHeap_black_size(heap));
  printf("  free: %p\n", FREE);
  printf("    white: %i\n\n", GCHeap_white_size(heap));
}

void
gc_unsnap(GCHeap *heap, Cell* me) {
  Cell *my_prev = me->prev;
  Cell *my_next = me->next;
  me->next = NULL;
  me->prev = NULL;

  if(BOTTOM == me) BOTTOM = my_next;
  if(TOP    == me) TOP    = my_next;
  if(SCAN   == me) SCAN   = my_next;
  if(FREE   == me) FREE   = my_next;

  my_prev->next = my_next;
  my_next->prev = my_prev;
}

void
gc_insert_in(GCHeap *heap, Cell* me, Cell* him) {
  if(me == him) return; // we do nothing

  gc_unsnap(heap, me);

  Cell *his_prev = him->prev;

  his_prev->next = me;
  him->prev      = me;

  me->prev  = his_prev;
  me->next  = him;

  if(him == TOP)    TOP = me;
  if(him == BOTTOM) BOTTOM = me;
  if(him == SCAN)   SCAN = me;
  if(him == FREE)   FREE = me;
}

void
gc_release(Cell *cell)
{
  Value_destroy((VALUE)cell->value);
}

