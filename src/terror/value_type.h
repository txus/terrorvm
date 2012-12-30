#ifndef _tvm_value_type_h_
#define _tvm_value_type_h_

typedef enum {
  NumberType = 0,
  ObjectType,
  StringType,
  VectorType,
  MapType,
  ClosureType,
  TrueType,
  FalseType,
  NilType,
} ValueType;

#endif
