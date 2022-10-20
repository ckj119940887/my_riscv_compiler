#include "rvcc.h"
#include <stdio.h>

extern char *CurrentInput;
extern int Depth;

//expr = mul ("+" mul | "-" mul) *
//mul = primary("*" primary | "/" primary)
//primary = "(" expr ")" | num
//AST越往下优先级越高
//static Node *expr


int main(int Argc, char **Argv) {
  if(Argc != 2) {
    error("%s: invalid number of arguments", Argv[0]);
  }
  
  Token *Tok = tokenize(Argv[1]);

  Node *Nd = parse(Tok);

  codegen(Nd);

  return 0;
}
