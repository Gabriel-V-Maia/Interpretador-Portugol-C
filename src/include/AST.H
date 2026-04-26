#ifndef AST_H
#define AST_H
#include <stdlib.h>

typedef struct AST_STRUCT
{
    enum {
        AST_FUNCTION_CALL,
        AST_VARIABLE_DEFINITION,
        AST_VARIABLE,
        AST_STRING,
        AST_REAL,
        AST_COMPOUND,
        AST_PROGRAMA,
        AST_INICIO,
        AST_FUNCTION_DEF,
        AST_BINOP,
        AST_UNOP,
        AST_SE,
        AST_ENQUANTO,
        AST_PARA,
        AST_REPITA,
        AST_RETORNE,
        AST_BOOL,
        AST_ASSIGN,
        AST_IMPORT,
        AST_STRING_INTERP
    } type;

    char*               variable_definition_varname;
    struct AST_STRUCT*  variable_definition_value;
    char*               variable_definition_type;

    char*               variable_name;

    char*               function_call_name;
    struct AST_STRUCT** function_call_arguments;
    size_t              function_call_arguments_size;

    char*               string_value;
    char*               real_value;
    int                 bool_value;

    struct AST_STRUCT** compound_value;
    size_t              compound_size;

    struct AST_STRUCT*  body;
    struct AST_STRUCT*  entryBody;

    char*               function_def_name;
    char*               function_def_return_type;
    struct AST_STRUCT*  function_def_body;
    char**              function_def_param_names;
    char**              function_def_param_types;
    size_t              function_def_param_count;

    struct AST_STRUCT*  binop_left;
    int                 binop_op;
    struct AST_STRUCT*  binop_right;

    int                 unop_op;
    struct AST_STRUCT*  unop_operand;

    struct AST_STRUCT*  se_condition;
    struct AST_STRUCT*  se_then;
    struct AST_STRUCT*  se_else;

    struct AST_STRUCT*  enquanto_condition;
    struct AST_STRUCT*  enquanto_body;

    char*               para_var;
    struct AST_STRUCT*  para_from;
    struct AST_STRUCT*  para_to;
    struct AST_STRUCT*  para_body;

    struct AST_STRUCT*  repita_body;
    struct AST_STRUCT*  repita_condition;

    struct AST_STRUCT*  retorne_value;

    char*               assign_varname;
    struct AST_STRUCT*  assign_value;

    char*               import_path;

    struct AST_STRUCT** interp_parts;
    size_t              interp_size;

    int line;
    int column;
} AST_T;

AST_T* init_ast(int type);
void   ast_print(AST_T* ast);

#endif
