#include "lexer.h"
#include "token.h"
#include "buffer.h"
#include "parser.h"

Value * vparse_scope(Parser * parser){


}

Value * vparse_term(char * content)

Value * vparse(char * content){
    Parser * parser = Parser_construct(content);
}




int main () {
    Token * t = lex("class X{prop=\"val\"}}d=X()print(d.val)",Buffer_s("class X{prop=\"val\"}}d=X()print(d.val)"));

    Token_dump(t);

}