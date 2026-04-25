#include "preprocessor.h"
#include "../include/parser.h"
#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

preprocessor_T* init_preprocessor(Diagnostic* diag, Debugger* debugger)
{
    preprocessor_T* pre = calloc(1, sizeof(preprocessor_T));
    pre->included_count = 0;
    pre->diagnostic     = diag;
    pre->debugger       = debugger;
    return pre;
}

static int already_included(preprocessor_T* pre, const char* path)
{
    for (int i = 0; i < pre->included_count; i++)
        if (strcmp(pre->included[i], path) == 0)
            return 1;
    return 0;
}

static void mark_included(preprocessor_T* pre, const char* path)
{
    if (pre->included_count >= MAX_INCLUDED) return;
    pre->included[pre->included_count++] = strdup(path);
}

static char* resolve_path(const char* import_path)
{
    if (import_path[0] == '.') {
        return strdup(import_path);
    }

    char* full = calloc(strlen(LIBS_PATH) + strlen(import_path) + 1, sizeof(char));
    strcat(full, LIBS_PATH);
    strcat(full, import_path);
    return full;
}

static char* read_file(const char* path)
{
    FILE* f = fopen(path, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char* buf = calloc(size + 1, sizeof(char));
    fread(buf, 1, size, f);
    fclose(f);
    return buf;
}

static AST_T* inject_nodes(AST_T* ast, AST_T* imported, size_t at)
{
    size_t add      = imported->compound_size;
    size_t new_size = ast->compound_size + add - 1;

    AST_T** novo = calloc(new_size, sizeof(AST_T*));

    for (size_t i = 0; i < at; i++)
        novo[i] = ast->compound_value[i];

    for (size_t i = 0; i < add; i++)
        novo[at + i] = imported->compound_value[i];

    for (size_t i = at + 1; i < ast->compound_size; i++)
        novo[add + i - 1] = ast->compound_value[i];

    ast->compound_value = novo;
    ast->compound_size  = new_size;

    return ast;
}

AST_T* preprocessor_run(preprocessor_T* pre, AST_T* ast)
{
    if (!ast) return ast;

    for (size_t i = 0; i < ast->compound_size; i++)
    {
        AST_T* node = ast->compound_value[i];
        if (!node || node->type != AST_IMPORT) continue;

        char* path = resolve_path(node->import_path);

        if (already_included(pre, path)) {
            debugger_print(pre->debugger, "ja incluido: %s, pulando", path);
            free(path);
            continue;
        }

        debugger_print(pre->debugger, "importando: %s", path);

        char* src = read_file(path);
        if (!src) {
            fprintf(stderr, "erro: nao foi possivel abrir '%s'\n", path);
            exit(1);
        }

        mark_included(pre, path);

        lexer_T*  lexer  = init_lexer(src, pre->debugger);
        parser_T* parser = init_parser(lexer, pre->diagnostic, pre->debugger);
        AST_T*    imported = parser_parse(parser);

        imported = preprocessor_run(pre, imported);

        ast = inject_nodes(ast, imported, i);
        i--;
        free(path);
        free(src);
    }

    return ast;
}
