#include "codegen.h"
#include "../include/token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void emit_programa(codegen_T* cg, AST_T* ast);
static void emit_function_def(codegen_T* cg, AST_T* ast);
static void emit_compound(codegen_T* cg, AST_T* ast);
static void emit_variable_definition(codegen_T* cg, AST_T* ast);
static void emit_assign(codegen_T* cg, AST_T* ast);
static void emit_function_call(codegen_T* cg, AST_T* ast);
static void emit_string_interp(codegen_T* cg, AST_T* ast);
static void emit_se(codegen_T* cg, AST_T* ast);
static void emit_enquanto(codegen_T* cg, AST_T* ast);
static void emit_para(codegen_T* cg, AST_T* ast);
static void emit_repita(codegen_T* cg, AST_T* ast);
static void emit_retorne(codegen_T* cg, AST_T* ast);
static void emit_binop(codegen_T* cg, AST_T* ast);
static void emit_unop(codegen_T* cg, AST_T* ast);
static void emit_string(codegen_T* cg, AST_T* ast);
static void emit_real(codegen_T* cg, AST_T* ast);
static void emit_bool(codegen_T* cg, AST_T* ast);
static void emit_variable(codegen_T* cg, AST_T* ast);

typedef struct {
    int     ast_type;
    emit_fn fn;
} emit_rule_t;

static emit_rule_t emit_rules[] = {
    { AST_PROGRAMA,            emit_programa            },
    { AST_FUNCTION_DEF,        emit_function_def        },
    { AST_COMPOUND,            emit_compound            },
    { AST_VARIABLE_DEFINITION, emit_variable_definition },
    { AST_ASSIGN,              emit_assign              },
    { AST_FUNCTION_CALL,       emit_function_call       },
    { AST_SE,                  emit_se                  },
    { AST_ENQUANTO,            emit_enquanto             },
    { AST_PARA,                emit_para                },
    { AST_REPITA,              emit_repita              },
    { AST_RETORNE,             emit_retorne             },
    { AST_BINOP,               emit_binop               },
    { AST_UNOP,                emit_unop                },
    { AST_STRING,              emit_string              },
    { AST_STRING_INTERP,       emit_string_interp       },
    { AST_REAL,                emit_real                },
    { AST_BOOL,                emit_bool                },
    { AST_VARIABLE,            emit_variable            },
    { -1, NULL }
};

static void codegen_register_var(codegen_T* cg, const char* name, const char* type)
{
    if (cg->var_count >= CODEGEN_MAX_VARS) return;
    cg->vars[cg->var_count].name = (char*)name;
    cg->vars[cg->var_count].type = (char*)type;
    cg->var_count++;
}

static const char* codegen_lookup_type(codegen_T* cg, const char* name)
{
    for (int i = 0; i < cg->var_count; i++)
        if (strcmp(cg->vars[i].name, name) == 0)
            return cg->vars[i].type;
    return "cadeia";
}

static const char* type_to_format(const char* type)
{
    if (strcmp(type, "inteiro") == 0) return "%d";
    if (strcmp(type, "real")    == 0) return "%f";
    if (strcmp(type, "logico")  == 0) return "%d";
    return "%s";
}

static const char* portugol_type_to_c(const char* type)
{
    if (!type)                        return "void";
    if (strcmp(type, "inteiro") == 0) return "int";
    if (strcmp(type, "real")    == 0) return "double";
    if (strcmp(type, "logico")  == 0) return "int";
    if (strcmp(type, "cadeia")  == 0) return "char*";
    if (strcmp(type, "nulo")    == 0) return "void";
    return "void";
}

static const char* binop_str(int op)
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
    case TOKEN_E:         return "&&";
    case TOKEN_OU:        return "||";
    default:              return "?";
    }
}

static const char* resolve_builtin(const char* name)
{
    if (strcmp(name, "escreva") == 0) return "printf";
    if (strcmp(name, "leia")    == 0) return "scanf";
    if (strcmp(name, "sair")    == 0) return "exit";
    return name;
}

codegen_T* init_codegen(const char* output_path, Debugger* debugger)
{
    codegen_T* cg = calloc(1, sizeof(codegen_T));
    cg->output    = fopen(output_path, "w");
    cg->debugger  = debugger;
    cg->var_count = 0;

    if (!cg->output) {
        fprintf(stderr, "erro: nao foi possivel criar '%s'\n", output_path);
        exit(1);
    }

    return cg;
}

void codegen_emit(codegen_T* cg, AST_T* ast)
{
    if (!ast) return;

    for (int i = 0; emit_rules[i].fn != NULL; i++) {
        if (emit_rules[i].ast_type == (int)ast->type) {
            emit_rules[i].fn(cg, ast);
            return;
        }
    }

    fprintf(stderr, "codegen: tipo desconhecido %d\n", ast->type);
}

static void emit_func_signature(codegen_T* cg, AST_T* ast)
{
    const char* rtype = portugol_type_to_c(ast->function_def_return_type);
    fprintf(cg->output, "%s %s(", rtype, ast->function_def_name);

    for (size_t i = 0; i < ast->function_def_param_count; i++) {
        fprintf(cg->output, "%s %s",
            portugol_type_to_c(ast->function_def_param_types[i]),
            ast->function_def_param_names[i]);
        if (i + 1 < ast->function_def_param_count)
            fprintf(cg->output, ", ");
    }

    if (ast->function_def_param_count == 0)
        fprintf(cg->output, "void");

    fprintf(cg->output, ")");
}

static void emit_forward_decl(codegen_T* cg, AST_T* ast)
{
    if (strcmp(ast->function_def_name, "inicio") == 0) return;
    emit_func_signature(cg, ast);
    fprintf(cg->output, ";\n");
}

static void emit_programa(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "#include <stdio.h>\n");
    fprintf(cg->output, "#include <stdlib.h>\n\n");

    for (size_t i = 0; i < ast->compound_size; i++) {
        AST_T* node = ast->compound_value[i];
        if (node->type == AST_FUNCTION_DEF)
            emit_forward_decl(cg, node);
    }

    fprintf(cg->output, "\n");

    for (size_t i = 0; i < ast->compound_size; i++)
        codegen_emit(cg, ast->compound_value[i]);
}

static void emit_function_def(codegen_T* cg, AST_T* ast)
{
    if (strcmp(ast->function_def_name, "inicio") == 0) {
        fprintf(cg->output, "int main(void) {\n");
    } else {
        emit_func_signature(cg, ast);
        fprintf(cg->output, " {\n");

        for (size_t i = 0; i < ast->function_def_param_count; i++)
            codegen_register_var(cg, ast->function_def_param_names[i],
                                     ast->function_def_param_types[i]);
    }

    codegen_emit(cg, ast->function_def_body);

    if (strcmp(ast->function_def_name, "inicio") == 0)
        fprintf(cg->output, "return 0;\n");

    fprintf(cg->output, "}\n\n");
}

static void emit_compound(codegen_T* cg, AST_T* ast)
{
    for (size_t i = 0; i < ast->compound_size; i++)
        codegen_emit(cg, ast->compound_value[i]);
}

static void emit_variable_definition(codegen_T* cg, AST_T* ast)
{
    const char* ctype = portugol_type_to_c(ast->variable_definition_type);
    codegen_register_var(cg, ast->variable_definition_varname, ast->variable_definition_type);
    fprintf(cg->output, "%s %s = ", ctype, ast->variable_definition_varname);
    codegen_emit(cg, ast->variable_definition_value);
    fprintf(cg->output, ";\n");
}

static void emit_assign(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "%s = ", ast->assign_varname);
    codegen_emit(cg, ast->assign_value);
    fprintf(cg->output, ";\n");
}
static void emit_string_interp(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "printf(\"");

    for (size_t i = 0; i < ast->interp_size; i++) {
        AST_T* part = ast->interp_parts[i];
        if (part->type == AST_STRING)
            fprintf(cg->output, "%s", part->string_value);
        else
            fprintf(cg->output, "%%d");
    }

    fprintf(cg->output, "\"");

    for (size_t i = 0; i < ast->interp_size; i++) {
        AST_T* part = ast->interp_parts[i];
        if (part->type == AST_STRING) continue;

        fprintf(cg->output, ", ");

        if (part->type != AST_FUNCTION_CALL) {
            codegen_emit(cg, part);
            continue;
        }

        fprintf(cg->output, "%s(", part->function_call_name);
        for (size_t j = 0; j < part->function_call_arguments_size; j++) {
            codegen_emit(cg, part->function_call_arguments[j]);
            if (j + 1 < part->function_call_arguments_size)
                fprintf(cg->output, ", ");
        }
        fprintf(cg->output, ")");
    }

    fprintf(cg->output, ");\n");
}

static void emit_function_call(codegen_T* cg, AST_T* ast)
{
    if (strcmp(ast->function_call_name, "escreva") == 0 &&
        ast->function_call_arguments_size == 1 &&
        ast->function_call_arguments[0]->type == AST_STRING_INTERP)
    {
        emit_string_interp(cg, ast->function_call_arguments[0]);
        return;
    }

    const char* name = resolve_builtin(ast->function_call_name);

    fprintf(cg->output, "%s(", name);

    for (size_t i = 0; i < ast->function_call_arguments_size; i++) {
        codegen_emit(cg, ast->function_call_arguments[i]);
        if (i + 1 < ast->function_call_arguments_size)
            fprintf(cg->output, ", ");

    }
    fprintf(cg->output, ");\n");
}

static void emit_se(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "if (");
    codegen_emit(cg, ast->se_condition);
    fprintf(cg->output, ") {\n");
    codegen_emit(cg, ast->se_then);
    fprintf(cg->output, "}");

    if (ast->se_else) {
        fprintf(cg->output, " else {\n");
        codegen_emit(cg, ast->se_else);
        fprintf(cg->output, "}");
    }

    fprintf(cg->output, "\n");
}

static void emit_enquanto(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "while (");
    codegen_emit(cg, ast->enquanto_condition);
    fprintf(cg->output, ") {\n");
    codegen_emit(cg, ast->enquanto_body);
    fprintf(cg->output, "}\n");
}

static void emit_para(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "for (int %s = ", ast->para_var);
    codegen_emit(cg, ast->para_from);
    fprintf(cg->output, "; %s < ", ast->para_var);
    codegen_emit(cg, ast->para_to);
    fprintf(cg->output, "; %s++) {\n", ast->para_var);
    codegen_emit(cg, ast->para_body);
    fprintf(cg->output, "}\n");
}

static void emit_repita(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "do {\n");
    codegen_emit(cg, ast->repita_body);
    fprintf(cg->output, "} while (!(");
    codegen_emit(cg, ast->repita_condition);
    fprintf(cg->output, "));\n");
}

static void emit_retorne(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "return ");
    codegen_emit(cg, ast->retorne_value);
    fprintf(cg->output, ";\n");
}

static void emit_binop(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "(");
    codegen_emit(cg, ast->binop_left);
    fprintf(cg->output, " %s ", binop_str(ast->binop_op));
    codegen_emit(cg, ast->binop_right);
    fprintf(cg->output, ")");
}

static void emit_unop(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "!(");
    codegen_emit(cg, ast->unop_operand);
    fprintf(cg->output, ")");
}

static void emit_string(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "\"%s\"", ast->string_value);
}

static void emit_real(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "%s", ast->real_value);
}

static void emit_bool(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "%d", ast->bool_value);
}

static void emit_variable(codegen_T* cg, AST_T* ast)
{
    fprintf(cg->output, "%s", ast->variable_name);
}

void codegen_compile(const char* c_file, const char* out_file)
{
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "gcc %s -o %s", c_file, out_file);
    int ret = system(cmd);
    if (ret != 0)
        fprintf(stderr, "erro: gcc falhou com codigo %d\n", ret);
}
