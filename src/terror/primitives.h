#ifndef _tvm_primitives_h_
#define _tvm_primitives_h_

#include <terror/value.h>
#include <terror/state.h>
#include <assert.h>

#define CHECK_TYPE(O, T) check((O)->type == (T), "Type mismatch in primitive operation")
#define CHECK_PRESENCE(O) check((O), "Missing argument in primitive operation")

#define PRIMITIVE_ARGS STATE, void*, void*, void*
// Generic primitive methods
VALUE Primitive_to_s(PRIMITIVE_ARGS);
VALUE Primitive_prototype(PRIMITIVE_ARGS);
VALUE Primitive_or(PRIMITIVE_ARGS);
VALUE Primitive_equals(PRIMITIVE_ARGS);
VALUE Primitive_is(PRIMITIVE_ARGS);
VALUE Primitive_clone(PRIMITIVE_ARGS);
VALUE Primitive_print(PRIMITIVE_ARGS);
VALUE Primitive_puts(PRIMITIVE_ARGS);
VALUE Primitive_require(PRIMITIVE_ARGS);

// Number primitive methods
VALUE Primitive_Number_add(PRIMITIVE_ARGS);
VALUE Primitive_Number_sub(PRIMITIVE_ARGS);
VALUE Primitive_Number_mul(PRIMITIVE_ARGS);
VALUE Primitive_Number_div(PRIMITIVE_ARGS);
VALUE Primitive_Number_gt(PRIMITIVE_ARGS);
VALUE Primitive_Number_lt(PRIMITIVE_ARGS);

// String primitive methods
VALUE Primitive_String_concat(PRIMITIVE_ARGS);

// Vector primitive methods
VALUE Primitive_Vector_at(PRIMITIVE_ARGS);
VALUE Primitive_Vector_push(PRIMITIVE_ARGS);
VALUE Primitive_Vector_to_map(PRIMITIVE_ARGS);
VALUE Primitive_Vector_each(PRIMITIVE_ARGS);
VALUE Primitive_Vector_each_with_index(PRIMITIVE_ARGS);

// Map primitive methods
VALUE Primitive_Map_get(PRIMITIVE_ARGS);
VALUE Primitive_Map_set(PRIMITIVE_ARGS);
VALUE Primitive_Map_each(PRIMITIVE_ARGS);

#endif
