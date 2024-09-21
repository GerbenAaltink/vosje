#ifndef TOKEN_H
#define TOKEN_H
#include <stdlib.h>
#include <stdio.h>


typedef enum TokenType {
    TT_NONE = 'N',
    TT_PLUS,
    TT_MINUS,
    TT_STRING = 's',
    TT_CONST = 'c',
    TT_NUMBER = 'n',
    TT_BOOLEAN = 'b',
    TT_OPERATOR,
    TT_BRACKETS,
    TT_CURLY_BRACES,
    TT_BRACES,
    TT_CURLY_BRACE_OPEN,
    TT_CURLY_BRACE_CLOSE,
    TT_BRACE_OPEN,
    TT_BRACE_CLOSE,
    TT_BRACKED_OPEN,
    TT_BRACKED_CLOSE
} TokenType;

typedef struct Token {
    char * filename;
    int line;
    int col;
    char * content;
    TokenType type;
    struct Token * next;
    struct Token * prev;
} Token;

Token * Token_construct(){
    Token * token = (Token *)malloc(sizeof(Token));
    token->type = TT_NONE;
    token->filename = NULL;
    token->line = 0;
    token->col = 0;
    token->content = NULL;
    token->next = NULL;
    token->prev = NULL;
    return token;
}

void Token_destruct(Token * token){
    if(token->prev == NULL){
        free(token->filename);
    }
    if(token->content != NULL)
    {
        free(token->content);
    }
    if(token->next){
        Token_destruct(token->next);
    }
    free(token);

}

Token * Token_first(Token * token){
    while(token->prev)
        token = token->prev;
    return token;
}

Token * Token_dump(Token * token){
    printf("Line: %d Col:%d (%s)\n",token->line, token->col, token->content);
    if(token->next){
        Token_dump(token->next);
    }
}

#endif