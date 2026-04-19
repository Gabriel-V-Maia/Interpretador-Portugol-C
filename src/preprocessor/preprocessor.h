#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "../include/AST.H"
#include "../include/lexer.h"
#include "../diagnostics/diagnostics.h"
#include "../debugger/debugger.h"

#define MAX_INCLUDED 64
#define LIBS_PATH    "libs/"

typedef struct {
    char*      included[MAX_INCLUDED];
    int        included_count;
    Diagnostic* diagnostic;
    Debugger*   debugger;
} preprocessor_T;

preprocessor_T* init_preprocessor(Diagnostic* diag, Debugger* debugger);
AST_T*          preprocessor_run(preprocessor_T* pre, AST_T* ast);

#endif
