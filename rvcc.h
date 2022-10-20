#ifndef __RVCC_H__
#define __RVCC_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef struct Type Type;
typedef struct Token Token; 
typedef struct Node Node;

//Token种类
typedef enum {
  TK_IDENT,   //标识符
  TK_PUNCT,   //操作符
  TK_KEYWORD, //关键字
  TK_STR,     //字符串字面量
  TK_NUM,     //数字
  TK_EOF,     //终止符
} TokenKind;

// 类型种类
typedef enum {
  TY_VOID,   // void类型
  TY_BOOL,   // _Bool布尔类型
  TY_CHAR,   // char字符类型
  TY_SHORT,  // short短整型
  TY_INT,    // int整型
  TY_LONG,   // long长整型
  TY_FLOAT,  // float类型
  TY_DOUBLE, // double类型
  TY_ENUM,   // enum枚举类型
  TY_PTR,    // 指针
  TY_FUNC,   // 函数
  TY_ARRAY,  // 数组
  TY_STRUCT, // 结构体
  TY_UNION,  // 联合体
} TypeKind;

//源文件信息
typedef struct {
  char *Name;
  int FileNo;
  char *Contents;
} File;

//数据类型
struct Type {
  TypeKind Kind;    //种类
  int Size;         //大小，sizeof返回的指
  int Align;        //对齐
  bool IsUnsigned;  //是否为无符号

  Type *Base;       //指向的类型

  Token *Name;      //类型的名称
  Token *NamePos;   //名称位置

  //数组
  int ArrayLen;     //数组长度

  //Member *Mems;     //结构体
  bool IsFlexible;  //是否为灵活的

  //函数类型
  Type *ReturnTy;   //函数返回类型
  Type *Params;     //形参
  bool IsVariadic;  //可变参数
  Type *Next;       //下一个类型
};

struct Token {
  TokenKind Kind; //种类
  Token * Next;   //指向下一个终结符

  int64_t Val;  //整数  
  double FVal;  //浮点
  char *Loc;    //在解析的字符串中的位置
  int Len;      //长度
  Type *Ty;     //TK_NUM or TK_STR
  // char *Str;    //字符串字面量,包括'\0'
  //
  // File *File;       //源文件
  // int LineNo;       //行号
  // bool AtBOL;       //终结符在行首时为true
  // bool HasSpace;    //终结符前是否有空格
  // //Hideset *Hideset; //用于宏展开
  // Token *Origin;    //宏展开前的原始终结符
};

// AST的节点种类
typedef enum {
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_NUM,       // 数字
  ND_NEG,       // 符号
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_EXPR_STMT, // 表达式语句
} NodeKind;

struct Node {
  NodeKind Kind;  //节点种类

  Node *Next;     //下一个表达式语句

  Node *LHS;      //左部
  Node *RHS;      //右部

  int64_t Val; // 存储ND_NUM种类的值
};

void verrorAt(char *Loc, char *Fmt, va_list VA);
void errorAt(char *Loc, char *Fmt, ...);
void errorTok(Token *Tok, char *Fmt, ...);
void error(char *Fmt, ...);

int getNumber(Token *Tok); 
bool equal(Token *Tok, char *Str); 
bool startsWith(char *Str, char *SubStr);
int readPunct(char *Ptr);
Token *skip(Token *Tok, char *Str); 
Token *newToken(TokenKind Kind, char *Start, char *End); 

void genExpr(Node *Nd); 

// program = stmt*
// stmt = exprStmt
// exprStmt = expr ";"
// expr = equality
// equality = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = unary ("*" unary | "/" unary)*
// unary = ("+" | "-") unary | primary
// primary = "(" expr ")" | num
Node *expr(Token **Rest, Token *Tok);
Node *equality(Token **Rest, Token *Tok);
Node *relational(Token **Rest, Token *Tok);
Node *add(Token **Rest, Token *Tok);
Node *mul(Token **Rest, Token *Tok);
Node *unary(Token **Rest, Token *Tok);
Node *primary(Token **Rest, Token *Tok);
Node *exprStmt(Token **Rest, Token *Tok); 

//入口函数
Token *tokenize(); 
Node *parse(Token *Tok);
void codegen(Node *Nd);
#endif
