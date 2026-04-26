#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../include/AST.H"
#include "../diagnostics/diagnostics.h"

#define SEMANTIC_MAX_VARS 256
#define SEMANTIC_MAX_SCOPES 64

typedef struct {
    char* vars[SEMANTIC_MAX_VARS];
    char* types[SEMANTIC_MAX_VARS];
    int   count;
} scope_T;

typedef struct {
    scope_T  scopes[SEMANTIC_MAX_SCOPES];
    int      depth;
    Diagnostic* diag;
} semantic_T;

semantic_T* init_semantic(Diagnostic* diag);
void        semantic_check(semantic_T* sem, AST_T* programa);

#endif
