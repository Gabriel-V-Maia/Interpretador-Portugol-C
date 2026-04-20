#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/parser.h"
#include "include/AST.H"
#include "include/token.h"
#include "diagnostics/diagnostics.h"
#include "debugger/debugger.h"

typedef AST_T* (*parse_fn)(parser_T*);

typedef struct {
    TokenType token_type;
    parse_fn  fn;
} statement_rule_t;

static AST_T* parser_parse_id(parser_T* parser);
static AST_T* parser_parse_se(parser_T* parser);
static AST_T* parser_parse_enquanto(parser_T* parser);
static AST_T* parser_parse_para(parser_T* parser);
static AST_T* parser_parse_repita(parser_T* parser);
static AST_T* parser_parse_retorne(parser_T* parser);
static AST_T* parser_parse_import(parser_T* parser);

static statement_rule_t statement_rules[] = {
    { TOKEN_ID,       parser_parse_id       },
    { TOKEN_SE,       parser_parse_se       },
    { TOKEN_ENQUANTO, parser_parse_enquanto  },
    { TOKEN_PARA,     parser_parse_para      },
    { TOKEN_REPITA,   parser_parse_repita    },
    { TOKEN_RETORNE,  parser_parse_retorne   },
    { 0, NULL }
};

static const char* typeKeywords[] = { "inteiro", "real", "logico", "cadeia", NULL };

static int isVarType(const char* value)
{
    for (int i = 0; typeKeywords[i] != NULL; i++)
        if (strcmp(typeKeywords[i], value) == 0)
            return 1;
    return 0;
}

parser_T* init_parser(lexer_T* lexer, Diagnostic* diag, Debugger* debugger)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->diagnostic = diag;
    parser->debugger_instance = debugger;
    return parser;
}

void parser_eat(parser_T* parser, TokenType token_type)
{
    if (parser->current_token->type == token_type) {
        parser->current_token = lexer_get_next_token(parser->lexer);
    } else {
        diagnostic_error(parser->diagnostic, parser->current_token,
            "token inesperado `%s` (tipo %d), esperava tipo %d",
            parser->current_token->value,
            parser->current_token->type,
            token_type);
    }
}

static AST_T* parser_parse_import(parser_T* parser)
{
    parser_eat(parser, TOKEN_IMPORTAR);

    AST_T* node = init_ast(AST_IMPORT);
    node->import_path = parser->current_token->value;
    parser_eat(parser, TOKEN_STRING);

    debugger_print(parser->debugger_instance, "importar: %s", node->import_path);
    return node;
}

AST_T* parser_parse_string(parser_T* parser)
{
    if (parser->current_token->type == TOKEN_STRING) {
        AST_T* node = init_ast(AST_STRING);
        node->string_value = parser->current_token->value;
        parser_eat(parser, TOKEN_STRING);
        return node;
    }

    AST_T* node = init_ast(AST_STRING_INTERP);
    node->interp_parts = NULL;
    node->interp_size  = 0;

    while (parser->current_token->type == TOKEN_STRING_PART ||
           parser->current_token->type == TOKEN_INTERP_EXPR)
    {
        AST_T* part;

        if (parser->current_token->type == TOKEN_STRING_PART) {
            part = init_ast(AST_STRING);
            part->string_value = parser->current_token->value;
            parser_eat(parser, TOKEN_STRING_PART);
        } else {
            part = init_ast(AST_VARIABLE);
            part->variable_name = parser->current_token->value;
            parser_eat(parser, TOKEN_INTERP_EXPR);
        }

        node->interp_size++;
        node->interp_parts = realloc(node->interp_parts,
            node->interp_size * sizeof(AST_T*));
        node->interp_parts[node->interp_size - 1] = part;
    }

    return node;
}

AST_T* parser_parse_real(parser_T* parser)
{
    AST_T* node = init_ast(AST_REAL);
    node->real_value = parser->current_token->value;
    parser_eat(parser, TOKEN_REAL);
    return node;
}

AST_T* parser_parse_bool(parser_T* parser)
{
    AST_T* node = init_ast(AST_BOOL);
    node->bool_value = strcmp(parser->current_token->value, "verdadeiro") == 0 ? 1 : 0;
    parser_eat(parser, TOKEN_BOOL);
    return node;
}

AST_T* parser_parse_variable(parser_T* parser)
{
    char* token_value = parser->current_token->value;
    debugger_print(parser->debugger_instance, "parsing %s\n", token_value);
    parser_eat(parser, TOKEN_ID);

    if (parser->current_token->type == TOKEN_LPAREN)
    {
        debugger_print(parser->debugger_instance, "parsing function call, %s\n", token_value);
        parser_eat(parser, TOKEN_LPAREN);

        AST_T** args = NULL;
        size_t argc  = 0;

        while (parser->current_token->type != TOKEN_RPAREN &&
               parser->current_token->type != TOKEN_END)
        {
            AST_T* arg = parser_parse_expr(parser);
            args = realloc(args, sizeof(AST_T*) * (argc + 1));
            args[argc++] = arg;

            if (parser->current_token->type == TOKEN_VIRGULA)
                parser_eat(parser, TOKEN_VIRGULA);
        }

        parser_eat(parser, TOKEN_RPAREN);

        AST_T* node = init_ast(AST_FUNCTION_CALL);
        node->function_call_name           = token_value;
        node->function_call_arguments      = args;
        node->function_call_arguments_size = argc;

        debugger_print(parser->debugger_instance, "Parsing da função %s acabou!", token_value);
        return node;
    }

    AST_T* node = init_ast(AST_VARIABLE);
    node->variable_name = token_value;
    debugger_print(parser->debugger_instance, "done parsing ast_variable, %s\n", token_value);
    return node;
}

AST_T* parser_parse_expr(parser_T* parser);

AST_T* parser_parse_factor(parser_T* parser)
{
    switch (parser->current_token->type)
    {
    case TOKEN_REAL:         return parser_parse_real(parser);
    case TOKEN_STRING:       return parser_parse_string(parser);
    case TOKEN_STRING_PART:  return parser_parse_string(parser);
    case TOKEN_INTERP_EXPR:  return parser_parse_string(parser);
    case TOKEN_BOOL:         return parser_parse_bool(parser);
    case TOKEN_NAO:
    {
        parser_eat(parser, TOKEN_NAO);
        AST_T* node = init_ast(AST_UNOP);
        node->unop_op      = TOKEN_NAO;
        node->unop_operand = parser_parse_factor(parser);
        return node;
    }
    case TOKEN_LPAREN:
    {
        parser_eat(parser, TOKEN_LPAREN);
        AST_T* expr = parser_parse_expr(parser);
        parser_eat(parser, TOKEN_RPAREN);
        return expr;
    }
    case TOKEN_ID: return parser_parse_variable(parser);
    default:
        diagnostic_error(parser->diagnostic, parser->current_token,
            "fator inesperado '%s'", parser->current_token->value);
        exit(1);
    }
}

AST_T* parser_parse_term(parser_T* parser)
{
    AST_T* left = parser_parse_factor(parser);

    while (parser->current_token->type == TOKEN_MULT ||
           parser->current_token->type == TOKEN_DIV)
    {
        TokenType op = parser->current_token->type;
        parser_eat(parser, op);
        AST_T* right = parser_parse_factor(parser);

        AST_T* node  = init_ast(AST_BINOP);
        node->binop_left  = left;
        node->binop_op    = op;
        node->binop_right = right;
        left = node;
    }

    return left;
}

AST_T* parser_parse_arith(parser_T* parser)
{
    AST_T* left = parser_parse_term(parser);

    while (parser->current_token->type == TOKEN_MAIS ||
           parser->current_token->type == TOKEN_MENOS)
    {
        TokenType op = parser->current_token->type;
        parser_eat(parser, op);
        AST_T* right = parser_parse_term(parser);

        AST_T* node  = init_ast(AST_BINOP);
        node->binop_left  = left;
        node->binop_op    = op;
        node->binop_right = right;
        left = node;
    }

    return left;
}

AST_T* parser_parse_comparison(parser_T* parser)
{
    AST_T* left = parser_parse_arith(parser);

    while (parser->current_token->type == TOKEN_MAIOR    ||
           parser->current_token->type == TOKEN_MENOR    ||
           parser->current_token->type == TOKEN_MAIOR_EQ ||
           parser->current_token->type == TOKEN_MENOR_EQ ||
           parser->current_token->type == TOKEN_IGUAL    ||
           parser->current_token->type == TOKEN_DIFERENTE)
    {
        TokenType op = parser->current_token->type;
        parser_eat(parser, op);
        AST_T* right = parser_parse_arith(parser);

        AST_T* node  = init_ast(AST_BINOP);
        node->binop_left  = left;
        node->binop_op    = op;
        node->binop_right = right;
        left = node;
    }

    return left;
}

AST_T* parser_parse_expr(parser_T* parser)
{
    debugger_print(parser->debugger_instance,
        "parse_expr: token type=%d value='%s'",
        parser->current_token->type,
        parser->current_token->value);

    AST_T* left = parser_parse_comparison(parser);

    while (parser->current_token->type == TOKEN_E ||
           parser->current_token->type == TOKEN_OU)
    {
        TokenType op = parser->current_token->type;
        parser_eat(parser, op);
        AST_T* right = parser_parse_comparison(parser);

        AST_T* node  = init_ast(AST_BINOP);
        node->binop_left  = left;
        node->binop_op    = op;
        node->binop_right = right;
        left = node;
    }

    return left;
}

AST_T* parser_parse_variable_definition(parser_T* parser)
{
    char* variable_type = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);

    char* variable_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);

    parser_eat(parser, TOKEN_EQUALS);

    AST_T* value = parser_parse_expr(parser);

    AST_T* node = init_ast(AST_VARIABLE_DEFINITION);
    node->variable_definition_varname = variable_name;
    node->variable_definition_value   = value;
    node->variable_definition_type    = variable_type;

    debugger_print(parser->debugger_instance,
        "variavel definida: '%s' tipo '%s'", variable_name, variable_type);

    return node;
}

static AST_T* parser_parse_id(parser_T* parser)
{
    if (isVarType(parser->current_token->value))
        return parser_parse_variable_definition(parser);

    token_T* id_token = parser->current_token;
    char* name = id_token->value;

    parser_eat(parser, TOKEN_ID);

    if (parser->current_token->type == TOKEN_EQUALS) {
        parser_eat(parser, TOKEN_EQUALS);
        AST_T* value = parser_parse_expr(parser);

        AST_T* node = init_ast(AST_ASSIGN);
        node->assign_varname = name;
        node->assign_value   = value;
        return node;
    }

    if (parser->current_token->type == TOKEN_LPAREN) {
        parser_eat(parser, TOKEN_LPAREN);

        AST_T** args = NULL;
        size_t argc  = 0;

        while (parser->current_token->type != TOKEN_RPAREN &&
               parser->current_token->type != TOKEN_END)
        {
            AST_T* arg = parser_parse_expr(parser);
            args = realloc(args, sizeof(AST_T*) * (argc + 1));
            args[argc++] = arg;

            if (parser->current_token->type == TOKEN_VIRGULA)
                parser_eat(parser, TOKEN_VIRGULA);
        }

        parser_eat(parser, TOKEN_RPAREN);

        AST_T* node = init_ast(AST_FUNCTION_CALL);
        node->function_call_name           = name;
        node->function_call_arguments      = args;
        node->function_call_arguments_size = argc;
        return node;
    }

    diagnostic_error(parser->diagnostic, id_token,
        "identificador '%s' usado como statement inválido", name);
    exit(1);
}

AST_T* parser_parse_statements(parser_T* parser);

static AST_T* parser_parse_block(parser_T* parser)
{
    parser_eat(parser, TOKEN_OPENINGBRACKET);
    AST_T* body = parser_parse_statements(parser);
    parser_eat(parser, TOKEN_CLOSINGBRACKET);
    return body;
}

static AST_T* parser_parse_se(parser_T* parser)
{
    parser_eat(parser, TOKEN_SE);
    parser_eat(parser, TOKEN_LPAREN);
    AST_T* condition = parser_parse_expr(parser);
    parser_eat(parser, TOKEN_RPAREN);

    AST_T* then_body = parser_parse_block(parser);
    AST_T* else_body = NULL;

    if (parser->current_token->type == TOKEN_SENAO) {
        parser_eat(parser, TOKEN_SENAO);
        else_body = parser_parse_block(parser);
    }

    AST_T* node = init_ast(AST_SE);
    node->se_condition = condition;
    node->se_then      = then_body;
    node->se_else      = else_body;

    return node;
}

static AST_T* parser_parse_enquanto(parser_T* parser)
{
    parser_eat(parser, TOKEN_ENQUANTO);
    parser_eat(parser, TOKEN_LPAREN);
    AST_T* condition = parser_parse_expr(parser);
    parser_eat(parser, TOKEN_RPAREN);

    AST_T* body = parser_parse_block(parser);

    AST_T* node = init_ast(AST_ENQUANTO);
    node->enquanto_condition = condition;
    node->enquanto_body      = body;

    return node;
}

static AST_T* parser_parse_para(parser_T* parser)
{
    parser_eat(parser, TOKEN_PARA);

    char* var = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);

    parser_eat(parser, TOKEN_EQUALS);
    AST_T* from = parser_parse_expr(parser);

    parser_eat(parser, TOKEN_ATE);
    AST_T* to = parser_parse_expr(parser);

    AST_T* body = parser_parse_block(parser);

    AST_T* node = init_ast(AST_PARA);
    node->para_var  = var;
    node->para_from = from;
    node->para_to   = to;
    node->para_body = body;

    return node;
}

static AST_T* parser_parse_repita(parser_T* parser)
{
    parser_eat(parser, TOKEN_REPITA);

    AST_T* body = parser_parse_block(parser);

    parser_eat(parser, TOKEN_ATE);
    parser_eat(parser, TOKEN_LPAREN);
    AST_T* condition = parser_parse_expr(parser);
    parser_eat(parser, TOKEN_RPAREN);

    AST_T* node = init_ast(AST_REPITA);
    node->repita_body      = body;
    node->repita_condition = condition;

    return node;
}

static AST_T* parser_parse_retorne(parser_T* parser)
{
    parser_eat(parser, TOKEN_RETORNE);
    AST_T* value = parser_parse_expr(parser);

    AST_T* node = init_ast(AST_RETORNE);
    node->retorne_value = value;

    return node;
}

AST_T* parser_parse_statement(parser_T* parser)
{
    for (int i = 0; statement_rules[i].fn != NULL; i++)
        if (parser->current_token->type == statement_rules[i].token_type)
            return statement_rules[i].fn(parser);

    diagnostic_error(parser->diagnostic, parser->current_token,
        "declaração inesperada '%s'", parser->current_token->value);
    exit(1);
}

AST_T* parser_parse_statements(parser_T* parser)
{
    AST_T* compound = init_ast(AST_COMPOUND);
    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));
    compound->compound_size  = 0;

    while (parser->current_token->type != TOKEN_CLOSINGBRACKET &&
           parser->current_token->type != TOKEN_END)
    {
        AST_T* statement = parser_parse_statement(parser);
        if (!statement) break;

        compound->compound_size++;
        compound->compound_value = realloc(
            compound->compound_value,
            compound->compound_size * sizeof(struct AST_STRUCT*)
        );
        compound->compound_value[compound->compound_size - 1] = statement;
    }

    return compound;
}

AST_T* parser_parse_function_def(parser_T* parser)
{
    parser_eat(parser, TOKEN_FUNC);

    char* name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);

    parser_eat(parser, TOKEN_LPAREN);
    parser_eat(parser, TOKEN_RPAREN);

    AST_T* body = parser_parse_block(parser);

    AST_T* node = init_ast(AST_FUNCTION_DEF);
    node->function_def_name = name;
    node->function_def_body = body;

    debugger_print(parser->debugger_instance, "Parseando funcao: %s", name);
    return node;
}

AST_T* parser_parse_programa(parser_T* parser)
{
    parser_eat(parser, TOKEN_PROGRAMA);
    parser_eat(parser, TOKEN_OPENINGBRACKET);

    AST_T* programa = init_ast(AST_PROGRAMA);
    programa->compound_value = calloc(1, sizeof(AST_T*));
    programa->compound_size  = 0;

    while (parser->current_token->type != TOKEN_CLOSINGBRACKET &&
           parser->current_token->type != TOKEN_END)
    {
        AST_T* node = NULL;

        if (parser->current_token->type == TOKEN_IMPORTAR)
            node = parser_parse_import(parser);
        else if (parser->current_token->type == TOKEN_FUNC)
            node = parser_parse_function_def(parser);
        else {
            diagnostic_error(parser->diagnostic, parser->current_token,
                "esperava 'importar' ou 'funcao', encontrado '%s'",
                parser->current_token->value);
            exit(1);
        }

        programa->compound_size++;
        programa->compound_value = realloc(
            programa->compound_value,
            programa->compound_size * sizeof(AST_T*)
        );
        programa->compound_value[programa->compound_size - 1] = node;
    }

    parser_eat(parser, TOKEN_CLOSINGBRACKET);
    return programa;
}

AST_T* parser_parse(parser_T* parser)
{
    if (strcmp(parser->current_token->value, "programa") == 0)
        return parser_parse_programa(parser);

    diagnostic_error(parser->diagnostic, parser->current_token,
        "Programa deve começar com 'programa'");
    exit(1);
}
