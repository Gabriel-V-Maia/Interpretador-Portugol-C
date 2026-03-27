#include <stdlib.h>

#include "lexer.h"
#include "AST.H"
#include "token.h"

#include "../diagnostics/diagnostics.h"
#include "../debugger/debugger.h"

#ifndef PARSER_H
#define PARSER_H

typedef struct PARSER_STRUCT
{
  lexer_T* lexer;
  token_T *current_token;
  Diagnostic *diagnostic;
  Debugger *debugger_instance;
} parser_T;

parser_T* init_parser(lexer_T* lexer, Diagnostic* diag, Debugger* debugger);

void parser_eat(parser_T* parser, TokenType token_type);

AST_T* parser_parse(parser_T* parser);

AST_T* parser_parse_statement(parser_T* parser);

AST_T* parser_parse_id(parser_T* parser);

AST_T* parser_parse_statements(parser_T* parser);

AST_T* parser_parse_expr(parser_T* parser);

AST_T* parser_parse_factor(parser_T* parser);

AST_T* parser_parse_term(parser_T* parser);

AST_T* parser_parse_function_call(parser_T* parser);

AST_T* parser_parse_variable(parser_T* parser);

AST_T* parser_parse_variable_definition(parser_T* parser);

AST_T* parser_parse_string(parser_T* parser);

#endif

