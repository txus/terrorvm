#ifndef _tvm_file_utils_h_
#define _tvm_file_utils_h_

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <terror/bstrlib.h>
#include <terror/darray.h>

bstring readfile(bstring filename);
DArray *getlines(struct bstrList *file, int start, int end);

#endif

