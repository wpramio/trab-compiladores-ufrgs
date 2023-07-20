// author: Willian R. Pramio (00262875)
// AST - Abstract Syntax Tree

#ifndef AST_HEADER
#define AST_HEADER

#include "hash.h"

#define MAX_SONS 4

// Node types
#define AST_SYMBOL          1
#define AST_SYMBOL_CHAR     2
#define AST_SYMBOL_STRING   3
#define AST_GLOBAL_DEC_LIST 4
#define AST_VEC_ACCESS      5
#define AST_ADD             6
#define AST_SUB             7
#define AST_MULT            8
#define AST_DIV             9
#define AST_BIGG            10
#define AST_LESS            11
#define AST_LE              12
#define AST_GE              13
#define AST_EQ              14
#define AST_DIF             15
#define AST_AND             16
#define AST_OR              17
#define AST_NEG             18
#define AST_NESTED_EXPR     19
#define AST_FUNC_CALL       20
#define AST_FUNC_CALL_ARG   21
#define AST_INPUT           22
#define AST_TYPE_CHAR       23
#define AST_TYPE_INT        24
#define AST_TYPE_REAL       25
#define AST_TYPE_BOOL       26
#define AST_VAR_DEC         27
#define AST_VEC_DEC         28
#define AST_VEC_TAIL        29
#define AST_FUNC_DEC        30
#define AST_ARG_LIST        31
#define AST_ARG             32
#define AST_CMD_BLOCK       33
#define AST_CMD_LIST        34
#define AST_VAR_ASSIGN      35
#define AST_VEC_ASSIGN      36
#define AST_IF              37
#define AST_IF_ELSE         38
#define AST_IF_LOOP         39
#define AST_OUTPUT          40
#define AST_RETURN          41
#define AST_EMPTY_CMD       42
#define AST_OUTPUT_ARG      43
#define AST_VAR_ACCESS      44

typedef struct ast_node
{
  int type;
  int datatype;
  HASH_NODE *symbol;
  struct ast_node * son[MAX_SONS];
} AST;

AST *astCreate(int type, HASH_NODE *symbol, AST* s0, AST* s1, AST* s2, AST* s3);
void *astPrint(AST *node, int level);
void *astDecompile(char* filename_out);
void astDecompileNode(AST *node, FILE* file_out);

#endif
