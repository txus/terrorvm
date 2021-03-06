#include <stdio.h>
#include <string.h>
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
Value_new(STATE, ValueType type)
{
  VALUE val = (VALUE)GC_allocate(state->heap);
  val->type = type;
  val->table = Hashmap_create(NULL, NULL);
  val->fields = DArray_create(sizeof(VALUE), 10);
  val->prototype = Object_bp;
  return val;
};

VALUE
Value_from_prototype(STATE, ValueType type, VALUE prototype) {
  VALUE val = Value_new(state, type);
  val->prototype = prototype;
  return val;
}

static inline int
Hashmap_destroy_bstring_key(HashmapNode *node)
{
  bdestroy(node->key);
  return 0;
}

void
Value_destroy(VALUE o)
{
  if(o) {
    o->prototype = NULL;
    switch(o->type) {
    case StringType:
      if(o->data.as_str) free(o->data.as_str);
      break;
    case VectorType:
      DArray_destroy((DArray*)(o->data.as_data));
      break;
    case ClosureType:
      if(VAL2FN(o) && VAL2FN(o)->scope) {
        CallFrame_release(VAL2FN(o)->scope);
      }
      break;
    default:
      break;
    }
    DArray_destroy(o->fields);
    Hashmap_traverse(o->table, Hashmap_destroy_bstring_key);
    Hashmap_destroy(o->table);

    free(o);
  }
}

#define append(B, S) { bstring x = bfromcstr(S); bconcat((B), x); bdestroy(x); }
#define appendf(B, S, ...) { bstring x = bformat(S, ##__VA_ARGS__); bconcat((B), x); bdestroy(x); }

VALUE
Value_to_s(STATE, VALUE o)
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
        bdestroy(str);
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
          append(str, VAL2STR(Value_to_s(state, val)));
          if(idx+1 < _count) append(str, ", ");
        });
        append(str, "]");
        break;
      }
      case MapType: {
        append(str, "{");
        Value_each(o, ^ void (VALUE key, VALUE val) {
          appendf(str, "%s => ", VAL2STR(key));
          append(str, VAL2STR(Value_to_s(state, val)));
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
          append(str, VAL2STR(Value_to_s(state, val)));
          append(str,"\n");
        });
        append(str, "}>");
        break;
      }
    }
  }

  VALUE result = String_new(state, bdata(str));
  bdestroy(str);
  return result;
}

void
Value_print(STATE, VALUE o)
{
  printf("%s", VAL2STR(Value_to_s(state, o)));
}

VALUE
Lobby_new(STATE)
{
  VALUE val = Value_new(state, ObjectType);
  return val;
}

VALUE
Number_new(STATE, double num)
{
  VALUE val = Value_from_prototype(state, NumberType, Number_bp);
  val->data.as_num = num;

  return val;
}

VALUE
String_new(STATE, char* value)
{
  VALUE val = Value_from_prototype(state, StringType, String_bp);
  val->data.as_str = strdup(value);
  return val;
}

VALUE
Closure_new(STATE, Function *fn, CallFrame *scope)
{
  VALUE val = Value_from_prototype(state, ClosureType, Closure_bp);
  if(scope) {
    CallFrame_retain(scope);
    fn->scope = scope;
  }

  val->data.as_data = fn;
  return val;
}

VALUE
Vector_new(STATE, DArray *array)
{
  VALUE val = Value_from_prototype(state, VectorType, Vector_bp);
  val->data.as_data = array;

  return val;
}

VALUE
Map_new(STATE, DArray *array)
{
  VALUE val = Value_from_prototype(state, MapType, Map_bp);

  assert(DArray_count(array) % 2 == 0 && "Map element count must be even.");

  // Maps are our 'object literals', so the object table will be the hash
  // itself.
  Hashmap *hash = val->table;

  // Preserve insertion order
  DArray *keys = DArray_create(sizeof(VALUE), 10);
  DArray_destroy(val->fields);
  val->fields = keys;

  while(DArray_count(array) > 0) {
    VALUE key = (VALUE)DArray_pop(array);
    assert(key->type == StringType && "All map keys must be strings.");

    DArray_push(keys, key);

    VALUE value = (VALUE)DArray_pop(array);

    Hashmap_set(hash, bfromcstr(VAL2STR(key)), value);

  }

  DArray_destroy(array);

  return val;
};

void
Value_set(STATE, VALUE receiver, char *key, VALUE value)
{
  bstring _slotname = bfromcstr(key);
  Hashmap_delete(receiver->table, _slotname);
  Hashmap_set(receiver->table, _slotname, value);

  // We need to keep the value safe from collection until it's referenced by
  // `receiver`.
  GC_protect(value);
  VALUE field = String_new(state, key);
  GC_unprotect(value);

  DArray_push(receiver->fields, field);
}

VALUE
Value_get(VALUE receiver, char *key)
{
  bstring _key = bfromcstr(key);
  VALUE result = (VALUE)Hashmap_get(receiver->table, _key);
  bdestroy(_key);

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
      bstring str = bfromcstr(VAL2STR(k));
      VALUE v = (VALUE)Hashmap_get(hash, str);
      bdestroy(str);
      iter(k, v);
    }
  }
}
