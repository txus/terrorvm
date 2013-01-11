#include <terror/primitives.h>
#include <terror/runtime.h>
#include <terror/bstrlib.h>
#include <terror/state.h>
#include <terror/call_frame.h>
#include <terror/input_reader.h>
#include <terror/vector.h>
#include <terror/map.h>
#include <terror/vm.h>
#include <assert.h>

/*
 * Generic primitives
 */

VALUE
Primitive_prototype(STATE, void *a, void *_, void *__)
{
  VALUE self      = (VALUE)a;
  VALUE prototype = self->prototype;

  if(!prototype) prototype = NilObject;

  return prototype;
}


VALUE
Primitive_or(STATE, void *a, void *b, void *__)
{
  VALUE self  = (VALUE)a;
  VALUE other = (VALUE)b;

  if(self == NilObject || self == FalseObject) return FalseObject;
  if(other == NilObject || other == FalseObject) return FalseObject;

  return TrueObject;
}

VALUE
Primitive_equals(STATE, void *a, void *b, void *__)
{
  VALUE self  = (VALUE)a;
  VALUE other = (VALUE)b;

  if(self->type != other->type) return FalseObject;
  if(Primitive_is(state, self, other, NULL) == TrueObject) return TrueObject;

  switch(self->type) {
  case NumberType:
    return INT2BOOL(VAL2NUM(self) == VAL2NUM(other));
  case ObjectType:
    // TODO: Deep-analyze objects.
    return FalseObject;
  case StringType:
    return INT2BOOL(strcmp(VAL2STR(self), VAL2STR(other)) == 0);
  case VectorType: {
    __block VALUE _other  = other;
    __block State* _state = state;
    __block int _result   = 1;

    Vector_each_with_index(self, ^ void (VALUE vector, int idx) {
      VALUE equals = Primitive_equals(
        _state,
        Vector_at(vector, idx),
        Vector_at(_other, idx),
        NULL
      );
      if(!VAL2BOOL(equals)) _result = 0;
    });

    return INT2BOOL(_result);
  }
  case MapType:
    // TODO: Deep-analyze maps.
    return FalseObject;
  case ClosureType:
    // TODO: Deep-analyze closures.
    return FalseObject;
  case TrueType:
    return INT2BOOL(other == TrueObject);
  case FalseType:
    return INT2BOOL(other == FalseObject);
  case NilType:
    return INT2BOOL(other == NilObject);
  default:
    break;
  }
  return FalseObject;
}

VALUE
Primitive_is(STATE, void *a, void *b, void *__)
{
  VALUE self  = (VALUE)a;
  VALUE other = (VALUE)b;
  return self == other ? TrueObject : FalseObject;
}

VALUE
Primitive_clone(STATE, void *a, void *_, void *__)
{
  VALUE obj = (VALUE)a;
  return Value_from_prototype(obj->type, obj);
}

VALUE
Primitive_print(STATE, void *_, void *b, void *__)
{
  Value_print((VALUE)b);
  return NilObject;
}

VALUE
Primitive_puts(STATE, void *a, void *b, void *c)
{
  Primitive_print(state, a, b, c);
  printf("\n");
  return NilObject;
}

VALUE
Primitive_require(STATE, void *a, void *_, void *__)
{
  VALUE filename = (VALUE)a;
  CHECK_TYPE(filename, StringType);
  bstring filename_str = bfromcstr(VAL2STR(filename));

  Hashmap *fns = state->functions;

  Function *main = NULL; // entry point

  BytecodeFile *file = BytecodeFile_new(filename_str);
  int fn_count = DArray_count(file->function_names);
  for(int j=0; j < fn_count; j++) {
    bstring fn_name = (bstring)DArray_at(file->function_names, j);
    Function *fn = (Function*)Hashmap_get(file->functions, fn_name);

    if(bstrcmp(fn_name, bfromcstr("main")) == 0) {
      main = fn;
    } else {
      Hashmap_set(fns, fn_name, fn);
    }
  }

  CallFrame *frame = CallFrame_new(state->lobby, main, NULL);
  Stack_push(FRAMES, frame);

  return VM_run(state);
}

/*
 * Number primitives
 */

VALUE
Primitive_Number_add(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, NumberType);
  CHECK_TYPE(right, NumberType);

  return Number_new(VAL2NUM(left) + VAL2NUM(right));
}

VALUE
Primitive_Number_sub(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, NumberType);
  CHECK_TYPE(right, NumberType);

  return Number_new(VAL2NUM(left) - VAL2NUM(right));
}

VALUE
Primitive_Number_mul(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, NumberType);
  CHECK_TYPE(right, NumberType);

  return Number_new(VAL2NUM(left) * VAL2NUM(right));
}

VALUE
Primitive_Number_div(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, NumberType);
  CHECK_TYPE(right, NumberType);

  assert(VAL2NUM(right) != 0 && "Cannot divide by zero");

  return Number_new(VAL2NUM(left) / VAL2NUM(right));
}

/*
 * String primitives
 */

VALUE
Primitive_String_concat(STATE, void *a, void *b, void *_)
{
  VALUE str_a = (VALUE)a;
  VALUE str_b = (VALUE)b;

  CHECK_TYPE(str_a, StringType);
  CHECK_TYPE(str_b, StringType);

  bstring s = bfromcstr(VAL2STR(str_a));
  bconcat(s, bfromcstr(VAL2STR(str_b)));

  return String_new(bdata(s));
}

/*
 * Vector primitives
 */

VALUE
Primitive_Vector_at(STATE, void *a, void *b, void *_)
{
  VALUE vector = (VALUE)a;
  VALUE index  = (VALUE)b;

  CHECK_TYPE(vector, VectorType);
  CHECK_TYPE(index, NumberType);

  VALUE result = Vector_at(vector, VAL2INT(index));

  if(!result) result = NilObject;

  return result;
}

VALUE
Primitive_Vector_push(STATE, void *a, void *b, void *_)
{
  VALUE vector  = (VALUE)a;
  VALUE element = (VALUE)b;

  CHECK_TYPE(vector, VectorType);
  CHECK_PRESENCE(element);

  Vector_push(vector, element);

  return vector;
}

VALUE
Primitive_Vector_each(STATE, void *a, void *b, void *_)
{
  VALUE vector  = (VALUE)a;
  VALUE closure = (VALUE)b;

  CHECK_TYPE(vector, VectorType);
  CHECK_TYPE(closure, ClosureType);

  Vector_each(vector, ^ void (VALUE element) {
    DArray *args = DArray_create(sizeof(VALUE), 10);
    DArray_push(args, element);

    Closure_invoke(state, closure, NULL, args);
  });

  return vector;
}

VALUE
Primitive_Vector_each_with_index(STATE, void *a, void *b, void *_)
{
  VALUE vector  = (VALUE)a;
  VALUE closure = (VALUE)b;

  CHECK_TYPE(vector, VectorType);
  CHECK_TYPE(closure, ClosureType);

  Vector_each_with_index(vector, ^ void (VALUE element, int idx) {
    DArray *args = DArray_create(sizeof(VALUE), 10);
    DArray_push(args, element);
    DArray_push(args, Number_new(idx));

    Closure_invoke(state, closure, NULL, args);
  });

  return vector;
}

VALUE
Primitive_Vector_to_map(STATE, void *a, void *_, void *__)
{
  VALUE vector = (VALUE)a;
  CHECK_TYPE(vector, VectorType);

  return Map_new(VAL2ARY(vector));
}

/*
 * Map primitives
 */

VALUE
Primitive_Map_get(STATE, void *a, void *b, void *_)
{
  VALUE map = (VALUE)a;
  VALUE key = (VALUE)b;

  CHECK_TYPE(key, StringType);

  VALUE result = Value_get(map, VAL2STR(key));
  check(result, "No member named %s.", VAL2STR(key));

  return result;

error:
  return NULL;
}

VALUE
Primitive_Map_set(STATE, void *a, void *b, void *c)
{
  VALUE map   = (VALUE)a;
  VALUE key   = (VALUE)b;
  VALUE value = (VALUE)c;

  CHECK_TYPE(key, StringType);
  CHECK_PRESENCE(value);

  Value_set(map, VAL2STR(key), value);

  return value;
}

VALUE
Primitive_Map_each(STATE, void *a, void *b, void *_)
{
  VALUE map     = (VALUE)a;
  VALUE closure = (VALUE)b;

  CHECK_PRESENCE(map);
  CHECK_TYPE(closure, ClosureType);

  Map_each(map, ^ void (bstring key, VALUE value) {
    DArray *args = DArray_create(sizeof(VALUE), 10);
    DArray_push(args, String_new(bdata(key)));
    DArray_push(args, value);

    Closure_invoke(state, closure, NULL, args);
  });

  return map;
}

