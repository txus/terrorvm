#ifndef _tvm_debugger_h_
#define _tvm_debugger_h_

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
  struct bstrList* current_file;
};

typedef struct debugger_s Debugger;

Debugger* Debugger_new();
void Debugger_stop(struct state_s* state);
void Debugger_prompt(struct state_s* state);
void Debugger_load_current_file(struct state_s* state);
void Debugger_print_context(struct state_s* state);

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
    (S)->dbg->line_changed = 0; \
  }

#endif
