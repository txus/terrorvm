#ifndef _tvm_vm_h_
#define _tvm_vm_h_

#include <terror/stack.h>
#include <terror/state.h>
#include <terror/value.h>
#include <terror/input_reader.h>

VALUE VM_run(STATE);
void VM_start(bstring filename);

void Stack_print(Stack* stack);
#define ABORT() \
  CallFrame_print_backtrace(CURR_FRAME); \
  exit(EXIT_FAILURE)

#ifdef DEBUGI
#define debugi(M, ...) debug(M, ##__VA_ARGS__)
#else
#define debugi(M, ...)
#endif

#endif
