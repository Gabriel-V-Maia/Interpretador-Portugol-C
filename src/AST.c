#include "include/AST.H"
#include "include/token.h"
#include <stdio.h>

static const char* binop_op_str(int op)
{
    switch (op) {
    case TOKEN_MAIS:      return "+";
    case TOKEN_MENOS:     return "-";
    case TOKEN_MULT:      return "*";
    case TOKEN_DIV:       return "/";
    case TOKEN_MAIOR:     return ">";
    case TOKEN_MENOR:     return "<";
    case TOKEN_MAIOR_EQ:  return ">=";
    case TOKEN_MENOR_EQ:  return "<=";
    case TOKEN_IGUAL:     return "==";
    case TOKEN_DIFERENTE: return "!=";
    case TOKEN_E:         return "e";
    case TOKEN_OU:        return "ou";
    default:              return "?";
    }
}

AST_T* init_ast(int type)
{
    AST_T* ast = calloc(1, sizeof(AST_T));
    ast->type = type;
    ast->variable_definition_varname = NULL;
    ast->variable_definition_value   = NULL;
    ast->variable_name                = NULL;
    ast->function_call_name           = NULL;
    ast->function_call_arguments      = NULL;
    ast->function_call_arguments_size = 0;
    ast->string_value                 = NULL;
    ast->real_value                   = NULL;
    ast->compound_value               = NULL;
    ast->compound_size                = 0;
    ast->function_def_name            = NULL;
    ast->function_def_body            = NULL;
    return ast;
}

void ast_print(AST_T* ast)
{
    if (!ast) { printf("NULL"); return; }
    switch (ast->type) {
    case AST_VARIABLE_DEFINITION:
        printf("AST_VARIABLE_DEFINITION(type: %s, varname: %s, value: ",
            ast->variable_definition_type,
            ast->variable_definition_varname);
        ast_print(ast->variable_definition_value);
        printf(")");
        break;
    case AST_ASSIGN:
        printf("AST_ASSIGN(varname: %s, value: ", ast->assign_varname);
        ast_print(ast->assign_value);
        printf(")");
        break;
    case AST_VARIABLE:
        printf("AST_VARIABLE(name: %s)", ast->variable_name);
        break;
    case AST_STRING:
        printf("AST_STRING(value: \"%s\")", ast->string_value);
        break;
    case AST_REAL:
        printf("AST_REAL(value: %s)", ast->real_value);
        break;
    case AST_BOOL:
        printf("AST_BOOL(value: %s)", ast->bool_value ? "verdadeiro" : "falso");
        break;
    case AST_FUNCTION_CALL:
        printf("AST_FUNCTION_CALL(name: %s, args: [", ast->function_call_name);
        for (size_t i = 0; i < ast->function_call_arguments_size; i++) {
            ast_print(ast->function_call_arguments[i]);
            if (i + 1 < ast->function_call_arguments_size) printf(", ");
        }
        printf("])");
        break;
    case AST_FUNCTION_DEF:
        printf("AST_FUNCTION_DEF(name: %s, body:\n", ast->function_def_name);
        ast_print(ast->function_def_body);
        printf(")");
        break;
    case AST_COMPOUND:
        printf("AST_COMPOUND([\n");
        for (size_t i = 0; i < ast->compound_size; i++) {
            printf("  ");
            ast_print(ast->compound_value[i]);
            printf("\n");
        }
        printf("])");
        break;
    case AST_INICIO:
        printf("AST_INICIO(\n");
        ast_print(ast->entryBody);
        printf(")");
        break;
    case AST_PROGRAMA:
        printf("AST_PROGRAMA([\n");
        for (size_t i = 0; i < ast->compound_size; i++) {
            printf("  ");
            ast_print(ast->compound_value[i]);
            printf("\n");
        }
        printf("])");
        break;
    case AST_BINOP:
        printf("AST_BINOP(left: ");
        ast_print(ast->binop_left);
        printf(", op: %s, right: ", binop_op_str(ast->binop_op));
        ast_print(ast->binop_right);
        printf(")");
        break;
    case AST_UNOP:
        printf("AST_UNOP(op: nao, operand: ");
        ast_print(ast->unop_operand);
        printf(")");
        break;
    case AST_SE:
        printf("AST_SE(condition: ");
        ast_print(ast->se_condition);
        printf(",\n  then: ");
        ast_print(ast->se_then);
        if (ast->se_else) {
            printf(",\n  else: ");
            ast_print(ast->se_else);
        }
        printf(")");
        break;
    case AST_ENQUANTO:
        printf("AST_ENQUANTO(condition: ");
        ast_print(ast->enquanto_condition);
        printf(",\n  body: ");
        ast_print(ast->enquanto_body);
        printf(")");
        break;
    case AST_PARA:
        printf("AST_PARA(var: %s, from: ", ast->para_var);
        ast_print(ast->para_from);
        printf(", to: ");
        ast_print(ast->para_to);
        printf(",\n  body: ");
        ast_print(ast->para_body);
        printf(")");
        break;
    case AST_REPITA:
        printf("AST_REPITA(body: ");
        ast_print(ast->repita_body);
        printf(",\n  until: ");
        ast_print(ast->repita_condition);
        printf(")");
        break;
    case AST_RETORNE:
        printf("AST_RETORNE(value: ");
        ast_print(ast->retorne_value);
        printf(")");
        break;
    default:
        printf("AST_UNKNOWN(%d)", ast->type);
        break;
    }
}
