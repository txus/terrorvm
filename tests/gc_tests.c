#include "minunit.h"
#include <terror/gc.h>
#include <terror/value.h>
#include <terror/state.h>
#include <assert.h>

STATE = NULL;

#define ASSERT_SIZES(E, G, B, W) { \
  mu_assert(GCHeap_ecru_size(heap) == (E), "Wrong ecru size.");  \
  mu_assert(GCHeap_grey_size(heap) == (G), "Wrong grey size.");  \
  mu_assert(GCHeap_black_size(heap) == (B), "Wrong black size.");  \
  mu_assert(GCHeap_white_size(heap) == (W), "Wrong white size.");  \
}

char *test_GCHeap_new()
{
  GCHeap *heap   = GCHeap_new(8, 8);

  int size       = GCHeap_size(heap);

  mu_assert(TOP == BOTTOM &&
            TOP == SCAN &&
            TOP == FREE,
            "Pointers are not set to the correct point.");

  mu_assert(size == 8, "Wrong Heap size");

  // Ecru, Grey, Black, White
  ASSERT_SIZES(0, 0, 0, 8);

  return NULL;
}

char *test_memory_chunk()
{
  MemoryChunk chunk = MemoryChunk_allocate(5);

  Cell *a = chunk.head;
  Cell *b = a->next;
  Cell *c = b->next;
  Cell *d = c->next;
  Cell *e = chunk.tail;

#define A(E) mu_assert((E), "Failed: " #E)

  A(a->prev == NULL);
  A(a->next == b);
  A(b->prev == a);
  A(b->next == c);
  A(c->prev == b);
  A(c->next == d);
  A(d->prev == c);
  A(d->next == e);
  A(e->prev == d);
  A(e->next == NULL);

  return NULL;
}

char *test_unsnap()
{
  GCHeap *heap = calloc(1, sizeof(GCHeap));

  MemoryChunk chunk = MemoryChunk_allocate(3);
  Cell *head = chunk.head;
  Cell *tail = chunk.tail;

  head->prev = tail;
  tail->next = head;

  Cell *a = head;
  Cell *b = a->next;
  Cell *c = b->next;

  gc_unsnap(heap, b);

  mu_assert(a->next == c, "a->next should == c");
  mu_assert(c->prev == a, "c->prev should == a");
  mu_assert(c->next == a, "c->next should == a");
  mu_assert(a->prev == c, "a->prev should == c");
  mu_assert(b->next == NULL, "b->next should == NULL");
  mu_assert(b->prev == NULL, "b->prev should == NULL");

  return NULL;
}

char *test_insert_in()
{
  GCHeap *heap = calloc(1, sizeof(GCHeap));

  MemoryChunk chunk = MemoryChunk_allocate(5);
  Cell *head = chunk.head;
  Cell *tail = chunk.tail;

  head->prev = tail;
  tail->next = head;

  Cell *a = head;
  // here is where we'll put e
  Cell *b = a->next;
  Cell *c = b->next;
  Cell *d = c->next;
  Cell *e = d->next;

  gc_insert_in(heap, e, b);

  mu_assert(e->prev == a, "e->prev should == a");
  mu_assert(e->next == b, "e->next should == b");

  mu_assert(a->next == e, "a->next should == e");
  mu_assert(b->prev == e, "b->prev should == e");

  mu_assert(d->next == a, "d->next should == a");
  mu_assert(a->prev == d, "a->prev should == d");

  return NULL;
}

char *test_alloc()
{
  GCHeap *heap = GCHeap_new(5, 10);

  STATE = State_new();

  VALUE val = gc_alloc(state, heap);
  Cell *cell = val->gc.cell;

  mu_assert(cell->next == FREE, "Cell->next should be 'free'");
  mu_assert(cell == FREE->prev, "Cell should be after 'free'");
  mu_assert(cell->value == val,    "Cell didn't get a value");

  mu_assert(GCHeap_white_size(heap) == 4, "White size should be 3");
  mu_assert(GCHeap_black_size(heap) == 1, "Black size should be 2");

  return NULL;
}

char *test_pointers()
{
  GCHeap *heap = GCHeap_new(5, 5);

  GC_PUSH_FREE();
  GC_PUSH_FREE();
  // Ecru, Grey, Black, White
  ASSERT_SIZES(0, 0, 2, 3)

  GC_PUSH_SCAN();
  // Ecru, Grey, Black, White
  ASSERT_SIZES(0, 1, 1, 3)

  GC_PULL_BOTTOM();
  // Ecru, Grey, Black, White
  ASSERT_SIZES(1, 1, 1, 2)

  GC_PUSH_TOP();
  // Ecru, Grey, Black, White
  ASSERT_SIZES(2, 0, 1, 2)

  return NULL;
}

VALUE create_map(STATE)
{
  DArray *array = DArray_create(sizeof(VALUE), 10);
  return Map_new(state, array);
}

char *test_scan()
{
  STATE = State_new();
  GCHeap *heap = state->heap;

  VALUE map = create_map(state);
  VALUE foo = Number_new(state, 123);
  VALUE bar = Number_new(state, 999);
  Value_set(state, map, "foo", foo);
  Value_set(state, map, "bar", bar);

  DArray_push(state->rootset, map);

  gc_flip(NULL, heap);
  gc_prepare_to_scan(heap, state->rootset);
  gc_scan(heap);

  // Map should be black, its children should be grey and there should be still
  // 5 white slots.

  // Ecru, Grey, Black, White
  ASSERT_SIZES(0, 4, 1, 15);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_GCHeap_new);
  mu_run_test(test_memory_chunk);
  mu_run_test(test_unsnap);
  mu_run_test(test_insert_in);
  mu_run_test(test_alloc);
  mu_run_test(test_pointers);
  mu_run_test(test_scan);

  return NULL;
}

RUN_TESTS(all_tests);
