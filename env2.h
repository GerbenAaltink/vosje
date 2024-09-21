#ifndef ENV_H
#define ENV_H
#include "value.h"

typedef struct Env {

    char * name;
    Value * value;

    struct Env * env;
    struct Env * next;
    struct Env * prev;
    struct Env * parent;

} Env;

Env * Env_construct(Env * parent){
    Env * env = (Env *)malloc(sizeof(Env));
    env->next = NULL;
    env->prev = NULL;
    env->parent = parent;
    env->env = NULL;
    env->name = NULL;
    env->value = NULL;
    return env;
}

Env * Env_first(Env * env){
    while(env->prev)
        env = env->prev;
    return env;
}

Env * Env_last(Env * env){
    while(env->next){
        env = env->next;
    }
    return env;
}

Env * Env_get(Env * env, char * name){
    while(env){
        if(env->name && !strcmp(env->name, name)){
            return env;
        }
        env = env->next;
    }
    return NULL;
}

Value * Env_get_value(Env * env, char * name){
    Env * res = Env_get(env, name);
    if(res)
    {
        return res->value;
    }
    return NULL;
}

Env * Env_set(Env * env, char * name, Value * value){
    Env * var = Env_get(env, name);
    if(var == NULL)
    {
        var = Env_construct(NULL);
        var->name = name;
        Env * last = Env_last(env);
        last->next = var;
        var->prev = last;
    }
    var->value = value;
    return var;
}

void Env_dump(Env * env, int indent){
    env = Env_first(env);
    while(env){
        for(int i =0; i < indent; i++){
            printf("-");
        }
        printf("env:");
        if(env->name){
            printf("%s: ", env->name);
        }else{
            printf("(null): ");
        }
        Value_print(env->value);
        if(env->env)
            Env_dump(env->env, indent+1);
        env = env->next;
    }
}

void Env_destruct(Env * env){
    if(env->env)
    {
        Env_destruct(env->env);
        env->env = NULL;
    }
    while(env){
        if(env->name != NULL){
            free(env->name);
        }
        if(env->value != NULL){
            Value_destruct(env->value);
        }
        env = env->next;
    }
}

#endif