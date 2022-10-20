#include "rvcc.h"

//新建一个节点
Node *newNode(NodeKind Kind) {
  Node *Nd = calloc(1, sizeof(Node));
  Nd->Kind = Kind;
  return Nd;
}

//新建一个单叉树
Node *newUnary(NodeKind Kind, Node *Expr) {
  Node *Nd = newNode(Kind);
  Nd->LHS = Expr;
  return Nd;
}

//新建一个二叉树节点
Node *newBinary(NodeKind Kind, Node *LHS, Node *RHS) {
  Node *Nd = newNode(Kind);
  Nd->LHS = LHS;
  Nd->RHS = RHS;
  return Nd;
}

//创建一个数字节点
Node *newNum(int64_t Val) {
  Node *Nd = newNode(ND_NUM);
  Nd->Val = Val;
  return Nd;
}

//解析相等性
Node *equality(Token **Rest, Token *Tok) {
  Node *Nd = relational(&Tok, Tok);

  while (true) {
    // ("==" | "!=")
    if (equal(Tok, "==")) {
      Nd = newBinary(ND_EQ, Nd, relational(&Tok, Tok->Next));
      continue;
    }

    if (equal(Tok, "!=")) {
      Nd = newBinary(ND_NE, Nd, relational(&Tok, Tok->Next));
      continue;
    }

    *Rest = Tok;
    return Nd;
  }
}

//解析比较关系
Node *relational(Token **Rest, Token *Tok) {
  Node *Nd = add(&Tok, Tok);

  // <,>,<=,>=
  while (true) {
    if (equal(Tok, "<")) {
      Nd = newBinary(ND_LT, Nd, add(&Tok, Tok->Next));
      continue;
    }

    if (equal(Tok, "<=")) {
      Nd = newBinary(ND_LE, Nd, add(&Tok, Tok->Next));
      continue;
    }

    //没有ND_GE和ND_GT,只是把比较关系两边的操作数对调
    if (equal(Tok, ">")) {
      Nd = newBinary(ND_LT, add(&Tok, Tok->Next), Nd);
      continue;
    }

    if (equal(Tok, ">=")) {
      Nd = newBinary(ND_LE, add(&Tok, Tok->Next), Nd);
      continue;
    }

    *Rest = Tok;
    return Nd;
  }
}

// 解析表达式
// expr = equality
Node *expr(Token **Rest, Token *Tok) {
  return equality(Rest, Tok);
}

//解析加减
//add = mul ("+" mul | "-" mul) 
Node *add(Token **Rest, Token *Tok) {
  //mul
  Node *Nd = mul(&Tok, Tok);

  while (true) {
    if(equal(Tok, "+")) {
      Nd = newBinary(ND_ADD, Nd, mul(&Tok, Tok->Next));
      continue;
    }

    if(equal(Tok, "-")) {
      Nd = newBinary(ND_SUB, Nd, mul(&Tok, Tok->Next));
      continue;
    }

    *Rest = Tok;
    return Nd;
  }
} 

//解析乘除
Node *mul(Token **Rest, Token *Tok) {
  Node *Nd = unary(&Tok, Tok);

  while (true) {
    if(equal(Tok, "*")) {
      Nd = newBinary(ND_MUL, Nd, unary(&Tok, Tok->Next));
    }

    if(equal(Tok, "/")) {
      Nd = newBinary(ND_DIV, Nd, unary(&Tok, Tok->Next));
    }

    *Rest = Tok;
    return Nd;
  }
}

//解析一元运算符
Node *unary(Token **Rest, Token *Tok) {
  if(equal(Tok, "+")) 
    return unary(Rest, Tok->Next);
  
  if(equal(Tok, "-")) 
    return newUnary(ND_NEG, unary(Rest, Tok->Next));

  return primary(Rest, Tok);
}

//解析括号和数字
Node *primary(Token **Rest, Token *Tok) {
  if(equal(Tok, "(")) {
    Node *Nd = expr(&Tok, Tok->Next);
    *Rest = skip(Tok, ")");
    return Nd;
  }

  if(Tok->Kind == TK_NUM) {
    Node *Nd = newNum(Tok->Val);
    *Rest = Tok->Next;
    return Nd;
  }

  errorTok(Tok, "expected an expression");
  return NULL;
}

//解析语句
Node *stmt(Token **Rest, Token *Tok) {
  return exprStmt(Rest, Tok);
}

//解析表达式语句
Node *exprStmt(Token **Rest, Token *Tok) {
  Node *Nd = newUnary(ND_EXPR_STMT, expr(&Tok, Tok));
  *Rest = skip(Tok, ";");
  return Nd;
}

//语法分析入口函数
Node *parse(Token *Tok) {
  Node Head = {};
  Node *Cur = &Head;

  while(Tok->Kind != TK_EOF) {
    Cur->Next = stmt(&Tok, Tok);
    Cur = Cur->Next;
  }

  return Head.Next;
}
