#ifndef _fx_opcode_h_
#define _fx_opcode_h_

typedef enum {
  NOOP = 0,

  PUSHSELF = 0x10,  // 16
  PUSH,             // 17
  PUSHTRUE,         // 18
  PUSHFALSE,        // 19
  PUSHNIL,          // 20

  PUSHLOCAL = 0x20, // 32
  SETLOCAL,         // 33

  ADD,              // 34

  JMP = 0x30,       // 48
  JIF,              // 49
  JIT,              // 50

  GETSLOT = 0x40,   // 64
  SETSLOT,          // 65

  POP,

  SEND = 0x80,      // 128
  RET = 0x90,       // 144

  DUMP,             // 145
} OpCode;

#endif
