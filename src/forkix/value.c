#include <stdio.h>
#include <forkix/value.h>
#include <forkix/bstrlib.h>
#include <forkix/runtime.h>
#include <forkix/primitives.h>
#include <forkix/gc.h>

VALUE Object_bp;
VALUE Integer_bp;
VALUE String_bp;
VALUE Vector_bp;
VALUE Map_bp;
VALUE Closure_bp;

VALUE NilObject;

VALUE
Value_new(ValueType type)
{
  VALUE val = gc_alloc(sizeof(val_t));
  val->type = type;
  val->table = Hashmap_create(NULL, NULL);
  val->prototype = Object_bp;
  return val;
};

VALUE
Value_from_prototype(ValueType type, VALUE prototype)
{
  VALUE val = Value_new(type);
  val->prototype = prototype;
  return val;
}

void
Value_destroy(VALUE o)
{
  gc_dealloc(o);
}

static inline int
Hashmap_print_cb(HashmapNode *node) {
  char *name = bdata((bstring)(node->key));
  VALUE obj = (VALUE)(node->data);
  Value_print(String_new(name));
  printf(" => ");
  Value_print(obj);
  printf(", ");
  return 0;
}

static inline void
__Value_print(VALUE o)
{
  check(o, "Cannot print NULL value");

  // Blueprints
  if(o == Object_bp) { printf("Object"); return;
    } else if (o == Integer_bp) { printf("Integer"); return;
    } else if (o == String_bp) { printf("String"); return;
    } else if (o == Closure_bp) { printf("Closure"); return;
    } else if (o == Vector_bp) { printf("Vector"); return;
    } else if (o == Map_bp) { printf("Map"); return;
  }

  switch(o->type) {
    case IntegerType: {
      printf("%i", VAL2INT(o));
      break;
    }
    case StringType: {
      printf("\"%s\"", VAL2STR(o));
      break;
    }
    case TrueType: {
      printf("true");
      break;
    }
    case FalseType: {
      printf("false");
      break;
    }
    case NilType: {
      printf("nil");
      break;
    }
    case ClosureType: {
      printf("#<Closure %p>", o);
      break;
    }
    case VectorType: {
      printf("#<Vector %p>", o);
      break;
    }
    case MapType: {
      printf("{");
      Hashmap_traverse(o->table, Hashmap_print_cb);
      printf("}");
      break;
    }
    default: {
      printf("#<Object %p ", o);
      printf("{");
      Hashmap_traverse(o->table, Hashmap_print_cb);
      printf("}>");
      break;
    }
  }
error:
  return;
}

void
Value_print(VALUE o)
{
  __Value_print(o);

  return; // for now
  if(o->prototype) {
    printf(" (");
    __Value_print(o->prototype);
    printf(")");
  }
}

VALUE
Lobby_new()
{
  VALUE val = Value_new(ObjectType);
  return val;
}

VALUE
Integer_new(int num)
{
  VALUE val = Value_from_prototype(IntegerType, Integer_bp);
  val->data.as_int = num;

  return val;
}

VALUE
String_new(char* value)
{
  VALUE val = Value_from_prototype(StringType, String_bp);
  val->data.as_str = value;
  return val;
}

VALUE
Closure_new(Function *fn)
{
  VALUE val = Value_from_prototype(ClosureType, Closure_bp);
  val->data.as_data = fn;
  return val;
}

VALUE
Vector_new(DArray *array)
{
  VALUE val = Value_from_prototype(VectorType, Vector_bp);
  val->data.as_data = array;

  return val;
}

VALUE
Map_new(DArray *array)
{
  VALUE val = Value_from_prototype(MapType, Map_bp);

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
  VALUE result = (VALUE)Hashmap_get(receiver->table, bfromcstr(key));

  if(!result && receiver->prototype) {
    result = Value_get(receiver->prototype, key);
  }

  return result;
}

