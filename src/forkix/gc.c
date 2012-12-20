#include <forkix/gc.h>
#include <stdlib.h>

VALUE
gc_alloc(size_t size)
{
  VALUE val = calloc(1, size);
  return val;
}

void
gc_dealloc(VALUE obj)
{
  free(obj);
  obj = NULL;
}
