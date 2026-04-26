#include "semantic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

semantic_T* init_semantic(Diagnostic* diag)
{
    semantic_T* sem = calloc(1, sizeof(semantic_T));
    sem->depth = 0;
    sem->diag  = diag;
    return sem;
}

static void scope_declare(semantic_T* sem, const char* name, const char* type)
{
    scope_T* s = &sem->scopes[sem->depth];
    if (s->count >= SEMANTIC_MAX_VARS) return;
    s->vars[s->count]  = strdup(name);
    s->types[s->count] = strdup(type ? type : "nulo");
    s->count++;
}

static int scope_has(semantic_T* sem, const char* name)
{
    for (int d = sem->depth; d >= 0; d--) {
        scope_T* s = &sem->scopes[d];
        for (int i = 0; i < s->count; i++)
            if (strcmp(s->vars[i], name) == 0) return 1;
    }
    return 0;
}

static void scope_push(semantic_T* sem)
{
    sem->depth++;
    sem->scopes[sem->depth].count = 0;
}

static void scope_pop(semantic_T* sem)
{
    sem->scopes[sem->depth].count = 0;
    sem->depth--;
}

static token_T make_fake_token(AST_T* node, const char* value)
{
    token_T tok;
    tok.line   = node->line;
    tok.column = node->column;
    tok.value  = (char*)value;
    tok.type   = TOKEN_ID;
    return tok;
}

static void check_node(semantic_T* sem, AST_T* node)
{
    if (!node) return;

    switch (node->type) {
    case AST_VARIABLE_DEFINITION:
        check_node(sem, node->variable_definition_value);
        scope_declare(sem, node->variable_definition_varname, node->variable_definition_type);
        break;

    case AST_ASSIGN: {
        if (!scope_has(sem, node->assign_varname)) {
            token_T tok = make_fake_token(node, node->assign_varname);
            diagnostic_error(sem->diag, &tok,
                "variavel '%s' nao foi declarada", node->assign_varname);
        }
        check_node(sem, node->assign_value);
        break;
    }

    case AST_VARIABLE: {
        if (!scope_has(sem, node->variable_name)) {
            token_T tok = make_fake_token(node, node->variable_name);
            diagnostic_error(sem->diag, &tok,
                "variavel '%s' nao foi declarada", node->variable_name);
        }
        break;
    }

    case AST_STRING_INTERP:
        for (size_t i = 0; i < node->interp_size; i++)
            check_node(sem, node->interp_parts[i]);
        break;

    case AST_COMPOUND:
        scope_push(sem);
        for (size_t i = 0; i < node->compound_size; i++)
            check_node(sem, node->compound_value[i]);
        scope_pop(sem);
        break;

    case AST_SE:
        check_node(sem, node->se_condition);
        check_node(sem, node->se_then);
        check_node(sem, node->se_else);
        break;

    case AST_ENQUANTO:
        check_node(sem, node->enquanto_condition);
        check_node(sem, node->enquanto_body);
        break;

    case AST_PARA:
        scope_push(sem);
        scope_declare(sem, node->para_var, "inteiro");
        check_node(sem, node->para_from);
        check_node(sem, node->para_to);
        check_node(sem, node->para_body);
        scope_pop(sem);
        break;

    case AST_REPITA:
        check_node(sem, node->repita_body);
        check_node(sem, node->repita_condition);
        break;

    case AST_FUNCTION_CALL:
        for (size_t i = 0; i < node->function_call_arguments_size; i++)
            check_node(sem, node->function_call_arguments[i]);
        break;

    case AST_RETORNE:
        check_node(sem, node->retorne_value);
        break;

    case AST_BINOP:
        check_node(sem, node->binop_left);
        check_node(sem, node->binop_right);
        break;

    case AST_UNOP:
        check_node(sem, node->unop_operand);
        break;

    default: break;
    }
}

void semantic_check(semantic_T* sem, AST_T* programa)
{
    for (size_t i = 0; i < programa->compound_size; i++) {
        AST_T* fn = programa->compound_value[i];
        if (fn->type != AST_FUNCTION_DEF) continue;

        sem->depth = 0;
        sem->scopes[0].count = 0;

        for (size_t j = 0; j < fn->function_def_param_count; j++)
            scope_declare(sem, fn->function_def_param_names[j], fn->function_def_param_types[j]);

        check_node(sem, fn->function_def_body);
    }
}
