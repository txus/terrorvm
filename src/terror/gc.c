#include <terror/gc.h>
#include <terror/state.h>
#include <stdlib.h>

VALUE
gc_alloc(STATE)
{
  /*
   * Advance the "free" pointer,
   * swallowing an ecru cell into black.
   */

  GCHeader *old_ecru = (GCHeader*)(state->heap->free);

  /*
   * If there are no slots in the freelist,
   * WHAT DO?
   */
  if(old_ecru == state->heap->scan) {
  }

  VALUE val = calloc(1, sizeof(val_t));
  check(val, "Out of memory.");

  GCHeader *black    = old_ecru->prev;
  GCHeader *new_ecru = old_ecru->next;

  GCHeader *allocated = (GCHeader*)val;

  black->next       = allocated;
  new_ecru->prev    = allocated;
  state->heap->free = new_ecru;

  return val;
error:
  return NULL;
}

Heap*
Heap_new()
{
  GCHeader *memory = calloc(1, sizeof(val_t));
  memory->prev = memory;
  memory->next = memory;

  Heap *heap = calloc(1, sizeof(Heap));

  heap->bottom =
    heap->top  =
    heap->free =
    heap->scan = memory;

  return heap;
}
