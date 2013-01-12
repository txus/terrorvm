#ifndef _tvm_vm_h_
#define _tvm_vm_h_

#include <terror/stack.h>
#include <terror/state.h>
#include <terror/value.h>
#include <terror/input_reader.h>

VALUE VM_run(STATE);
void VM_start(bstring filename);

void Stack_print(Stack* stack);

#endif
