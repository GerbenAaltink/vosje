#ifndef ENV_H
#define ENV_H
#include "token.h"
#include "value.h"

typedef struct Env{
    Token * token;
    Value * vars;
} Env;


#endif 