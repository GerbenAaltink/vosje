#define DEBUG_LINE printf("Executed ",__LINE__);

#include "buffer.h"
#include "value.h"
#include "lexer.h"
#include "token.h"
#include "env.h"
#include "range.h"

typedef struct Parser
{
    Buffer *content;
    Value *value;
    Token *token;
    Env *env;
    Value *vars;
} Parser;

Parser *Parser_construct(char *content, Env *env)
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->content = Buffer_s(content);
    parser->token = lex("<stdin>", parser->content);
    Token_dump(parser->token);
    parser->env = Env_construct(env);
    parser->vars = Value_construct();
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
        Env_destruct(parser->env);
    }
    if(parser->vars != NULL){
        Value_destruct(parser->vars);
    }
    free(parser);
}

Value *Value_multiply(Value *left, Value *right)
{
    return Value_from_double(Value_number(left) * Value_number(right));
}
Value *Value_devide(Value *left, Value *right)
{
    return Value_from_double(Value_number(left) / Value_number(right));
}
Value *Value_plus(Value *left, Value *right)
{
    return Value_from_double(Value_number(left) + Value_number(right));
}
Value *Value_min(Value *left, Value *right)
{
    return Value_from_double(Value_number(left) - Value_number(right));
}

Value *parse_expression(Parser *parser);
Value *parse_term(Parser *parser);
Value *parse_factor(Parser *parser);
Value *parse(char *content);
Value *eval(char *content, Parser *parser);

Env *parse_parameters(Parser *parser)
{
    Token *original = parser->token;
}

char * string_substr(char * str, int start, int end){
    char * result = (char *)calloc(strlen(str) + 1,sizeof(char));
    int c;
    int length = 0;
    int index = -1;
    while(index != strlen(str)){
        index++;
        if(index < start){
            continue;
        }
        result[length] = str[index];
        
        length++;
        if(end && length == end )
            break;
    }
    return result;
}


char resolve_close_char(char open_char){
    char stop_char = 0;
    if(open_char == '{'){
        stop_char = '}';
    }else if(open_char == '('){
        stop_char = ')';
    }else if(open_char == '[')
        stop_char = ']';
    return stop_char;
}

Token * Token_find_end(Token * token){
    char open_char = token->content[0];
    char stop_char = resolve_close_char(token->content[0]);
    int opens = 1;
    while(token){
        if(token->content[0] == open_char){
            opens++;
        }
        if(token->content[0] == stop_char){
            opens--;
        }
        if(opens == 0){
            return token;
        }
        token = token->next;
    }
    return token;
}

bool Value_is_truthy(Value * val){
    if(val->value != NULL){
        char * s = Value_string(val);
        return strlen(s) > 0;
    }
    return (int)val->number > 0;
}


Value * parse_class(Parser * parser){
    Value * res = Value_construct();
    char * name = parser->token->content;
    Value_set_name(res, name);
    parser->token = parser->token->next;
    res->type = VT_CLASS;
    char * body = parser->token->content;
    //Value * body_value = Value_from_string(body);
    //Value * body_value = Value_from_token(parser->token, t_end);
    Value * body_value = Value_from_string(body);
    Value_set_name(body_value, "body");
    Value * children = Value_children(res);
    Value_push(children, body_value);
    Value_push(parser->vars, res);
    parser->token = parser->token->next;
    return res;
}

Value * parse_dump(Parser * parser){
    Token *original = parser->token;
          
    parser->token = lex("eval", Buffer_s(parser->token->content));
    Value *result = NULL;
    while (parser->token && parser->token->type != TT_NONE)
    {
        result = parse_expression(parser);
        Value_dump(result);
    }
    parser->token = original->next;
    return NULL;
}

Value * parse_while(Parser * parser){
    Value * res;
    char * condition = parser->token->content;
    parser->token = parser->token->next;
    char * block = parser->token->content;
    Value * vars_original = parser->vars;
    Value * vars_last = Value_last(parser->vars);
   
    while(Value_is_truthy((eval(condition, parser)))){
        res = eval(block, parser);
       
        vars_last->next = NULL;
        if(vars_last->next)
        {
            Value_destruct(vars_last->next);
            vars_last->next = NULL;
        }
    }
    parser->token = parser->token->next;
    return res;
}

Value * parse_strlen(Parser * parser){
    Value * content = eval(parser->token->content, parser);
   
    Value * value = Value_construct();
    value->number = (double)strlen(Value_string(content));
    value->type = VT_NUMBER;
    parser->token = parser->token->next;
    return value;
}


Value *parse_cmp(Value * left, Value * right){
    Value * res = Value_construct();
    res->type = VT_NUMBER;
    res->number = 0;
    if(left->value && right->value && !strcmp(Value_string(left), Value_string(right))){
        res->number = 1;
    }else if(left->type == VT_NUMBER && right->type == VT_NUMBER && left->number == right->number){
        res->number = 1;
    }
    return res;
}

Value *parse_if(Parser * parser){
    Value * ret = eval(parser->token->content, parser);
    
    if(Value_is_truthy(ret)){
            parser->token = parser->token->next;
        eval(parser->token->content, parser);
    }else{

            parser->token = parser->token->next;
    }

    parser->token = parser->token->next;
    //var->number = (int)Value_is_truthy(ret);
    //var->type = VT_NUMBER;

}

Value * parse_arguments(Parser * parser, char * data){
    if(!strlen(data))
        return NULL;
    Value * result = NULL;
    Value * temp;
    Token * original = parser->token;
    parser->token = lex("<parse_args",Buffer_s(data));

    char * name;
    while(parser->token){
        if(parser->token->type == TT_CONST){
            name = parser->token->content;
            parser->token = parser->token->next;
            temp = Value_construct();
            Value_set_name(temp,name); 
             
            
        }
        if(parser->token->type == TT_OPERATOR){
            parser->token = parser->token->next;
            Value * right  = parse_expression(parser);
            Value_copy(temp, right);
        }else{
            temp = Value_construct();
            Value_set_name(temp, name);
        }
        if(result == NULL){
            result = temp;
        }else{
            Value_push(result, temp);
        }
        parser->token = parser->token->next;
    }
    parser->token = original;
    return result;
}


Value * parse_brackets(Parser * parser, Value * value){
    
    if(!parser->token || parser->token->type != TT_BRACKETS){
        return value;
    }
    Range * range = Range_parse(parser->token->content);
    char * nstr = string_substr(Value_string(value),range->has_position ? range->position : range->start, range->has_position ? 1 : range->end);
    Value * nval = Value_from_string(nstr);
    Value_push(value, nval);
    parser->token = parser->token->next;

    return parse_brackets(parser, nval);
}

bool function_print(Parser *parser)
{
    Token *original = parser->token;

    Buffer * zz = Buffer_s(parser->token->content);
    printf("(%s)\n",zz->content);
    parser->token = lex("eval", Buffer_s(parser->token->content));
    Value *result = NULL;
    while (parser->token && parser->token->type != TT_NONE)
    {
        result = parse_expression(parser);
        Value_print(result);
    }
    printf("\n");
    parser->token = original->next;
    return NULL;
    // return result;
}


Value * parse_grouping(Parser * parser){
    Value * original = parser->vars;
    Value * scope = Value_children(parser->vars);
    parser->vars = scope;
    char open_char = parser->token->content[0];
    char stop_char = 0;
    if(open_char == '{'){
        stop_char = '}';
    }else if(open_char == '('){
        stop_char = ')';
    }else if(open_char == '[')
        stop_char = ']';

    int opens = 1;
    Value * value;
    while(parser->token){
        value = parse_expression(parser);
    }
    parser->vars - original;
    return value;
}

Value *parse_expression(Parser *parser)
{
    if(parser->token->type == TT_BRACE_OPEN){
        printf("HHHH\n");
    }
    Value *right = NULL;
    Value * vars_original;
printf(":%s:\'n",parser->token->content);
        
    if (parser->token->type == TT_CONST)
    {
        if (!strcmp(parser->token->content, "print"))
        {
            
            parser->token = parser->token->next;
            function_print(parser);
            return NULL;
        }
        if(!strcmp(parser->token->content, "strlen")){
            parser->token = parser->token->next;
            return parse_strlen(parser);
            
        }
        if(!strcmp(parser->token->content, "dump")){
            parser->token = parser->token->next;
            return parse_dump(parser);
        }
        if(!strcmp(parser->token->content, "class")){
            parser->token = parser->token->next;
            return parse_class(parser);
        }
        if(!strcmp(parser->token->content, "if")){
            parser->token = parser->token->next;
            return parse_if(parser);
        }
        if(!strcmp(parser->token->content, "while")){
    
            parser->token = parser->token->next;
            printf("HIERRR\n");
            return parse_while(parser);

        }else if(!strcmp(parser->token->content, "return")){
            parser->token = parser->token->next;
            
            Value * res = parse_expression(parser);
            return res;
        }
    }
    

    Value *factor = parse_term(parser);

    if(!parser->token)
        return factor;
    
    if(parser->token->type == TT_BRACES){
        if(factor->type == TT_NONE){
            Value * function_children = Value_children(factor);
            Value * function_arguments = Value_from_string(parser->token->content);
            Value_set_name(function_arguments, "arguments");
            Value_push(function_children, function_arguments);
            parser->token = parser->token->next;
            Value * function_body = Value_from_string(parser->token->content);
            Value_set_name(function_body, "body");
            Value_push(function_children, function_body);
            factor->type = VT_FUNCTION;
            parser->token = parser->token->next;
        }else if(factor->type == VT_FUNCTION){
            Value * function_arguments_execute = Value_get(factor->children, "arguments");
            Value * function_body_execute = Value_get(factor->children, "body");
            vars_original = parser->vars;
            Value * args = parse_arguments(parser,Value_string(function_arguments_execute));
            //eval(Value_string(function_arguments_execute), parser);
            Value * last_original = Value_last(parser->vars);
            Value_push(parser->vars, args);

            args = parse_arguments(parser, parser->token->content);
            Value_push(parser->vars, args);
            parser->vars = factor;
            factor = eval(Value_string(function_body_execute), parser);
            if(last_original->next){
                //Value_destruct(last_original->next);
                last_original->next = NULL;
            }
            parser->vars = vars_original;
              parser->token = parser->token->next;
        }else if(factor->type == VT_CLASS){
            Value * class_body = Value_get(factor->children, "body");
            Value * original_values = parser->vars;
            Value * object = Value_construct();
            object->type = VT_OBJECT;
            Value * object_vars = Value_children(object);
            parser->vars = object_vars;
            eval(Value_string(class_body), parser);
            Value_push(original_values, object);
            parser->vars = original_values;
            parser->token = parser->token->next;
            //Value_dump(original_values);
            return object;
        }else {
            parse_grouping(parser);
        }
    }

    while (parser->token && parser->token->type == TT_OPERATOR)
    {
        if (!strcmp(parser->token->content, "+"))
        {

            parser->token = parser->token->next;
            right = parse_term(parser);
            factor = Value_plus(factor, right);
        }
        else if (!strcmp(parser->token->content, "-"))
        {
            parser->token = parser->token->next;
            right = parse_term(parser);
            factor = Value_min(factor, right);
        }
        else if (!strcmp(parser->token->content, "="))
        {
            parser->token = parser->token->next;
            right = parse_expression(parser);
            Value_copy(factor, right);
            Value_print(right);
            factor->children = right->children;
        }else if(!strcmp(parser->token->content, "==")){
            parser->token = parser->token->next;
            right = parse_expression(parser);
            factor = parse_cmp(factor,right);
        }else if(!strcmp(parser->token->content, "!=")){
            parser->token = parser->token->next;
            right = parse_expression(parser);
            factor = parse_cmp(factor,right);
            factor->number = factor->number == 1 ? 0 : 1;
        }else if(!strcmp(parser->token->content, "++")){
            factor->number++;
            parser->token = parser->token->next;
        }else if(!strcmp(parser->token->content, "--")){
            factor->number++;
            parser->token = parser->token->next;
        }
        else
        {
            break;
        }
    }
    return factor;
}

Value *parse_term(Parser *parser)
{
    Value *factor = parse_factor(parser);
    Value *right;
    while (parser->token && parser->token->type == TT_OPERATOR)
    {

        if (!strcmp(parser->token->content, "/"))
        {

            parser->token = parser->token->next;
            right = parse_factor(parser);
            factor = Value_devide(factor, right);
        }
        else if (!strcmp(parser->token->content, "*"))
        {
            parser->token = parser->token->next;
            right = parse_factor(parser);
            factor = Value_multiply(factor, right);
        }
        else
        {
            break;
        }
    }
    return factor;
}

Value *parse_factor(Parser *parser)
{
    Value *result = NULL;
    if (parser->token == NULL)
        return result;
    if (parser->token->type == TT_STRING)
    {
        result = Value_from_string(parser->token->content);
    }
    else if (parser->token->type == TT_NUMBER)
    {
        result = Value_from_number(parser->token->content);
    }
    else if (parser->token->type == TT_BRACES)
    {
        result = eval(parser->token->content, parser);
    }
    else if (parser->token->type == TT_CONST)
    {
        result = Value_get_or_create(parser->vars, parser->token->content);
    }
    parser->token = parser->token->next;
    if (parser->token && parser->token->content && !strcmp(parser->token->content, "."))
    {
       
        parser->token = parser->token->next;
        Value *original_vars = parser->vars;
        parser->vars = Value_children(result);
        result = parse_factor(parser);
        parser->vars = original_vars;
    }

    if (result && result->prev == NULL)
        Value_push(parser->vars, result);
    if(result && result->type == TT_STRING)
        return parse_brackets(parser, result);
    return result;
}

Value *eval(char *content, Parser *parser)
{
    if(content == NULL || !strlen(content)){
        return NULL;
    }
    Token *original = parser->token;
    parser->token = lex("eval", Buffer_s(content));
    Value *result = NULL;
    Value * original_vars = parser->vars;
    while (parser->token)
    {
        result = parse_expression(parser);
        //parser->vars = original_vars;
    }
    parser->vars = original_vars;
    parser->token = original;
    return result;
}

Value *parse(char *content)
{
    Parser *parser = Parser_construct(content, NULL);
    Value * original = parser->vars;
    Value *result = NULL;
    while (parser->token)
    {
        result = parse_grouping(parser);
        parser->vars = original;
    }
    Value * result_cpy = Value_construct();
    if(result)
        Value_copy(result_cpy, result);
    Parser_destruct(parser);
    return result;
}