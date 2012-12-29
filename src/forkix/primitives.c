#include <forkix/primitives.h>
#include <forkix/runtime.h>
#include <forkix/bstrlib.h>
#include <forkix/state.h>
#include <forkix/call_frame.h>
#include <forkix/input_reader.h>
#include <forkix/vm.h>
#include <assert.h>

/*
 * Generic primitives
 */

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
 * Integer primitives
 */

VALUE
Primitive_Integer_add(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  return Integer_new(VAL2INT(left) + VAL2INT(right));
}

VALUE
Primitive_Integer_sub(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  return Integer_new(VAL2INT(left) - VAL2INT(right));
}

VALUE
Primitive_Integer_mul(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  return Integer_new(VAL2INT(left) * VAL2INT(right));
}

VALUE
Primitive_Integer_div(STATE, void *a, void *b, void *_)
{
  VALUE left  = (VALUE)a;
  VALUE right = (VALUE)b;

  CHECK_TYPE(left, IntegerType);
  CHECK_TYPE(right, IntegerType);

  assert(VAL2INT(right) != 0 && "Cannot divide by zero");

  return Integer_new(VAL2INT(left) / VAL2INT(right));
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
  CHECK_TYPE(index, IntegerType);

  VALUE result = (VALUE)DArray_at(VAL2ARY(vector), VAL2INT(index));

  if(!result) result = NilObject;

  return result;
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

