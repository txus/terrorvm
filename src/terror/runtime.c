#include <terror/runtime.h>
#include <terror/state.h>
#include <terror/value.h>
#include <terror/function.h>
#include <terror/primitives.h>

// Blueprints
VALUE Object_bp = NULL;
VALUE Number_bp = NULL;
VALUE String_bp = NULL;
VALUE Vector_bp = NULL;
VALUE Map_bp = NULL;
VALUE Closure_bp = NULL;

// Static objects
VALUE TrueObject  = NULL;
VALUE FalseObject = NULL;
VALUE NilObject   = NULL;

#define DEFNATIVE(V, N, F) Value_set(state, (V), (N), Closure_new(state, Function_native_new(state, (F)), NULL))

void Runtime_init(STATE) {
  Object_bp = Value_new(state, ObjectType);

  Number_bp = Value_from_prototype(state, NumberType, Object_bp);
  String_bp = Value_from_prototype(state, StringType, Object_bp);
  Vector_bp = Value_from_prototype(state, VectorType, Object_bp);
  Map_bp = Value_from_prototype(state, MapType, Object_bp);
  Closure_bp = Value_from_prototype(state, ClosureType, Object_bp);

  // Init extern constants
  TrueObject  = Value_new(state, TrueType);
  TrueObject->data.as_num = 1;
  FalseObject = Value_new(state, FalseType);
  FalseObject->data.as_num = 0;
  NilObject   = Value_new(state, NilType);
  NilObject->data.as_num = 0;

  VALUE map_get = Closure_new(state, Function_native_new(state, Primitive_Map_get), NULL);
  printf("[] => %p\n", map_get);
  Value_set(state, Object_bp, "[]", map_get);
  // These primitives cannot go in the prelude because they are used there.
  /* DEFNATIVE(Object_bp, "[]", Primitive_Map_get); */
  DEFNATIVE(Object_bp, "[]=", Primitive_Map_set);
}
