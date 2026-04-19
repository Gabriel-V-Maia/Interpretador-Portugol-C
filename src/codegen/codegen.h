#ifndef CODEGEN_H
#define CODEGEN_H

#include "../include/AST.H"
#include "../debugger/debugger.h"
#include <stdio.h>

typedef struct {
    FILE*     output;
    Debugger* debugger;
} codegen_T;

typedef void (*emit_fn)(codegen_T*, AST_T*);

codegen_T* init_codegen(const char* output_path, Debugger* debugger);
void       codegen_emit(codegen_T* cg, AST_T* ast);
void       codegen_compile(const char* c_file, const char* out_file);

#endif
