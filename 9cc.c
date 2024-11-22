#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind; //トークンの型
  Token *next;    //次の入力トークン
  int val;        //kindがTK_NUMの場合、その数値
  char *str;      //トークン文字列
  int len;        //トークンの長さ
};

//抽象構文木のノードの型
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
} NodeKind;

typedef struct Node Node;

Node *add();
bool consume(char *op);
Node *unary();
Node *mul();
Node *relational();
Node *equality();


//抽象構文木のノードの型
struct Node {
  NodeKind kind; //ノードの型
  Node *lhs; //左辺
  Node *rhs; //右辺
  int val; //kindがNO_NUMの場合のみ使う
};

//現在注目しているトークン
Token *token;


//入力プログラミング
char *user_input;

//エラー箇所を報告する
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");//pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

//次のトークンが期待している記号のときには、トークンを1つ読み進めて
//真を返す。それ以外の場合には偽をけ返す。
bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      //メモリーコンペア  
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

//次の次のトークンが期待している記号のときには、トークンを1つ読み進める。
//それ以外の場合にはエラーを報告する。
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error_at(token->str, "'%c'ではありません", op);
  token = token->next;
}

//次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
//ソレ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;

}

bool at_eof() {
  return token->kind == TK_EOF;
}

//新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

//入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    //空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur,p);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    // printf("foo");
    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}


Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *expr() {
  return equality();
}

Node *primary() {
  //次のトークンが"("なら、 "(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  //そうでなければ数値のはず
  return new_node_num(expect_number());
}

Node *unary() {
  if (consume('+'))
    return primary();
  if (consume('-'))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *mul() {
  Node *node = unary();

  for(;;) {
    if(consume('*'))
      node = new_node(ND_MUL, node, unary());
    else if (consume('/'))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume('+'))
      node = new_node(ND_ADD, node, mul());
    else if (consume('-'))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume('<'))
      node = new_node(ND_LT, node, add());
    else if (consume('<='))
      node = new_node(ND_LTE, node, add());
    else if (consume('>'))
      node = new_node(ND_LT, add(), node);
    else if (consume('>='))
      node = new_node(ND_LTE, add(), node);
    else
      return node;
  }
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume('=='))
      node = new_node(ND_EQ, node, relational());
    else if (consume('!='))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

int global_register_count = 0;
void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf(" mov w%d, %d\n", global_register_count, node->val);
    printf(" str w%d, [sp, #-16]!\n", global_register_count);
    // global_register_count;
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf(" ldr w1, [sp], #16\n");
  printf(" ldr w0, [sp], #16\n");

  switch (node->kind) {
    case ND_ADD:
      printf(" add w0, w0, w1\n");
      break;
    case ND_SUB:
      printf(" sub w0, w0, w1\n");
      break;
    case ND_MUL:
      printf(" mul w0, w0, w1\n");
      break;
    case ND_DIV:
      printf(" sdiv w0, w0, w1\n");
      break;
  }
  printf(" str w0, [sp, #-16]!\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  //トークナイズしてパースする
  user_input = argv[1];
  token = tokenize(argv[1]);
  Node *node = expr();

  //アセンブリの前半部分を出力
  printf(".globl main\n");
  printf("main:\n");

  //抽象構文木を下りながらコード生成
  gen(node);

  //スタックトップに式全体の値が残っているはずなので
  printf(" ret\n");
  return 0;
}
