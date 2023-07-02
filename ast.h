// author: Willian R. Pramio (00262875)
// AST - Abstract Syntax Tree

#ifndef AST_HEADER
#define AST_HEADER

#include "hash.h"

#define MAX_SONS 4

// Node types
#define AST_SYMBOL          1
#define AST_ADD             2
#define AST_SUB             3
#define AST_MULT            4
#define AST_DIV             5
#define AST_BIGG            6
#define AST_LESS            7
#define AST_LE              8
#define AST_GE              9
#define AST_EQ              10
#define AST_DIF             11
#define AST_AND             12
#define AST_OR              13
#define AST_NEG             14
#define AST_FUNC_CALL       15
#define AST_INPUT           16
#define AST_TYPE_CHAR       17
#define AST_TYPE_INT        18
#define AST_TYPE_REAL       19
#define AST_TYPE_BOOL       20
#define AST_FUNC_CALL_ARG   21
#define AST_VEC             22
#define AST_VAR_DEC         23
#define AST_VEC_DEC         24
#define AST_VEC_TAIL        25
#define AST_FUNC_DEC        26
#define AST_OUTPUT_ARG      27
#define AST_VAR_ASSIGN      28
#define AST_VEC_ASSIGN      29
#define AST_IF              30
#define AST_IF_ELSE         31
#define AST_IF_LOOP         32
#define AST_OUTPUT          33
#define AST_RETURN          34
#define AST_CMD_BLOCK       35
#define AST_CMD_LIST        36
#define AST_ARG_LIST        37
#define AST_ARG             38
#define AST_EMPTY_CMD       39
#define AST_GLOBAL_DEC_LIST 40

typedef struct ast_node
{
  int type;
  HASH_NODE *symbol;
  struct ast_node * son[MAX_SONS];
} AST;

AST *astCreate(int type, HASH_NODE *symbol, AST* s0, AST* s1, AST* s2, AST* s3);
void *astPrint(AST *node, int level);

#endif
