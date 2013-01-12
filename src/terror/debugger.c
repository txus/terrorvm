#include <terror/state.h>
#include <terror/debugger.h>
#include <terror/file_utils.h>
#include <terror/vm.h>
#include "stdlib.h"
#include "stdio.h"

Debugger*
Debugger_new()
{
  Debugger *debugger     = calloc(1, sizeof(Debugger));
  debugger->line_changed = 0;
  debugger->step         = STEP_LINE;
  debugger->current_line = -1;
  debugger->current_file = NULL;
  return debugger;
}

void
Debugger_load_current_file(STATE)
{
  if(DEBUGGER->current_file) {
    bstrListDestroy(DEBUGGER->current_file);
  }

  bstring current_filename = bfromcstr(CURR_FRAME->fn->filename);

  bstring buf = readfile(current_filename);

  struct bstrList *lines = bsplit(buf, '\n');

  if(lines) {
    // Insert line counts
    bstring *l = lines->entry;

    for(int i=1; i <= lines->qty; i++) {
      char linenum[8];
      sprintf(linenum, "%-7i", i);

      binsert(*l, 0, bfromcstr(linenum), ' ');
      l++;
    }

    DEBUGGER->current_file = lines;
  };
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

    switch(getchar()) {
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
        printf("\n\n");
        break;
      case 's':
        DEBUGGER->step = STEP_INS;
        cont = 1;
        break;
      case 't': {
        printf("\nBacktrace");
        printf("\n---------\n");
        CallFrame *p = CURR_FRAME;
        while(p) {
          CallFrame_print(p);
          p = p->parent;
        }
        printf("\n");
        break;
      }
      case 'l': {
        printf("\nLocals");
        printf("\n------");
        for(int i=0; i < DArray_count(CURR_FRAME->locals); i++) {
          printf("\n%i. ", i);
          Value_print((VALUE)DArray_at(CURR_FRAME->locals, i));
          printf("\n");
        }
        break;
      }
      case 'd':
        Stack_print(STACK);
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
    getchar();
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

