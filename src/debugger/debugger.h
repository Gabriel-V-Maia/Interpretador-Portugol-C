#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdio.h>

typedef struct Debugger Debugger;

struct Debugger {
  const char *module;
  int debug;

};

extern Debugger debugger;

void debugger_print(Debugger* self, const char* fmt, ...);
Debugger make_debugger(const char* module, int debug);

#endif
