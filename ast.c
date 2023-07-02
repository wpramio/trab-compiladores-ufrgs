// author: Willian R. Pramio (00262875)
// AST - Abstract Syntax Tree

#include "ast.h"

AST *astCreate(int type, HASH_NODE *symbol, AST* s0, AST* s1, AST* s2, AST* s3)
{
  AST* new_node;
  new_node = (AST*) calloc(1, sizeof(AST));
  new_node->type = type;
  new_node->symbol = symbol;
  new_node->son[0] = s0;
  new_node->son[1] = s1;
  new_node->son[2] = s2;
  new_node->son[3] = s3;

  return new_node;
}

void *astPrint(AST *node, int level)
{
  // won't print if it's a null pointer
  if (node != 0)
  {
    int i = 0;
    for(i=0; i < level; ++i)
      fprintf(stderr, "  ");
    fprintf(stderr, "AST(");
    switch (node->type)
    {
      case AST_SYMBOL: fprintf(stderr, "AST_SYMBOL"); break;
      case AST_ADD: fprintf(stderr, "AST_ADD"); break;
      case AST_SUB: fprintf(stderr, "AST_SUB"); break;
      default: fprintf(stderr, "AST_UNKOWN(%d)", node->type); break;
    }
    if (node->symbol != 0)
      fprintf(stderr, ", %s\n", node->symbol->text);
    else
      fprintf(stderr, ", 0\n");
    for(int i=0; i < MAX_SONS; ++i)
    {
      astPrint(node->son[i], level + 1);
    }
  }
}