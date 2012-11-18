#ifndef _fx_vm_h_
#define _fx_vm_h_

#include <forkix/stack.h>
#include <forkix/state.h>

typedef enum {
  NOOP = 0,
  PUSHINT,

  PUSHLOCAL = 0x20,

  ADD,

  POP,

  SEND = 0x80,
  RET = 0x90,

  DUMP,
} OpCode;

void VM_run(STATE state, Stack *frames);
void VM_start();

#endif
