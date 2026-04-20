#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOKEN_ID,
    TOKEN_FUNC,
    TOKEN_ENTRY,
    TOKEN_END,
    TOKEN_PROGRAMA,
    TOKEN_EQUALS,
    TOKEN_STRING,
    TOKEN_REAL,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_OPENINGBRACKET,
    TOKEN_CLOSINGBRACKET,
    TOKEN_SE,
    TOKEN_SENAO,
    TOKEN_ENQUANTO,
    TOKEN_PARA,
    TOKEN_REPITA,
    TOKEN_ATE,
    TOKEN_RETORNE,
    TOKEN_VETOR,
    TOKEN_BOOL,
    TOKEN_E,
    TOKEN_OU,
    TOKEN_NAO,
    TOKEN_MAIOR,
    TOKEN_MENOR,
    TOKEN_MAIOR_EQ,
    TOKEN_MENOR_EQ,
    TOKEN_DIFERENTE,
    TOKEN_IGUAL,
    TOKEN_MAIS,
    TOKEN_MENOS,
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_VIRGULA,
    TOKEN_IMPORTAR,
    TOKEN_STRING_PART,
    TOKEN_INTERP_EXPR
} TokenType;

typedef struct TOKEN_STRUCT
{
    TokenType type;
    char*     value;
    int       line;
    int       column;
} token_T;

token_T* init_token(TokenType type, char* value);

#endif
