// author: Willian R. Pramio (00262875)
// TAC - Three Address Code

#ifndef TAC_HEADER
#define TAC_HEADER

#include "hash.h"
#include "ast.h"

// Node types
enum TACtypes {
  TAC_SYMBOL, TAC_ADD, TAC_SUB, TAC_MULT, TAC_DIV, TAC_BIGG, TAC_LESS, TAC_LE, TAC_GE,
  TAC_EQ, TAC_DIF, TAC_AND, TAC_OR, TAC_NEG, TAC_LABEL, TAC_VEC_ACCESS, TAC_VEC_INIT_VALUE,
  TAC_TYPE_CHAR, TAC_TYPE_INT, TAC_TYPE_REAL, TAC_TYPE_BOOL, TAC_VAR_DEC, TAC_VEC_DEC,
  TAC_BEGIN_FUNC, TAC_END_FUNC, TAC_ARG, TAC_IF, TAC_IF_ELSE, TAC_IF_LOOP,
  TAC_JUMP, TAC_JFALSE, TAC_MOVE, TAC_COPY, TAC_COPY_VEC, TAC_OUTPUT, TAC_OUTPUT_ARG,
  TAC_INPUT, TAC_FUNC_CALL, TAC_FUNC_CALL_ARG, TAC_RETURN, TAC_UNKNOWN
};

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
TAC* tacInvert(TAC* tac);
TAC *generateCode(AST* node);
void tacPrint(TAC *tac);
void tacPrintBackwards(TAC *tac);
void tacPrintList(TAC* tac);
HASH_NODE *makeTemp();
HASH_NODE *makeLabel();

#endif
