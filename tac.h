// author: Willian R. Pramio (00262875)
// TAC - Three Address Code

#ifndef TAC_HEADER
#define TAC_HEADER

#include "hash.h"
#include "ast.h"

#define MAX_SONS 4

// Node types
enum TACtypes {
  TAC_SYMBOL, TAC_ADD, TAC_SUB, TAC_MULT, TAC_DIV, TAC_BIGG, TAC_LESS, TAC_LE, TAC_GE,
  TAC_EQ, TAC_DIF, TAC_AND, TAC_OR, TAC_NEG, TAC_LABEL, TAC_VEC_ACCESS,
  TAC_TYPE_CHAR, TAC_TYPE_INT, TAC_TYPE_REAL, TAC_TYPE_BOOL, TAC_VAR_DEC, TAC_VEC_DEC,
  TAC_BEGIN_FUNC, TAC_END_FUNC, TAC_ARG, TAC_IF, TAC_IF_ELSE, TAC_IF_LOOP,
  TAC_JUMP, TAC_JFALSE, TAC_MOVE, TAC_COPY, TAC_COPY_VEC,
  TAC_FUNC_CALL, TAC_FUNC_CALL_ARG, TAC_RETURN, TAC_OUTPUT, TAC_INPUT, TAC_UNKNOWN
};

// #define TAC_SYMBOL          1
// #define TAC_SYMBOL_CHAR     2
// #define TAC_SYMBOL_STRING   3
// #define TAC_GLOBAL_DEC_LIST 4
// #define TAC_VEC_ACCESS      5
// #define TAC_ADD             6
// #define TAC_SUB             7
// #define TAC_MULT            8
// #define TAC_DIV             9
// #define TAC_BIGG            10
// #define TAC_LESS            11
// #define TAC_LE              12
// #define TAC_GE              13
// #define TAC_EQ              14
// #define TAC_DIF             15
// #define TAC_AND             16
// #define TAC_OR              17
// #define TAC_NEG             18
// #define TAC_NESTED_EXPR     19
// #define TAC_FUNC_CALL       20
// #define TAC_FUNC_CALL_ARG   21
// #define TAC_INPUT           22
// #define TAC_TYPE_CHAR       23
// #define TAC_TYPE_INT        24
// #define TAC_TYPE_REAL       25
// #define TAC_TYPE_BOOL       26
// #define TAC_VAR_DEC         27
// #define TAC_VEC_DEC         28
// #define TAC_VEC_TAIL        29
// #define TAC_FUNC_DEC        30
// #define TAC_ARG_LIST        31
// #define TAC_ARG             32
// #define TAC_CMD_BLOCK       33
// #define TAC_CMD_LIST        34
// #define TAC_VAR_ASSIGN      35
// #define TAC_VEC_ASSIGN      36
// #define TAC_IF              37
// #define TAC_IF_ELSE         38
// #define TAC_IF_LOOP         39
// #define TAC_OUTPUT          40
// #define TAC_RETURN          41
// #define TAC_EMPTY_CMD       42
// #define TAC_OUTPUT_ARG      43
// #define TAC_VAR_ACCESS      44

typedef struct tac_node
{
  int opcode;
  HASH_NODE * res;
  HASH_NODE * op1;
  HASH_NODE * op2;
  struct tac_node * prev;
  struct tac_node * next;
} TAC;

TAC *tacCreate(int opcode, HASH_NODE* res, HASH_NODE* op1, HASH_NODE* op2);
TAC *tacJoin(TAC* tac1, TAC* tac2);
TAC *generateCode(AST* node);
void tacPrint(TAC *tac);
void tacPrintBackwards(TAC *tac);
HASH_NODE *makeTemp();
HASH_NODE *makeLabel();

#endif
