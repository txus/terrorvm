#include <terror/runtime.h>
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

#define DEFNATIVE(V, N, F) Value_set((V), (N), Closure_new(Function_native_new((F))))

void Runtime_init() {
  Object_bp = Value_new(ObjectType);

  // These primitives cannot go in the prelude because they are used there.
  DEFNATIVE(Object_bp, "[]", Primitive_Map_get);
  DEFNATIVE(Object_bp, "[]=", Primitive_Map_set);

  Number_bp = Value_from_prototype(NumberType, Object_bp);
  String_bp = Value_from_prototype(StringType, Object_bp);
  Vector_bp = Value_from_prototype(VectorType, Object_bp);
  Map_bp = Value_from_prototype(MapType, Object_bp);
  Closure_bp = Value_from_prototype(ClosureType, Object_bp);

  // Init extern constants
  TrueObject  = Value_new(TrueType);
  FalseObject = Value_new(FalseType);
  NilObject   = Value_new(NilType);
}

void Runtime_destroy() {
  Value_destroy(Number_bp);
  Value_destroy(String_bp);
  Value_destroy(Vector_bp);
  Value_destroy(Map_bp);
  Value_destroy(Closure_bp);
  Value_destroy(Object_bp);
  Number_bp = NULL;
  String_bp = NULL;
  Vector_bp = NULL;
  Map_bp = NULL;
  Closure_bp = NULL;
  Object_bp = NULL;

  Value_destroy(TrueObject);
  TrueObject = NULL;

  Value_destroy(FalseObject);
  FalseObject = NULL;

  Value_destroy(NilObject);
  NilObject = NULL;
}
