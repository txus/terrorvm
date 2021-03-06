#include <terror/state.h>
#include <terror/debugger.h>
#include <terror/file_utils.h>
#include <terror/vm.h>
#include <sweeper/sweeper.h>
#include "stdlib.h"
#include "stdio.h"

Debugger*
Debugger_new()
{
  Debugger *debugger     = calloc(1, sizeof(Debugger));
  debugger->line_changed = 0;
  debugger->step         = STEP_LINE;
  debugger->breakpoints  = DArray_create(sizeof(int), 10);
  debugger->current_line = -1;
  debugger->current_file = NULL;
  return debugger;
}

void
Debugger_destroy(Debugger* debugger)
{
  DArray_clear_destroy(debugger->breakpoints);
  if (debugger->current_file) bstrListDestroy(debugger->current_file);
  free(debugger);
}

Breakpoint*
Breakpoint_new(char *filename, int line)
{
  Breakpoint *breakpoint = calloc(1, sizeof(Breakpoint));
  breakpoint->filename = filename;
  breakpoint->line     = line;
  return breakpoint;
}

void
Debugger_load_current_file(STATE)
{
  if(DEBUGGER->current_file) {
    bstrListDestroy(DEBUGGER->current_file);
  }
  DEBUGGER->current_file = NULL;

  if(!CURR_FRAME->fn->filename) return;

  bstring current_filename = bfromcstr(CURR_FRAME->fn->filename);

  bstring buf = readfile(current_filename);
  bdestroy(current_filename);

  if(!buf) return;

  struct bstrList *lines = bsplit(buf, '\n');

  if(lines) {
    // Insert line counts
    bstring *l = lines->entry;

    for(int i=1; i <= lines->qty; i++) {
      char linenum[8];
      sprintf(linenum, "%-7i", i);
      bstring _linenum = bfromcstr(linenum);

      binsert(*l, 0, _linenum, ' ');
      bdestroy(_linenum);
      l++;
    }

    DEBUGGER->current_file = lines;
  };

  bdestroy(buf);
}

void
Debugger_stop(STATE)
{
  switch(DEBUGGER->step) {
    case STEP_INS: {
      break;
    }
    case STEP_LINE: {
      break;
    }
  }
  Debugger_prompt(state);
}

void
Debugger_prompt(STATE)
{
  int cont = 0;
  while(!cont) {
    Debugger_print_context(state);

    printf("> ");

    char cmd[100];
    char arg[100];
    scanf("%s", cmd);

    switch(cmd[0]) {
      case 'h':
        printf("\nCommands");
        printf("\n--------");
        printf("\nh: show this help");
        printf("\ns: step to the next bytecode instruction");
        printf("\nn: step to the next line of code");
        printf("\nc: continue execution");
        printf("\nd: show the stack");
        printf("\nl: show locals");
        printf("\nt: show backtrace");
        printf("\ng: show GC stats");
        printf("\nb: set breakpoint in a line. Example: b 30");
        printf("\n\n");
        break;
      case 's':
        DEBUGGER->step = STEP_INS;
        cont = 1;
        break;
      case 'b': {
        scanf("%s", arg);
        Debugger_set_breakpoint(state, atoi(arg));
        break;
      }
      case 'p':
        scanf("%s", arg);
        if(strcmp(arg, "self") == 0) {
          printf("\n");
          Value_print(state, CURR_FRAME->self);
          printf("\n");
        }
        break;
      case 't': {
        printf("\nBacktrace");
        printf("\n---------\n");
        CallFrame_print_backtrace(CURR_FRAME);
        break;
      }
      case 'g': {
        printf("\nGC");
        printf("\n--\n");
        SWPHeap_print(state->heap);
        break;
      }
      case 'l': {
        printf("\nLocals");
        printf("\n------");
        for(int i=0; i < DArray_count(CURR_FRAME->locals); i++) {
          printf("\n%i. ", i);
          Value_print(state, (VALUE)DArray_at(CURR_FRAME->locals, i));
          printf("\n");
        }
        break;
      }
      case 'd':
        Stack_print(state, STACK);
        cont = 1;
        break;
      case 'n':
        DEBUGGER->step = STEP_LINE;
        cont = 1;
        break;
      case 'c':
        DEBUGGER->step = STEP_NONE;
        cont = 1;
        break;
      case EOF:
        DEBUGGER->step = STEP_NONE;
        cont = 1;
        break;
      default:
        printf("I don't understand.\n");
        break;
    }
  }
}

void
Debugger_print_context(STATE)
{
  int ctx = 3;
  int ctx_start = DEBUGGER->current_line - ctx;
  int ctx_end   = DEBUGGER->current_line + ctx;

  bstring current_filename = bfromcstr(CURR_FRAME->fn->filename);
  printf("\n%s:%i\n", bdata(current_filename), DEBUGGER->current_line);
  bdestroy(current_filename);

  DArray *lines = getlines(DEBUGGER->current_file, ctx_start, ctx_end);

  for(int i=0; i < DArray_count(lines); i++) {
    bstring curr_line = (bstring)DArray_at(lines, i);

    if(DEBUGGER->current_line == atoi(bdata(curr_line))) {
      curr_line = bstrcpy((bstring)DArray_at(lines, i));
      breplace(curr_line, 5, 1, bfromcstr(">"), ' ');
    }
    printf("%s\n", bdata(curr_line));
  }
  printf("\n");
}


void
Debugger_set_breakpoint(STATE, int line)
{
  printf("Set breakpoint at line %i.", line);
  DArray_push(DEBUGGER->breakpoints, Breakpoint_new(CURR_FRAME->fn->filename, line));
}

void
Debugger_breakpoint(STATE)
{
  for(int i=0; i < DArray_count(DEBUGGER->breakpoints); i++) {
    Breakpoint *bp = (Breakpoint*)DArray_at(DEBUGGER->breakpoints, i);
    if(bp->line == DEBUGGER->current_line && 
       strcmp(bp->filename, CURR_FRAME->fn->filename) == 0) {
      printf("Breakpoint");
      Debugger_stop(state);
      DEBUGGER->current_line++; // to avoid breaking in the same point after 'c'
    }
  }
}
