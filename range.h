#include <stdbool.h>
#include <stdlib.h>
#include "lexer.h"
#include "buffer.h"
#include "value.h"
#include "parser_def.h"

typedef struct Range {
    int start;
    bool has_start;
    int end;
    bool has_end;
    int position;
    bool has_position;
    char * key;
    bool has_key;
    bool is_range;
} Range;

Range * Range_create(){
    Range * range = (Range *)malloc(sizeof(Range));
    range->start = 0;
    range->has_start = false;
    range->has_end = false;
    range->end = 0;
    range->position = 0;
    range->is_range = false;
    range->has_position = false;
    return range;
}

Range * Range_parse(Parser * parser, char * str){
    Token * token;
    Range * range = Range_create();
    Value * value;
    Token * token_first = lex("<range>", Buffer_s(str));
    token = token_first;
    Token * original = parser->token->next;
    parser->token = token_first;

        //parser->token->type != TT_OPERATOR
    if(strcmp(parser->token->content, ":")){
        value = parse_expression(parser);
        range->has_start = true;
        range->start = (int)Value_number(value);
    }
    if(parser->token == NULL || parser->token->type == TT_NONE)
    {

        range->has_position = true;
        range->position = (int)Value_number(value);
        parser->token = original;
        
        return range;
    }
    if(parser->token->type == TT_OPERATOR){
        range->is_range = true;
        parser->token = parser->token->next;
        
    }
    if(token == NULL || token->type == TT_NONE){
        parser->token = original;
        return range;
    
    }
    value = parse_expression(parser);
    range->end = (int)Value_number(value);
    range->has_end = true;
    parser->token = original;
    return range;
}