#include <stdio.h>
#include <forkix/value.h>
#include <forkix/bstrlib.h>
#include <forkix/runtime.h>
#include <forkix/primitives.h>
#include <forkix/gc.h>

VALUE NilObject;

VALUE
Value_new(ValueType type)
{
  VALUE val = gc_alloc(sizeof(val_t));
  val->type = type;
  val->table = Hashmap_create(NULL, NULL);
  return val;
};

void
Value_destroy(VALUE o)
{
  gc_dealloc(o);
}

void
Value_print(VALUE o)
{
  switch(o->type) {
    case IntegerType: {
      printf("#<Integer %p @value=%i>\n", o, VAL2INT(o));
      break;
    }
    case StringType: {
      printf("#<String %p @value=%s>\n", o, VAL2STR(o));
      break;
    }
    case TrueType: {
      printf("#<True %p>\n", o);
      break;
    }
    case FalseType: {
      printf("#<False %p>\n", o);
      break;
    }
    case NilType: {
      printf("#<Nil %p>\n", o);
      break;
    }
    case MainType: {
      printf("#<Main %p>\n", o);
      break;
    }
    case ClosureType: {
      printf("#<Closure %p>\n", o);
      break;
    }
    default: {
      printf("#<Object %p>\n", o);
      break;
    }
  }
}

#define DEFNATIVE(V, N, F) Value_set((V), (N), Closure_new(Function_native_new((F))))

VALUE
Main_new()
{
  VALUE val = Value_new(MainType);
  DEFNATIVE(val, "print", Primitive_print);
  DEFNATIVE(val, "puts", Primitive_puts);
  return val;
}

VALUE
Integer_new(int num)
{
  VALUE val = Value_new(IntegerType);
  val->data.as_int = num;

  DEFNATIVE(val, "+", Primitive_Integer_add);
  DEFNATIVE(val, "-", Primitive_Integer_sub);
  DEFNATIVE(val, "*", Primitive_Integer_mul);
  DEFNATIVE(val, "/", Primitive_Integer_div);

  return val;
}

VALUE
String_new(char* value)
{
  VALUE val = Value_new(StringType);
  val->data.as_str = value;
  return val;
}

VALUE
Closure_new(Function *fn)
{
  VALUE val = Value_new(ClosureType);
  val->data.as_data = fn;
  return val;
}

void
Value_set(VALUE receiver, char *key, VALUE value)
{
    bstring _slotname = bfromcstr(key);
    Hashmap_delete(receiver->table, _slotname);
    Hashmap_set(receiver->table, _slotname, value);
}

VALUE
Value_get(VALUE receiver, char *key)
{
  return (VALUE)Hashmap_get(receiver->table, bfromcstr(key));
}

