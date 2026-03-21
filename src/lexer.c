#include "include/lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

lexer_T* init_lexer(char* contents)
{
    lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->contents      = contents;
    lexer->contents_len  = strlen(contents); // calcula uma vez só
    lexer->index         = 0;
    lexer->currentChar   = contents[0];
    lexer->line          = 1;
    lexer->column        = 1;
    return lexer;
}

void lexer_advance(lexer_T* lexer)
{
    if (lexer->currentChar != '\0' && lexer->index < lexer->contents_len)
    {
        if (lexer->currentChar == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }

        lexer->index++;
        lexer->currentChar = lexer->contents[lexer->index];
    }
}

void lexer_skip_whitespace(lexer_T* lexer)
{
    while (lexer->currentChar == ' ' || lexer->currentChar == '\n')
        lexer_advance(lexer);
}

token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token)
{
    lexer_advance(lexer);
    return token;
}

char* lexer_get_current_char_as_string(lexer_T* lexer)
{
    char* str = calloc(2, sizeof(char));
    str[0] = lexer->currentChar;
    str[1] = '\0';
    return str;
}

// helper: estampa linha/coluna no token antes de retornar
static token_T* stamp(lexer_T* lexer, token_T* token, int line, int col)
{
    token->line   = line;
    token->column = col;
    return token;
}

token_T* lexer_collect_string(lexer_T* lexer)
{
    int start_line = lexer->line;
    int start_col  = lexer->column;

    lexer_advance(lexer); // pula a aspa de abertura
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';

    while (lexer->currentChar != '"' && lexer->currentChar != '\0')
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);
        free(s);
        lexer_advance(lexer);
    }

    lexer_advance(lexer); // pula a aspa de fechamento
    return stamp(lexer, init_token(TOKEN_STRING, value), start_line, start_col);
}

token_T* lexer_collect_id(lexer_T* lexer)
{
    int start_line = lexer->line;
    int start_col  = lexer->column;

    char* value = calloc(1, sizeof(char));
    value[0] = '\0';

    while (isalnum(lexer->currentChar))
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);
        free(s);
        lexer_advance(lexer);
    }

    token_T* token;
    if      (strcmp(value, "funcao")   == 0) token = init_token(TOKEN_FUNC,     value);
    else if (strcmp(value, "programa") == 0) token = init_token(TOKEN_PROGRAMA, value);
    else                                     token = init_token(TOKEN_ID,        value);

    printf("[lexer_collect_id] Criado token %s (type=%d)\n", token->value, token->type);
    return stamp(lexer, token, start_line, start_col);
}

token_T* lexer_get_next_token(lexer_T* lexer)
{
    while (lexer->currentChar != '\0' && lexer->index < lexer->contents_len)
    {
        if (isspace(lexer->currentChar)) {
            lexer_skip_whitespace(lexer);
            continue;
        }

        int line = lexer->line;
        int col  = lexer->column;

        if (isalnum(lexer->currentChar))
            return lexer_collect_id(lexer);

        if (lexer->currentChar == '"')
            return lexer_collect_string(lexer);

        switch (lexer->currentChar)
        {
            case '=': return stamp(lexer, lexer_advance_with_token(lexer, init_token(TOKEN_EQUALS,         lexer_get_current_char_as_string(lexer))), line, col);
            case '{': return stamp(lexer, lexer_advance_with_token(lexer, init_token(TOKEN_OPENINGBRACKET, lexer_get_current_char_as_string(lexer))), line, col);
            case '}': return stamp(lexer, lexer_advance_with_token(lexer, init_token(TOKEN_CLOSINGBRACKET, lexer_get_current_char_as_string(lexer))), line, col);
            case '(': return stamp(lexer, lexer_advance_with_token(lexer, init_token(TOKEN_LPAREN,         lexer_get_current_char_as_string(lexer))), line, col);
            case ')': return stamp(lexer, lexer_advance_with_token(lexer, init_token(TOKEN_RPAREN,         lexer_get_current_char_as_string(lexer))), line, col);
        }

        lexer_advance(lexer);
    }

    return NULL;
}
