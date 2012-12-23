#ifndef _fx_value_type_h_
#define _fx_value_type_h_

typedef enum {
  IntegerType = 0,
  ObjectType,
  StringType,
  ArrayType,
  HashType,
  ClosureType,
  TrueType,
  FalseType,
  NilType,
  MainType,
} ValueType;

#endif
