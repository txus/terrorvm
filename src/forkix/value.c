#include <forkix/value.h>
#include <forkix/gc.h>

VALUE
Value_new(ValueType type)
{
  VALUE val = gc_alloc(sizeof(val_t));
  val->type = type;
  return val;
};

void
Value_destroy(VALUE o)
{
  gc_dealloc(o);
}

VALUE
Integer_new(int num)
{
  VALUE val = Value_new(IntegerType);
  val->data.as_int = num;
  return val;
}

