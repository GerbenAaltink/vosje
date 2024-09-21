#ifndef LEXER_H
#define LEXER_H
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include <stddef.h>
#include <ctype.h>
#include <stdbool.h>

bool is_number(char c){
    return c >= '0' && c <= '9';
}
bool is_operator(char c){
    return c == '*' || c == '+' || c == '/' || c == '-' || c == '=' || c == '.' || c == ':' || c == '!' || c == '<' || c == '>' || c == '=' || c == ',';

}

char * lex_group(Buffer * buffer, char char_open, char char_close) {

    Buffer * group = Buffer_construct();

    // Remove first char, should be equal to char_open
    Buffer_read_c(buffer);

    int depth = 1;
    char c;
    while((c = Buffer_read_c(buffer))){
        if(c == char_open){
            depth++;
        }
        if(c == char_close){
            depth--;
        }
        if(depth == 0)
            break;
        Buffer_write_c(group,c);
    }
    return Buffer_tostr(group);
}

char * lex_string(Buffer * buffer){
    Buffer * string = Buffer_construct();
    
    // Remove first "
    Buffer_read_c(buffer);
    
    char c;
    while((c = Buffer_read_c(buffer)) != '"'){
        Buffer_write_c(string, c);
        if(c == '\\'){
            c = Buffer_read_c(buffer);
            Buffer_write_c(string, c);   
        }
    }
    return Buffer_tostr(string);
}

char * lex_char(Buffer * buffer){
    char * res = (char *)malloc(sizeof(char) * 2);
    res[0] = Buffer_read_c(buffer);
    res[1] = '\0';
    return res;
}

char * lex_symbol(Buffer * buffer){
    Buffer * symbol = Buffer_construct();
    char c;
    short dot_count = 0;
    while((c = Buffer_read_c(buffer)) != '\0'){
        if(isalnum(c)){
            Buffer_write_c(symbol,c);
        }else{
            Buffer_unread_c(buffer);
            break;
        }
    }
    return Buffer_tostr(symbol);
}
char * lex_number(Buffer * buffer){
    Buffer * number = Buffer_construct();
    char c;
    short dot_count = 0;
    while((c = Buffer_read_c(buffer)) != '\0'){
        if(is_number(c)){
            Buffer_write_c(number,c);
        }else if(dot_count == 0 && c == '.'){
            dot_count++;
            Buffer_write_c(number,c);
        }else{
            Buffer_unread_c(buffer);
            break;
        }
    }
    return Buffer_tostr(number);
}
char * lex_operator(Buffer * buffer) {
    Buffer * operator = Buffer_construct();
    char c;
    while((c = Buffer_read_c(buffer)) != '\0'){
        if(is_operator(c)){
            Buffer_write_c(operator,c);
        }else{
            Buffer_unread_c(buffer);
            break;
        }
    }
    return Buffer_tostr(operator); 
}

Token * lex(char * filename, Buffer * buffer){
    
    char c;
    Token * first = Token_construct();
    Token * token = first;
    unsigned int col = 0;
    unsigned int line = 1;
    while((c = Buffer_peek_c(buffer)) != '\0'){
        
        col++;
        token->col = col;
        token->line = line;
        if(c == '\n') {
            col = 0;
            line++;
            Buffer_read_c(buffer);
            continue;
        } else if(c == ' ' || c == '\t'){
            Buffer_read_c(buffer);
            continue;
        }else if(is_number(c)){
            char * number = lex_number(buffer);
            token->content = number;
            token->type = TT_NUMBER;
            //printf("NR:%s\n", number);
        }else if(is_operator(c)){
            char * operator = lex_operator(buffer);
            token->content = operator;
            token->type = TT_OPERATOR;
            //printf("OP:%s\n", operator);
        }else if(c == '"'){
            char * string = lex_string(buffer);
            token->content = string;
            token->type = TT_STRING;
            //printf("STR:%s\n", string);
        }
        else if(c == '{'){
            char * group_curly_braces = lex_group(buffer, '{', '}');
            token->content = group_curly_braces;
            token->type = TT_CURLY_BRACES;
        }else if(c == '('){
            char * group_braces = lex_group(buffer, '(', ')');
            token->content = group_braces;
            token->type = TT_BRACES;
        }else if(c == '['){
            char * group_brackets = lex_group(buffer, '[', ']');
            token->content = group_brackets;
            token->type = TT_BRACKETS;
        }
        else if(c == '{'){
            char * group_char = lex_char(buffer);
            token->content = group_char;
            token->type == TT_CURLY_BRACE_OPEN;
        }else if(c == '}'){
            char * group_char = lex_char(buffer);
            token->content = group_char;
            token->type == TT_CURLY_BRACE_CLOSE;
        }else if(c == '('){
            char * group_char = lex_char(buffer);
            token->content = group_char;
            token->type == TT_BRACE_OPEN;
        }else if(c == ')'){
            char * group_char = lex_char(buffer);
            token->content = group_char;
            token->type == TT_BRACE_CLOSE;
        }else if(c == '['){
            char * group_char = lex_char(buffer);
            token->content = group_char;
            token->type == TT_BRACE_OPEN;
        }else if(c == ']'){
            char * group_char = lex_char(buffer);
            token->content = group_char;
            token->type == TT_BRACE_CLOSE;
        }
        else if(isalnum(c)) {
            char * symbol = lex_symbol(buffer);
            token->content = symbol;
            token->type = TT_CONST;
        }
        
        else{
            //printf("UNDEFINED:%c\n",c);
            // Remove undefined char from buffer
            Buffer_read_c(buffer);
            continue;
        }

        token->next = Token_construct();
        token->next->prev = token;
        token = token->next;

    }
    //if(token && token->prev)
    token->prev->next = NULL;
    if(token->prev)
    Token_destruct(token);
    return first;
}

#endif