#include <stdio.h>
#include <terror/value.h>
#include <terror/bstrlib.h>
#include <terror/runtime.h>
#include <terror/primitives.h>
#include <terror/call_frame.h>
#include <terror/vector.h>
#include <terror/gc.h>

VALUE Object_bp;
VALUE Number_bp;
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
  val->fields = DArray_create(sizeof(VALUE), 10);
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

#define append(B, S) bconcat((B), bfromcstr(S))
#define appendf(B, S, ...) bconcat((B), bformat(S, ##__VA_ARGS__))

VALUE
Value_to_s(VALUE o)
{
  bstring str = bfromcstr("");

  // Blueprints
  if(o == Object_bp)          { append(str, "Object"); }
    else if (o == Number_bp)  { append(str, "Number"); }
    else if (o == String_bp)  { append(str, "String"); }
    else if (o == Closure_bp) { append(str, "Closure"); }
    else if (o == Vector_bp)  { append(str, "Vector"); }
    else if (o == Map_bp)     { append(str, "Map"); }
    else {
    switch(o->type) {
      case NumberType: {
        appendf(str, "%f", VAL2NUM(o));
        break;
      }
      case StringType: {
        return o;
        break;
      }
      case TrueType: {
        append(str, "true");
        break;
      }
      case FalseType: {
        append(str, "false");
        break;
      }
      case NilType: {
        append(str, "nil");
        break;
      }
      case ClosureType: {
        Function *fn = VAL2FN(o);
        appendf(str, "#<Closure %p ", o);
        if(fn->c_fn) {
          append(str, "(C code)");
        } else {
          append(str, "[");
          int *ptr = fn->code;
          while(*ptr != 144) {
            appendf(str, "%i, ", *ptr);
            ptr++;
          }
          append(str, "144]"); // RET
        }
        append(str, ">");
        break;
      }
      case VectorType: {
        append(str, "[");
        __block int _count = Vector_count(o);
        Vector_each_with_index(o, ^ void (VALUE val, int idx) {
          append(str, VAL2STR(Value_to_s(val)));
          if(idx+1 < _count) append(str, ", ");
        });
        append(str, "]");
        break;
      }
      case MapType: {
        append(str, "{");
        Value_each(o, ^ void (VALUE key, VALUE val) {
          appendf(str, "%s => ", VAL2STR(key));
          append(str, VAL2STR(Value_to_s(val)));
          append(str, ", ");
        });
        append(str, "}");
        break;
      }
      default: {
        appendf(str, "#<Object %p ", o);
        append(str, "{");
        Value_each(o, ^ void (VALUE key, VALUE val) {
          appendf(str, "%s => ", VAL2STR(key));
          append(str, VAL2STR(Value_to_s(val)));
          append(str,"\n");
        });
        append(str, "}>");
        break;
      }
    }
  }

  return String_new(bdata(str));
}

void
Value_print(VALUE o)
{
  printf("%s", VAL2STR(Value_to_s(o)));
}

VALUE
Lobby_new()
{
  VALUE val = Value_new(ObjectType);
  return val;
}

VALUE
Number_new(double num)
{
  VALUE val = Value_from_prototype(NumberType, Number_bp);
  val->data.as_num = num;

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
Closure_new(Function *fn, CallFrame *scope)
{
  VALUE val = Value_from_prototype(ClosureType, Closure_bp);
  fn->scope = scope;
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

  // Preserve insertion order
  DArray *keys = DArray_create(sizeof(VALUE), 10);
  val->fields = keys;

  for(int i=0; i < count; i += 2) {
    VALUE key   = (VALUE)DArray_at(array, i);
    DArray_push(keys, key);

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
    DArray_push(receiver->fields, String_new(key));
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

void
Value_each(VALUE obj, Slots_iter iter)
{
  Hashmap *hash = VAL2HASH(obj);
  DArray *keys = (DArray*)(obj->fields);

  for(int i = 0; i < DArray_count(keys); i++) {
    VALUE k = (VALUE)DArray_at(keys, i);
    if(k) {
      VALUE v = (VALUE)Hashmap_get(hash, bfromcstr(VAL2STR(k)));
      iter(k, v);
    }
  }
}
