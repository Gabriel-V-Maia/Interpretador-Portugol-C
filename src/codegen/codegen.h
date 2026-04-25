#ifndef CODEGEN_H
#define CODEGEN_H

#include "../include/AST.H"
#include "../debugger/debugger.h"
#include <stdio.h>

#define CODEGEN_MAX_VARS 256

typedef struct {
    char* name;
    char* type;
} var_type_t;

typedef struct {
    FILE*      output;
    Debugger*  debugger;
    var_type_t vars[CODEGEN_MAX_VARS];
    int        var_count;
} codegen_T;

typedef void (*emit_fn)(codegen_T*, AST_T*);

codegen_T* init_codegen(const char* output_path, Debugger* debugger);
void       codegen_emit(codegen_T* cg, AST_T* ast);
void       codegen_compile(const char* c_file, const char* out_file);
void       start_builtins();
#endif
