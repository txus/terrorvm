#ifndef _fx_vm_h_
#define _fx_vm_h_

#include <forkix/stack.h>
#include <forkix/state.h>
#include <forkix/input_reader.h>

typedef enum {
  NOOP = 0,

  PUSHSELF = 0x10,  // 16
  PUSHINT,          // 17

  PUSHLOCAL = 0x20, // 32

  ADD,              // 33

  POP,

  SEND = 0x80,      // 128
  RET = 0x90,       // 144

  DUMP,             // 145
} OpCode;

void VM_run(STATE state, Stack *frames);
void VM_start(BytecodeFile *file);

#endif
