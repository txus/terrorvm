#include <forkix/runtime.h>

VALUE TrueObject  = NULL;
VALUE FalseObject = NULL;
VALUE NilObject   = NULL;

void Runtime_init() {
  // Init extern constants
  TrueObject  = Value_new(TrueType);
  FalseObject = Value_new(FalseType);
  NilObject   = Value_new(NilType);
}

void Runtime_destroy() {
  Value_destroy(TrueObject);
  TrueObject = NULL;

  Value_destroy(FalseObject);
  FalseObject = NULL;

  Value_destroy(NilObject);
  NilObject = NULL;
}
