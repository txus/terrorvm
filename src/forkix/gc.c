#include <forkix/gc.h>
#include <stdlib.h>

VALUE
gc_alloc(size_t size)
{
  VALUE val = calloc(1, size);
  check(val, "Out of memory.");
  return val;
error:
  return NULL;
}

void
gc_dealloc(VALUE obj)
{
  free(obj);
  obj = NULL;
}
