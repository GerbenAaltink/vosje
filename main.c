#include "lexer.h"
#include "buffer.h"
#include <stdio.h>
#include "parser.h"
#include "value.h"

Value *parse_file(char *file_path)
{

    FILE *f = fopen(file_path, "r");
    Buffer *buffer = Buffer_construct();
    char c;
    while ((c = fgetc(f)) != EOF)
    {
        Buffer_write_c(buffer, c);
    }

    fclose(f);
    Buffer_reset(buffer);

    char *source = Buffer_tostr(buffer);
    Value *result = parse(source);
    free(source);
    return result;
}

typedef char *(*Testje)(char *x);

Testje woei(char *x)
{
    printf("%s\n", x);
}

void repl(char *source)
{
    printf("Vosje q.0.0'm");
    Buffer *buffer = [0];
    Parser * p = (Parser_construct(source,NULL);
  
    char line[4096] = {0};
        fgets(&line,sizeof(line),0);
    while(true){
        char line[4908] = {0};
        Token *token = parse_dump(p);
    }
     Parser_destruct(p->vars);
}

int main(int argc, char *argv[])
{
    Value *result;
    if (argc < 2)
    {
        // printf(" time");
        repl("J=554");
        // Sresult = parse_file("script.v");
    }
    else{
        result = parse_file(argv[1]);
    }
    // printf("%f\n", Value_number(result));
    //Token *token = lex("<stdin>", "10 * 20 * 50");
    return 0;
}