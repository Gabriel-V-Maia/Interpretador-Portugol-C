#include <stdarg.h>
#include <stdio.h>

#include "debugger.h"

void debugger_print(Debugger* self, const char* fmt, ...)
{
  if (!self->debug)
    return;

  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[%s] ", self->module);

  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

Debugger make_debugger(const char* module, int debug)
{
    return (Debugger){
        .module = module,
        .debug  = debug,
    };
}

Debugger debugger = {0};
