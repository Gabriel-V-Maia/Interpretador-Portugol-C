#include "include/lexer.h"
#include "debugger/debugger.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

typedef struct {
    const char* word;
    TokenType   type;
} keyword_t;

static keyword_t keywords[] = {
    {"funcao", TOKEN_FUNC},
    {"programa", TOKEN_PROGRAMA},
    {"se", TOKEN_SE},
    {"senao", TOKEN_SENAO},
    {"enquanto", TOKEN_ENQUANTO},
    {"para", TOKEN_PARA},
    {"repita", TOKEN_REPITA},
    {"ate", TOKEN_ATE},
    {"retorne", TOKEN_RETORNE},
    {"vetor", TOKEN_VETOR},
    {"verdadeiro", TOKEN_BOOL},
    {"falso", TOKEN_BOOL},
    {"e", TOKEN_E},
    {"ou", TOKEN_OU},
    {"nao", TOKEN_NAO},
    {"importar", TOKEN_IMPORTAR},
    { NULL, 0 }
};

static TokenType resolve_keyword(const char* value)
{
    for (int i = 0; keywords[i].word != NULL; i++)
        if (strcmp(keywords[i].word, value) == 0)
            return keywords[i].type;
    return TOKEN_ID;
}

lexer_T* init_lexer(char* contents, Debugger* debugger_instance)
{
    lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->contents     = contents;
    lexer->contents_len = strlen(contents);
    lexer->index        = 0;
    lexer->currentChar  = contents[0];
    lexer->line         = 1;
    lexer->column       = 1;
    lexer->debugger_instance = debugger_instance;
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
    while (isspace(lexer->currentChar))
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

static token_T* stamp(token_T* token, int line, int col)
{
    token->line   = line;
    token->column = col;
    return token;
}

token_T* lexer_collect_string(lexer_T* lexer)
{
    int start_line = lexer->line;
    int start_col  = lexer->column;

    lexer_advance(lexer);
    char* value = calloc(1, sizeof(char));
    value[0] = '\0';

    while (lexer->currentChar != '"' && lexer->currentChar != '\0')
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, strlen(value) + strlen(s) + 1);
        strcat(value, s);
        free(s);
        lexer_advance(lexer);
    }

    lexer_advance(lexer);
    return stamp(init_token(TOKEN_STRING, value), start_line, start_col);
}

token_T* lexer_collect_id(lexer_T* lexer)
{
    int start_line = lexer->line;
    int start_col  = lexer->column;

    char* value = calloc(1, sizeof(char));
    value[0] = '\0';

    while (isalnum(lexer->currentChar) || lexer->currentChar == '_')
    {
        char* s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, strlen(value) + strlen(s) + 1);
        strcat(value, s);
        free(s);
        lexer_advance(lexer);
    }

    token_T* token;

    if (isdigit((unsigned char)value[0]))
    {
        if (lexer->currentChar == '.')
        {
            value = realloc(value, strlen(value) + 2);
            strcat(value, ".");
            lexer_advance(lexer);

            while (isdigit(lexer->currentChar))
            {
                char* s = lexer_get_current_char_as_string(lexer);
                value = realloc(value, strlen(value) + strlen(s) + 1);
                strcat(value, s);
                free(s);
                lexer_advance(lexer);
            }
        }
        token = init_token(TOKEN_REAL, value);
    }
    else
    {
        TokenType type = resolve_keyword(value);
        token = init_token(type, value);
    }

    debugger_print(lexer->debugger_instance, "Criado token %s com tipo %d", token->value, token->type);
    return stamp(token, start_line, start_col);
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

        if (isalnum(lexer->currentChar) || lexer->currentChar == '_')
            return lexer_collect_id(lexer);

        if (lexer->currentChar == '"')
            return lexer_collect_string(lexer);

        switch (lexer->currentChar)
        {
        case '=':
            lexer_advance(lexer);
            if (lexer->currentChar == '=') {
                lexer_advance(lexer);
                return stamp(init_token(TOKEN_IGUAL,    "=="), line, col);
            }
            return stamp(init_token(TOKEN_EQUALS, "="), line, col);

        case '!':
            lexer_advance(lexer);
            if (lexer->currentChar == '=') {
                lexer_advance(lexer);
                return stamp(init_token(TOKEN_DIFERENTE, "!="), line, col);
            }
            break;

        case '>':
            lexer_advance(lexer);
            if (lexer->currentChar == '=') {
                lexer_advance(lexer);
                return stamp(init_token(TOKEN_MAIOR_EQ, ">="), line, col);
            }
            return stamp(init_token(TOKEN_MAIOR, ">"), line, col);

        case '<':
            lexer_advance(lexer);
            if (lexer->currentChar == '=') {
                lexer_advance(lexer);
                return stamp(init_token(TOKEN_MENOR_EQ, "<="), line, col);
            }
            return stamp(init_token(TOKEN_MENOR, "<"), line, col);

        case '+': return stamp(lexer_advance_with_token(lexer, init_token(TOKEN_MAIS,           "+")), line, col);
        case '-': return stamp(lexer_advance_with_token(lexer, init_token(TOKEN_MENOS,          "-")), line, col);
        case '*': return stamp(lexer_advance_with_token(lexer, init_token(TOKEN_MULT,           "*")), line, col);
        case '/': return stamp(lexer_advance_with_token(lexer, init_token(TOKEN_DIV,            "/")), line, col);
        case '{': return stamp(lexer_advance_with_token(lexer, init_token(TOKEN_OPENINGBRACKET, "{")), line, col);
        case '}': return stamp(lexer_advance_with_token(lexer, init_token(TOKEN_CLOSINGBRACKET, "}")), line, col);
        case '(': return stamp(lexer_advance_with_token(lexer, init_token(TOKEN_LPAREN,         "(")), line, col);
        case ')': return stamp(lexer_advance_with_token(lexer, init_token(TOKEN_RPAREN,         ")")), line, col);
        case ',': return stamp(lexer_advance_with_token(lexer, init_token(TOKEN_VIRGULA,        ",")), line, col);
        }

        lexer_advance(lexer);
    }

    return stamp(init_token(TOKEN_END, "\0"), lexer->line, lexer->column);
}
