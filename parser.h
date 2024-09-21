#define DEBUG_LINE printf("Executed ",__LINE__);

#include "buffer.h"
#include "value.h"
#include "lexer.h"
#include "token.h"
#include "range.h"
#include "parser_def.h"
#include "builtins/dir.h"
#include "builtins/while.h"
#include "builtins/print.h"
#include "builtins/return.h"
#include "builtins/class.h"
#include "builtins/arguments.h"
#include "builtins/len.h"
#include "builtins/for.h"


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




static int fn_id_v = 0;

Value * fn_id(Value * args){

    fn_id_v++;

    return Value_from_double((double)fn_id_v);
}

typedef Value * (*Parser_Function)(Parser * parser);

Value * Parser_register(Parser * parser, char * name, Parser_Function fn){
    Value * val = Value_construct();
    Value_push(parser->methods, val);
    val->type = VT_INTERNAL_FUNCTION;
    val->method = fn;
    Value_set_name(val, name);
    return val;
}

Value * Parser_register_function(Parser * parser, char * name, Value_Function fn){
    Value * val = Value_construct();
    Value_push(parser->vars, val);
    val->type = VT_INTERNAL_FUNCTION;
    val->method = fn;
    Value_set_name(val, name);
    return val;
}

Value *parse_cmp(Value * left, Value * right){
    Value * res = Value_construct();
    res->type = VT_NUMBER;
    res->number = 0;
    if(left->type == VT_NULL && right->type == VT_NULL)
        res->number = 1;
    else if(left->value && right->value && !strcmp(Value_string(left), Value_string(right))){
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


Value * parse_array(Parser * parser){

    Value * res = Value_construct();
    res->type = VT_ARRAY;
    
    Token * original = parser->token;

    Buffer * source = Buffer_s(parser->token->content);
    parser->token = lex("<stdin>",source);
    Buffer_destruct(source);

    while(parser->token){
        Value * value = parse_expression(parser);
        Value * n = Value_construct();
        if(res->children == NULL){
            res->children = n;
        }else{
        Value_push(res->children, n);
        }
        Value_copy(n, value);
    }
    parser->token = original->next;
    return res;
}

Value * parse_brackets(Parser * parser, Value * value){
    
    if(!parser->token || parser->token->type != TT_BRACKETS){
        return value;
    }
    Range * range = Range_parse(parser, parser->token->content);
    if(value->type == VT_ARRAY && range->has_position){
        value = value->children;
        int i = 0;
        while(i < range->position && value){
            value = value->next;
            i++;
        }
        //if(parser->token)
        //    parser->token = parser->token->next;
        if(parser->token){
        return parse_brackets(parser, value);
        }else{
            return value;
        }
    }
    char * nstr = string_substr(Value_string(value),range->has_position ? range->position : range->start, range->has_position ? 1 : range->end);
    Value * nval = Value_from_string(nstr);
    Value_push(value, nval);
    //if(parser->token)
      //  parser->token = parser->token->next;

    return parse_brackets(parser, nval);
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
    
    Value *right = NULL;
    Value * vars_original;
Value *factor = NULL;
    if(parser->token->type == TT_BRACKETS){
       
        Value * arr = parse_array(parser);
        return arr;
    }

    if (parser->token->type == TT_CONST)
    {
        Value * m = Value_get(parser->methods, parser->token->content);
        if(m){
            parser->token = parser->token->next;
            Parser_Function fn = m->method;
            return fn(parser);
        }
    }
    

    if(parser->token->type == TT_BRACKETS){
            Value * original_values = parser->vars;
            Value * object = Value_construct();
            object->type = VT_OBJECT;
            Value * object_vars = Value_children(object);
            parser->vars = object_vars;
            eval(parser->token->content, parser);
            Value_push(original_values, object);
            parser->vars = original_values;
            parser->token = parser->token->next;
            //Value_dump(original_values);
            return object;
    }

    factor = parse_term(parser);
          


    if(!parser->token)
        return factor;

    
    if(parser->token->type == TT_BRACES){
        if(factor->type == VT_INTERNAL_FUNCTION){
            Value_Function fn = factor->method;
            Value * args = parse_arguments(parser,parser->token->content);
            factor = fn(args);
            if(parser->token)
                parser->token = parser->token->next;
           
        }else if(factor->type == TT_NONE){
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
        if(!strcmp(parser->token->content, ",")){
            parser->token = parser->token->next;
        }
        else if (!strcmp(parser->token->content, "+"))
        {
            parser->token = parser->token->next;
            right = parse_expression(parser);
            factor = Value_plus(factor, right);
        }
        else if (!strcmp(parser->token->content, "-"))
        {
            parser->token = parser->token->next;
            right = parse_expression(parser);
            factor = Value_min(factor, right);
        }
        else if (!strcmp(parser->token->content, "="))
        {
            parser->token = parser->token->next;
            right = parse_expression(parser);
            Value_copy(factor, right);
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
            factor->number--;
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
        if(!strcmp(parser->token->content, "NULL")){
            result = Value_construct();
            result->type = VT_NULL;
        }else{
            result = Value_get_or_create(parser->vars, parser->token->content);
        }
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
    if(result && (result->type == VT_STRING || result->type == VT_ARRAY )){
        return parse_brackets(parser, result);
    }
    return result;
}

Value *eval(char *content, Parser *parser)
{
    if(content == NULL || !strlen(content)){
        return NULL;
    }
    Token *original = parser->token;

    Buffer * source = Buffer_s(content);
    parser->token = lex("eval", source);
    Buffer_destruct(source);
;
    Value *result = NULL;
    Value * original_vars = parser->vars;
    while (parser->token)
    {
        result = parse_expression(parser);
    }
    parser->vars = original_vars;
    parser->token = original;
    return result;
}

Value *parse(char *content)
{
    Parser *parser = Parser_construct(content, NULL);
    Value * original = parser->vars;
    Parser_register(parser, "print", parse_print);
    Parser_register(parser, "strlen", parse_len);
    Parser_register(parser, "len", parse_len);
    Parser_register(parser, "count", parse_len);
    Parser_register(parser, "dump", parse_dump);
    Parser_register(parser, "class", parse_class);
    Parser_register(parser, "if", parse_if);
    Parser_register(parser, "return", parse_return);
    Parser_register(parser, "while", parse_while);
    Parser_register(parser, "for", parse_for);
    Parser_register_function(parser, "fopen", Builtin_fopen);
    Parser_register_function(parser, "fgetc", Builtin_fgetc);
    Parser_register_function(parser, "uid",fn_id);
    Parser_register_function(parser, "dir",Builtin_dir);
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