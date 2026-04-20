#include <stddef.h>
#include "../debugger/debugger.h"
#include "token.h"

#ifndef LEXER_H
#define LEXER_H

#define INTERP_QUEUE_MAX 64

typedef struct LEXER_STRUCT
{
    char         currentChar;
    unsigned int index;
    char*        contents;
    size_t       contents_len;
    int          line;
    int          column;
    Debugger*    debugger_instance;
    token_T*     interp_queue[INTERP_QUEUE_MAX];
    int          interp_queue_size;
    int          interp_queue_pos;
} lexer_T;

lexer_T* init_lexer(char* contents, Debugger* debugger_instance);
void     lexer_advance(lexer_T* lexer);
void     lexer_skip_whitespace(lexer_T* lexer);
token_T* lexer_get_next_token(lexer_T* lexer);
token_T* lexer_collect_string(lexer_T* lexer);
token_T* lexer_collect_id(lexer_T* lexer);
char*    lexer_get_current_char_as_string(lexer_T* lexer);
token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token);

#endif
