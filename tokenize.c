#include "rvcc.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

char *CurrentInput;

//输出出错位置，并退出
void verrorAt(char *Loc, char *Fmt, va_list VA) {
  fprintf(stderr, "%s\n", CurrentInput);

  //Loc是出错位置的指针，CurrentInput是当前输入的首地址
  int Pos = Loc - CurrentInput;
  //补齐为前Pos个位置为空格
  fprintf(stderr, "%*s", Pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, Fmt, VA);
  fprintf(stderr, "\n");
  va_end(VA);
  // 终止程序
  exit(1);
}

//字符解析出错
void errorAt(char *Loc, char *Fmt, ...) {
  va_list VA;
  va_start(VA, Fmt);
  verrorAt(Loc, Fmt, VA);
}

//Tok解析出错
void errorTok(Token *Tok, char *Fmt, ...) {
  va_list VA;
  va_start(VA, Fmt);
  verrorAt(Tok->Loc, Fmt, VA);
}

//输出出错信息
void error(char *Fmt, ...) {
  va_list VA;
  // VA获取Fmt后面所有参数
  va_start(VA, Fmt);
  // Vfprintf可以输出va_list类型的参数
  vfprintf(stderr, Fmt, VA);
  // 结尾加上换行符
  fprintf(stderr, "\n");
  // 清除VA
  va_end(VA);
  // 终止程序
  exit(1);
}

int getNumber(Token *Tok) {
  if(Tok->Kind != TK_NUM)
    errorTok(Tok, "expect a number");
  return Tok->Val;
}

//判断Str是否以SubStr开头
bool startsWith(char *Str, char *SubStr) {
  return strncmp(Str, SubStr, strlen(SubStr)) == 0;
}

//判断操作符是一字节还是两字节
int readPunct(char *Ptr) {
  if(startsWith(Ptr, "==") || startsWith(Ptr, "!=") || 
     startsWith(Ptr, "<=") || startsWith(Ptr, ">="))
    return 2;

  return ispunct(*Ptr) ? 1 : 0;
}

bool equal(Token *Tok, char *Str) {
  //比较字符串LHS,RHS的前N位, Str应该大于Tok->Len
  //int memcmp(const void *str1, const void *str2, size_t n);
  //其功能是把存储区 str1 和存储区 str2 的前 n 个字节进行比较。
  //memcmp按照字典序进行比较，LHS<RHS返回负值，LHS>RHS返回正值，LHS=RHS返回0
  //第Tok->Len位以'\0'为结尾，即操作符必须是'+,++', 而不能是"+ ,++1"等
  return memcmp(Tok->Loc, Str, (unsigned long int)Tok->Len) == 0 && Str[Tok->Len] == '\0';
}

//跳过Str字符
Token *skip(Token *Tok, char *Str) {
  if(!equal(Tok, Str))
    errorTok(Tok, "expect %s");

  return Tok->Next;
}

Token *newToken(TokenKind Kind, char *Start, char *End) {
  Token *Tok = calloc(1, sizeof(Token));

  Tok->Kind = Kind;
  Tok->Loc = Start;
  Tok->Len = End - Start;

  return Tok;
}

// 终结符解析
Token *tokenize(char *P) {
  CurrentInput = P;
  Token Head = {};
  Token *Cur = &Head;

  while (*P) {
    // 跳过空白和回车
    if(isspace(*P)) {
      ++P;
      continue;
    }

    // 解析数字
    if(isdigit(*P)) {
      //Head不用来存储信息，仅用来表示链表入口
      Cur->Next =  newToken(TK_NUM, P, P);

      Cur = Cur->Next;
      const char *OldPtr = P;
      Cur->Val = strtoul(P, &P, 10);
      Cur->Len = P - OldPtr;
      continue;
    }

    int PunctLen = readPunct(P);
    if(PunctLen) {
      Cur->Next = newToken(TK_PUNCT, P, P + PunctLen);
      Cur = Cur->Next;
      P = P + PunctLen;
      continue;
    }

    errorAt(P, "invalid token");
  }

  Cur->Next = newToken(TK_EOF, P, P);

  return Head.Next;
}

