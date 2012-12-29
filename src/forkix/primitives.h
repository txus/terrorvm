#ifndef _fx_primitives_h_
#define _fx_primitives_h_

#include <forkix/value.h>
#include <forkix/state.h>
#include <assert.h>

#define CHECK_TYPE(O, T) assert((O)->type == (T) && "Type mismatch in primitive operation")
#define CHECK_PRESENCE(O) assert((O) && "Missing argument in primitive operation")

#define PRIMITIVE_ARGS STATE, void*, void*, void*
// Generic primitive methods
VALUE Primitive_print(PRIMITIVE_ARGS);
VALUE Primitive_puts(PRIMITIVE_ARGS);
VALUE Primitive_require(PRIMITIVE_ARGS);

// Integer primitive methods
VALUE Primitive_Integer_add(PRIMITIVE_ARGS);
VALUE Primitive_Integer_sub(PRIMITIVE_ARGS);
VALUE Primitive_Integer_mul(PRIMITIVE_ARGS);
VALUE Primitive_Integer_div(PRIMITIVE_ARGS);

// Vector primitive methods
VALUE Primitive_Vector_at(PRIMITIVE_ARGS);
VALUE Primitive_Vector_to_map(PRIMITIVE_ARGS);

// Map primitive methods
VALUE Primitive_Map_get(PRIMITIVE_ARGS);
VALUE Primitive_Map_set(PRIMITIVE_ARGS);

#endif
