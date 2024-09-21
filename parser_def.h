#ifndef PARSER_DEF_H
#define PARSER_DEF_H
#include "value.h"
#include "token.h"
#include "buffer.h"
#include "env.h"
#include "lexer.h"
#include <stdlib.h>
typedef struct Parser
{
    Buffer *content;
    Value *value;
    Token *token;
    Env *env;
    Value *vars;
    Value * methods;
} Parser;

Parser *Parser_construct(char *content, Env *env)
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->content = Buffer_s(content);
    parser->token = lex("<stdin>", parser->content);
    parser->vars = Value_construct();
    parser->methods = Value_construct();
    Value_set_name(parser->vars, "globals");
    return parser;
}

void Parser_destruct(Parser *parser)
{
    if (parser->content != NULL)
    {
        free(parser->content);
    }
    if (parser->token != NULL)
    {
        Token *first_token = Token_first(parser->token);
        Token_destruct(first_token);
    }
    if (parser->env != NULL)
    {
      //  Env_destruct(parser->env);
    }
    if(parser->vars != NULL){
        Value_destruct(parser->vars);
    }
    free(parser);
}

Value *parse_expression(Parser *parser);
Value *parse_term(Parser *parser);
Value *parse_factor(Parser *parser);
Value *parse(char *content);
Value *eval(char *content, Parser *parser);
#endif