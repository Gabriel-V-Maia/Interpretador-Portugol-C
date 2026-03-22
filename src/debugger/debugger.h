#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdio.h>

typedef struct Debugger Debugger;

struct Debugger {
  const char *module;
  int debug;

  void (*print)(Debugger *self, const char *message);
};

extern Debugger debugger;

void debugger_print(Debugger* self, const char* message);
Debugger make_debugger(const char* module, int debug);

#endif
