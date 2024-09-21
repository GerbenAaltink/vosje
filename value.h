#ifndef VALUE_H
#define VALUE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
/*
char * strd(char * str){
    char * new = (char *)calloc(strlen(str) +1, sizeof(char));
    strcpy(new, str);
    return new;
}*/



typedef enum ValueType {
    VT_NULL = 'N',
    VT_STRING = 's',
    VT_NUMBER = 'n',
    VT_CONST = 'c',
    VT_FUNCTION = 'f',
    VT_CLASS = 'C',
    VT_OBJECT = 'o',
    VT_TOKEN = 't',
    VT_ARRAY = 'v',
    VT_INTERNAL_FUNCTION = 'i'
} ValueType;

typedef struct Value {
    ValueType type;
    char * name;
    double number;
    char * value;
    void * method;
    Token * token_start;
    Token * token_end;
    struct Value * next;
    struct Value * children;
    struct Value * prev;
    struct Value * parent;
    bool is_returned;
    void * ptr;
} Value;

typedef Value * (*Value_Function) (Value * args);

Value * Value_construct() {
    Value * value = (Value *)calloc(sizeof(Value),1);
    value->name = NULL;
    value->type = VT_NULL;
    value->value = NULL;
    value->number = 0;
    value->next = NULL;
    value->prev = NULL;
    value->children = NULL;
    value->parent = NULL;
    value->is_returned = false;
    value->ptr = NULL;
    return value;
}


Value * Value_push(Value * top, Value * value);


void Value_set_name(Value * value, char * name){
    if(value->name)
        free(value->name);
    value->name = strdup(name);
}

double Value_number(Value * value){
    if(value == NULL)
        return 0;
    double res = value->number;
    return res;
}
char * Value_string(Value * value){
    return value->value;
}


void Value_print(Value * value){
    if(value == NULL){
        printf("(null)");
    }else if(value->type == VT_NUMBER){
        printf("%f", Value_number(value));
    }else if(value->type == VT_STRING){
        printf("%s", Value_string(value));
    }else if(value->type == VT_TOKEN){
        Token * t = value->token_start;
        while(t && t != value->token_end){
            printf("%s ", t->content);
        }
        t = t->next;
    }

}

Value * Value_last(Value * value){
    if(!value)
        return NULL;
    while(value->next){
        
        value = value->next;
    }
    return value;
}
Value * Value_first(Value * value){
    if(!value)
        return NULL;
    while(value->prev)
        value = value->prev;
    return value;
}

Value * Value_get(Value * value, char * name){
    value = Value_last(value);
    while(value){
        if(value->name && !strcmp(value->name, name)){
            
            return value;

        }
        if(value->parent){
            return Value_get(value->parent, name);
        }
        
        value = value->prev;
    }
    return NULL;
}
Value * Value_get_or_create(Value * top, char * name){
    Value * value = Value_get(top, name);
    if(value == NULL){
        value = Value_construct();
        Value_push(top, value);
        Value_set_name(value, name);
    }
    return value;
}

Value * Value_push(Value * top, Value * value){
    if(value == NULL)
        return value;
    Value * last = Value_last(top);
    last->next = value;
    value->prev = last; 
    return value;
}

Value * Value_from_string(char * str){
    Value * value = Value_construct();
    value->type = VT_STRING;
    value->value =strdup(str);
    return value;
}

Value * Value_from_const(char * str){
    Value * value = Value_construct();
    value->type = VT_CONST;
    value->value = strdup(str);
    return value;
}

Value * Value_from_double(double dbl){
    Value * value = Value_construct();
    value->number = dbl;
    value->type = VT_NUMBER;
    return value;
}

Value * Value_from_number(char * number){
    Value * value = Value_construct();
    char * ptr;
    double nr = strtod(number,&ptr);
    //value->value = (char *)malloc(sizeof(double));
    value->type = VT_NUMBER;
    //*value->value = nr;
    value->number = nr;
    return value;
}

Value * Value_from_token(Token * token_start, Token * token_end){
    Value * value = Value_construct();
    value->type = VT_TOKEN;
    value->token_start = token_start;
    value->token_end = token_end;
    return value;
}

Token * Value_token(Value * value){
    return (Token *)value->value;
}

void _Value_dump(Value * value, unsigned int indent){
    unsigned int value_number = 0;
    while(value){
        value_number++;
        for(int i = 0; i < indent; i++){
            printf(" ");
        }
        if(value->name)
            printf("%s: ", value->name);
        else
            printf("%d: ", value_number);
        Value_print(value);
        printf("\n");
        if(value->children != NULL){
            _Value_dump(value->children, indent + 1);
        }
        value = value->next;
    }
}

void Value_dump(Value  * value){
    printf("[VALUE DUMP START]\n");
    _Value_dump(value, 0);
    printf("[VALUE DUMP END]\n");
}



void Value_destruct(Value * value){
    if(value->name != NULL)
        free(value->name);
    if(value->value != NULL)
        free(value->value);
    if(value->next)
        Value_destruct(value->next);
    free(value);
}

bool Value_is_truthy(Value * val){
    if(val->type == VT_NULL)
        return false;
    if(val->value != NULL){
        char * s = Value_string(val);
        return strlen(s) > 0;
    }
    return (int)val->number > 0;
}


Value * Value_cut(Value * value){
    if(value->prev){
        value->prev->next = value->next;
    }
    if(value->next){
        value->next->prev = value->prev;
    }
    return value;
}

Value * Value_children(Value * value){
    if(value->children == NULL){
        value->children = Value_construct();
        Value_set_name(value->children, "this");
        value->children->parent = value;
    }
    return value->children;
}

void Value_copy(Value * dest, Value * src){

    dest->number = src->number;
    dest->type = src->type;
    if(src->value){
        char * data = strdup(src->value);
        if(dest->value){
            free(dest->value);
        }
        dest->value = data;
    }
    dest->children = src->children;
    dest->ptr = src->ptr;

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
#endif