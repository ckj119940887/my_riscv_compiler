#include "rvcc.h"
#include <assert.h>

//记录栈的深度
int Depth;

void push(void) {
  printf("  addi sp, sp, -8\n");
  printf("  sd a0, 0(sp)\n");
  Depth++;
}

void pop(char *Reg) {
  printf("  ld %s, 0(sp)\n", Reg);
  printf("  addi sp, sp, 8\n");
  Depth--;
}

void genExpr(Node *Nd) {
  switch (Nd->Kind) {
    case ND_NUM:
      printf("  li a0, %ld\n", Nd->Val);
      return;
    case ND_NEG:
      //对于一元操作符,没有RHS
      genExpr(Nd->LHS);
      printf("  neg a0, a0\n");
      return;
    default:
      break;
  }

  //递归右子树
  genExpr(Nd->RHS);
  //将结果压栈
  push();
  //递归左子树
  genExpr(Nd->LHS);
  //将结果弹栈到a1
  pop("a1");

  switch (Nd->Kind) {
    case ND_ADD: 
      printf("  add a0, a0, a1\n"); 
      return;
    case ND_SUB:
      printf("  sub a0, a0, a1\n"); 
      return;
    case ND_MUL:
      printf("  mul a0, a0, a1\n"); 
      return;
    case ND_DIV:
      printf("  div a0, a0, a1\n"); 
      return;
    case ND_EQ:
    case ND_NE:
      //使用异或来完成相等比较
      printf("  xor a0, a0, a1\n");
      
      if(Nd->Kind == ND_EQ)
        printf("  seqz a0, a0\n");
      else
        printf("  snez a0, a0\n");
      return;
    case ND_LT:
      printf("  slt a0, a0, a1\n");
      return; 
    case ND_LE:
      // a0 <= a1 等价于 a1 < a0, a0 = a1 ^ 1
      printf("  slt a0, a1, a0\n");
      printf("  xori a0, a0, 1\n");
      return;
    default:
      break;
  }

  error("invalid expression");
}

void genStmt(Node *Nd) {
  if(Nd->Kind == ND_EXPR_STMT) {
    genExpr(Nd->LHS); 
    return;
  }

  error("invalid statement");
}

//代码生成入口函数，包含代码块的基础信息
void codegen(Node *Nd) {
  
  printf(".global main\n");
  printf("main:\n");

  for(Node *N = Nd; N; N = N->Next) {
    genStmt(N);
    assert(Depth == 0);
  }

  printf("  ret\n");
}
