#include <stdio.h>
#include <forkix/value.h>
#include <forkix/gc.h>

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
    default: {
      printf("#<Object %p>\n", o);
      break;
    }
  }
}

VALUE
Integer_new(int num)
{
  VALUE val = Value_new(IntegerType);
  val->data.as_int = num;
  return val;
}

VALUE
String_new(char* value)
{
  VALUE val = Value_new(StringType);
  val->data.as_str = value;
  return val;
}
