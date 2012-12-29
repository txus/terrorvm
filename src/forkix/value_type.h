#ifndef _fx_value_type_h_
#define _fx_value_type_h_

typedef enum {
  IntegerType = 0,
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
