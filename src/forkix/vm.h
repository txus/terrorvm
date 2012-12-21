#ifndef _fx_vm_h_
#define _fx_vm_h_

#include <forkix/stack.h>
#include <forkix/state.h>
#include <forkix/value.h>
#include <forkix/input_reader.h>

VALUE VM_run(STATE state, Stack *frames);
void VM_start(BytecodeFile *file);

#endif
