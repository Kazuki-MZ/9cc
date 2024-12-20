#include "9cc.h"

int global_register_count = 0;
void gen(Node *node) {
  printf("// kind %d\n", node->kind);
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
    case ND_EQ:
      printf(" cmp w0, w1\n");
      printf(" cset w0, eq\n");
      break;
    case ND_NE:
      printf(" cmp w0, w1\n");
      printf(" cset w0, ne\n");
      break;
    case ND_LT:
      printf(" cmp w0, w1\n");
      printf(" cset w0, lt\n");
      break;
    case ND_LTE:
      printf(" cmp w0, w1\n");
      printf(" cset w0, le\n");
      break;
    case ND_NUM:
      // no op
      break;
  }
  printf(" str w0, [sp, #-16]!\n");
}