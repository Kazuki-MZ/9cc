#include <stdbool.h>
#include <stdio.h>

typedef struct Node Node;

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
  TK_IDENT, // 識別子
} TokenKind;

typedef struct Token Token;

typedef enum {
    ND_ADD, //+
    ND_SUB, //-
    ND_MUL, //*
    ND_DIV, // /
    ND_NUM, // 整数
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LTE, // <=
    ND_ASSIGN, // =
} NodeKind;

typedef struct Node Node;

// トークン型
struct Token {
  TokenKind kind; //トークンの型
  Token *next;    //次の入力トークン
  int val;        //kindがTK_NUMの場合、その数値
  char *str;      //トークン文字列
  int len;        //トークンの長さ
};

//抽象構文木のノードの型
struct Node {
  NodeKind kind; //ノードの型
  Node *lhs; //左辺
  Node *rhs; //右辺
  int val; //kindがNO_NUMの場合のみ使う
};

Node *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
bool consume(char *op);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
Token *tokenize(char *p);

void gen(Node *node);

extern char *user_input;
extern Token *token;