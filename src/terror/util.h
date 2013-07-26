#ifndef _tvm_util_h_
#define _tvm_util_h_

#ifdef __linux__
#include <string.h>
char *strdup(const char *str)
{
  int n = strlen(str) + 1;
  char *dup = malloc(n);
  if(dup)
  {
    strcpy(dup, str);
  }
  return dup;
}
#endif

#endif
