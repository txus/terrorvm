#include <forkix/runtime.h>

// Blueprints
VALUE Integer_bp = NULL;
VALUE String_bp = NULL;
VALUE Vector_bp = NULL;
VALUE Map_bp = NULL;
VALUE Closure_bp = NULL;

// Static objects
VALUE TrueObject  = NULL;
VALUE FalseObject = NULL;
VALUE NilObject   = NULL;

void Runtime_init() {
  Integer_bp = Value_new(ObjectType);
  String_bp = Value_new(ObjectType);
  Vector_bp = Value_new(ObjectType);
  Map_bp = Value_new(ObjectType);
  Closure_bp = Value_new(ObjectType);

  // Init extern constants
  TrueObject  = Value_new(TrueType);
  FalseObject = Value_new(FalseType);
  NilObject   = Value_new(NilType);
}

void Runtime_destroy() {
  Value_destroy(Integer_bp);
  Value_destroy(String_bp);
  Value_destroy(Vector_bp);
  Value_destroy(Map_bp);
  Value_destroy(Closure_bp);
  Integer_bp = NULL;
  String_bp = NULL;
  Vector_bp = NULL;
  Map_bp = NULL;
  Closure_bp = NULL;

  Value_destroy(TrueObject);
  TrueObject = NULL;

  Value_destroy(FalseObject);
  FalseObject = NULL;

  Value_destroy(NilObject);
  NilObject = NULL;
}
