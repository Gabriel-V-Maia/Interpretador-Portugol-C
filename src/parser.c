#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/parser.h"
#include "include/token.h"
#include "diagnostics/diagnostics.h"

#include "debugger/debugger.h"



// TODO LIST
// - Fazer funções incompletas

parser_T* init_parser(lexer_T* lexer, Diagnostic* diag, Debugger *debugger)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->diagnostic = diag;
    parser->debugger_instance = debugger;
    
    return parser;
}

AST_T* parser_parse_real(parser_T* parser)
{
  AST_T* ast_real = init_ast(AST_REAL);

  ast_real->real_value = parser->current_token->value;
  parser_eat(parser, TOKEN_REAL);

  return ast_real;
}

AST_T* parser_parse_variable_definition(parser_T* parser)
{
  // tipo nome = valor 
  // inteiro numero = 10

  // pegamos o TIPO da variavel 
  char* variable_type = parser->current_token->value;
  parser_eat(parser, TOKEN_ID);

  // comemos o NOME IDENTIFICADOR da variavel 
  char* variable_name = parser->current_token->value; 
  parser_eat(parser, TOKEN_ID);

  // come o "="
  parser_eat(parser, TOKEN_EQUALS);

  debugger_print(parser->debugger_instance, "Parseando o valor do tipo -> %d", parser->current_token->type);
  
  // parsea o valor 
  AST_T* variable_value = parser_parse_expr(parser);
   
  // Declaramos a definição da variavel e então retornamos ela 
  
  AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);
  variable_definition->variable_definition_varname = variable_name;
  variable_definition->variable_definition_value = variable_value;


  debugger_print(parser->debugger_instance,
                 "variavel definida como: name: '%s', value: '%s'",
                 variable_definition->variable_definition_varname,
                 variable_definition->variable_definition_value);

  
  return variable_definition;
}


// TODO MELHORAR ISSO
int isReserved(const char* value)
{
    
    static const char* reservedTypes[] = {"inteiro", "real", "logico", "cadeia"};

    int count = sizeof(reservedTypes) / sizeof(reservedTypes[0]);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(value, reservedTypes[i]) == 0)
            return 1;
    }
    return 0;
}

AST_T* parser_parse_id(parser_T* parser)
{

  /*
  if (strcmp(parser->current_token->value, "inteiro") == 0)
  {
    return parser_parse_variable_definition(parser);
  } else {
    return parser_parse_variable(parser);
  }
  */ 

  if (isReserved(parser->current_token->value)) {
    debugger_print(parser->debugger_instance, "parseando def variavel %s (reservado)\n", parser->current_token->value);
    return parser_parse_variable_definition(parser);
  } else { 
    debugger_print(parser->debugger_instance,"parseando variavel nao reservada %s\n", parser->current_token->value);
    return parser_parse_variable(parser);
  }
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

// Função para pegar a "funcao inicio()"
// TODO ainda
AST_T* parser_parse_entrypoint(parser_T* parser)
{
    parser_eat(parser, TOKEN_FUNC); // "funcao"

    // Come o nome da função: "inicio"
    char* function_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);

    if (strcmp(function_name, "inicio") != 0)
    {
        printf("Erro: nome da função principal deve ser 'inicio', encontrado '%s'\n", function_name);
        exit(1);
    }

    parser_eat(parser, TOKEN_LPAREN);
    parser_eat(parser, TOKEN_RPAREN);

    parser_eat(parser, TOKEN_OPENINGBRACKET); // "{"

    AST_T* entryPoint_Body = parser_parse_statements(parser);

    parser_eat(parser, TOKEN_CLOSINGBRACKET); // "}"

    AST_T* entrypoint_node = init_ast(AST_INICIO);
    entrypoint_node->entryBody = entryPoint_Body;

    debugger_print(parser->debugger_instance, "Retornando node entrypoint");
    
    return entrypoint_node;
}

AST_T* parser_parse_programa(parser_T* parser)
{
    parser_eat(parser, TOKEN_PROGRAMA); // "programa"
    parser_eat(parser, TOKEN_OPENINGBRACKET); // "{"

    parser_parse_statements(parser);

    parser_eat(parser, TOKEN_CLOSINGBRACKET); // "}"

    return parser_parse_entrypoint(parser);
}

AST_T* parser_parse(parser_T* parser)
{
    if (strcmp(parser->current_token->value, "programa") == 0)
        return parser_parse_programa(parser);

    diagnostic_error(parser->diagnostic, parser->current_token, "Programa deve começar com 'programa'");
    exit(1);
}


AST_T* parser_parse_statement(parser_T* parser)
{
  switch (parser->current_token->type) 
  {
    case TOKEN_ID: return parser_parse_id(parser);
    case TOKEN_FUNC: return parser_parse_entrypoint(parser);
    default:
      diagnostic_error(parser->diagnostic, parser->current_token, "Declaração inesperada com '%s'", parser->current_token->value);
      exit(1);

  }
}


AST_T* parser_parse_statements(parser_T* parser)
{
    AST_T* compound = init_ast(AST_COMPOUND);
    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));
    compound->compound_size = 0;

    while (
        parser->current_token->type != TOKEN_CLOSINGBRACKET &&
        parser->current_token->type != TOKEN_END
    )
    {
        AST_T* statement = parser_parse_statement(parser);
        if (!statement)
            break;

        compound->compound_size += 1;
        compound->compound_value = realloc(
            compound->compound_value,
            compound->compound_size * sizeof(struct AST_STRUCT*)
        );

        compound->compound_value[compound->compound_size - 1] = statement;
    }

    return compound;
}

AST_T* parser_parse_expr(parser_T* parser)
{
  switch(parser->current_token->type)
  {
    case TOKEN_STRING: return parser_parse_string(parser);
    case TOKEN_REAL: return parser_parse_real(parser);
   }
  
  diagnostic_error(parser->diagnostic, parser->current_token,
                   "expressão inexperada %s");
}

AST_T* parser_parse_factor(parser_T* parser)
{

}
AST_T* parser_parse_term(parser_T* parser)
{

};


// TODO ESSA BOMBA A
AST_T* parser_parse_function_call(parser_T* parser)
{
    char* function_name = parser->current_token->value;
    printf("[parser_parse_function_call] parsing function %s\n", function_name);
    parser_eat(parser, TOKEN_ID);
    parser_eat(parser, TOKEN_LPAREN);

    AST_T** function_arguments = NULL;
    size_t argc = 0;

    if (parser->current_token->type != TOKEN_RPAREN)
    {
        while (parser->current_token->type != TOKEN_RPAREN)
        {
            char* arg_value = parser->current_token->value;
            AST_T* arg_ast = init_ast(AST_VARIABLE);
            arg_ast->variable_name = arg_value;

            function_arguments = realloc(function_arguments, sizeof(AST_T*) * (argc + 1));
            function_arguments[argc++] = arg_ast;

            parser_eat(parser, TOKEN_STRING);

            if (parser->current_token->type != TOKEN_RPAREN) {
              diagnostic_error(parser->diagnostic, parser->current_token,"Erro de sintaxe");
              exit(1); 
            }
        }
    }

    parser_eat(parser, TOKEN_RPAREN);

    AST_T* ast_function = init_ast(AST_FUNCTION_CALL);
    ast_function->function_call_name = function_name;
    ast_function->function_call_arguments = function_arguments;
    ast_function->function_call_arguments_size = argc;

    debugger_print(parser->debugger_instance, "Parsing da função %s acabou!", function_name);

    return ast_function;
}

AST_T* parser_parse_variable(parser_T* parser)
{
  char* token_value = parser->current_token->value;

  debugger_print(parser->debugger_instance,"parsing %s\n", parser->current_token->value);

  parser_eat(parser, TOKEN_ID);
  
  // caso o nosso token for um parenteses, vamos parsear como função
  if (parser->current_token->type == TOKEN_RPAREN || strcmp(parser->current_token->value, "inicio") == 0)
  {
    debugger_print(parser->debugger_instance, "parsing function, %s\n", parser->current_token->value);
    return parser_parse_function_call(parser);
  }
  
  AST_T* ast_variable = init_ast(AST_VARIABLE);
  ast_variable->variable_name = token_value;

  parser_eat(parser, TOKEN_ID);
  debugger_print(parser->debugger_instance, "done parsing ast_variable, %s\n", ast_variable->variable_name);
 
  return ast_variable;
}


AST_T* parser_parse_string(parser_T* parser)
{
  AST_T* ast_string = init_ast(AST_STRING);
  ast_string->string_value = parser->current_token->value;

  parser_eat(parser, TOKEN_STRING);
  return ast_string;
}

