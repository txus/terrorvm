#ifndef _tvm_debugger_h_
#define _tvm_debugger_h_

#include <terror/darray.h>
extern int Debug;

struct state_s;

typedef enum {
  STEP_NONE,
  STEP_INS,
  STEP_LINE
} DebuggerStep;

struct debugger_s {
  int line_changed;
  int current_line;
  int step;
  DArray *breakpoints;
  struct bstrList* current_file;
};

struct breakpoint_s {
  char *filename;
  int line;
};

typedef struct debugger_s Debugger;
typedef struct breakpoint_s Breakpoint;

Debugger* Debugger_new();
void Debugger_stop(struct state_s* state);
void Debugger_prompt(struct state_s* state);
void Debugger_load_current_file(struct state_s* state);
void Debugger_print_context(struct state_s* state);

Breakpoint* Breakpoint_new(char *filename, int line);
void Debugger_set_breakpoint(struct state_s* state, int line);
void Debugger_kill_breakpoint(struct state_s* state, int line);
void Debugger_breakpoint(struct state_s* state);

#define Debugger_setline(S, L) \
  if(Debug) { \
    (S)->dbg->current_line = (L); \
    (S)->dbg->line_changed = 1; \
  }

#define Debugger_evaluate(S) \
  if(Debug) { \
    if((S)->dbg->step == STEP_INS) { \
      Debugger_stop((S)); \
    } else if ((S)->dbg->step == STEP_LINE && (S)->dbg->line_changed) { \
      Debugger_stop((S)); \
    } \
    Debugger_breakpoint((S)); \
    (S)->dbg->line_changed = 0; \
  }

#endif
