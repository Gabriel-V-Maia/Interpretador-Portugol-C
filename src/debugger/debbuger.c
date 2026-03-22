#include "debugger.h"

void debugger_print(Debugger* self, const char* message)
{
    if (!self->debug) return;
    fprintf(stderr, "[%s] %s\n", self->module, message);
}

Debugger make_debugger(const char* module, int debug)
{
    return (Debugger){
        .module = module,
        .debug  = debug,
        .print  = debugger_print,
    };
}

Debugger debugger = {0};
