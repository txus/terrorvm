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
      printf("%i\n", VAL2INT(o));
      break;
    }
    case StringType: {
      printf("\"%s\"\n", VAL2STR(o));
      break;
    }
    case TrueType: {
      printf("true\n");
      break;
    }
    case FalseType: {
      printf("false\n");
      break;
    }
    case NilType: {
      printf("nil\n");
      break;
    }
    case ClosureType: {
      printf("#<Closure %p>\n", o);
      break;
    }
    case VectorType: {
      printf("#<Vector %p>\n", o);
      break;
    }
    case MapType: {
      printf("#<Map %p>\n", o);
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
Lobby_new()
{
  VALUE val = Value_new(ObjectType);
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

VALUE
Vector_new(DArray *array)
{
  VALUE val = Value_new(VectorType);
  val->data.as_data = array;

  DEFNATIVE(val, "[]", Primitive_Vector_at);
  DEFNATIVE(val, "to_map", Primitive_Vector_to_map);

  return val;
}

VALUE
Map_new(DArray *array)
{
  VALUE val = Value_new(MapType);

  int count = DArray_count(array);
  assert(count % 2 == 0 && "Map element count must be even.");

  // Maps are our 'object literals', so the object table will be the hash
  // itself.
  Hashmap *hash = val->table;

  for(int i=0; i < count; i += 2) {
    VALUE key   = (VALUE)DArray_at(array, i);
    VALUE value = (VALUE)DArray_at(array, i+1);
    assert(key->type == StringType && "All map keys must be strings.");

    Hashmap_set(hash, bfromcstr(VAL2STR(key)), value);
  }

  DEFNATIVE(val, "[]", Primitive_Map_get);
  DEFNATIVE(val, "[]=", Primitive_Map_set);

  return val;
};

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

