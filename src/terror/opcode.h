#ifndef _tvm_opcode_h_
#define _tvm_opcode_h_

typedef enum {
  NOOP = 0,
  SETLINE,

  PUSHSELF = 0x10,  // 16
  PUSH,             // 17
  PUSHTRUE,         // 18
  PUSHFALSE,        // 19
  PUSHNIL,          // 20
  PUSHLOBBY,        // 21

  PUSHLOCAL = 0x20, // 32
  SETLOCAL,         // 33
  PUSHLOCALDEPTH,   // 34
  SETLOCALDEPTH,    // 35

  JMP = 0x30,       // 48
  JIF,              // 49
  JIT,              // 50
  GOTO,             // 51

  GETSLOT = 0x40,   // 64
  SETSLOT,          // 65

  POP,              // 66
  DEFN,             // 67
  MAKEVEC,          // 68

  SEND = 0x80,      // 128
  RET = 0x90,       // 144

  DUMP,             // 145
} OpCode;



#endif
