// author: Willian R. Pramio (00262875)
// TAC - Three Address Code

#include "tac.h"

int temp_count = 0;
int label_count = 0;

TAC* tacCreate(int opcode, HASH_NODE* res, HASH_NODE* op1, HASH_NODE* op2)
{
  TAC* new_tac;
  new_tac = (TAC*) calloc(1, sizeof(TAC));
  new_tac->opcode = opcode;
  new_tac->res = res;
  new_tac->op1 = op1;
  new_tac->op2 = op2;

  return new_tac;
}

void tacPrint(TAC* tac)
{
  if (tac != 0 && tac->opcode != TAC_SYMBOL)
  {
    fprintf(stderr, "TAC(");
    switch (tac->opcode)
    {
      case TAC_SYMBOL: fprintf(stderr, "TAC_SYMBOL"); break;
      case TAC_ADD: fprintf(stderr, "TAC_ADD"); break;
      case TAC_SUB: fprintf(stderr, "TAC_SUB"); break;
      case TAC_MULT: fprintf(stderr, "TAC_MULT"); break;
      case TAC_DIV: fprintf(stderr, "TAC_DIV"); break;
      case TAC_BIGG: fprintf(stderr, "TAC_BIGG"); break;
      case TAC_LESS: fprintf(stderr, "TAC_LESS"); break;
      case TAC_LE: fprintf(stderr, "TAC_LE"); break;
      case TAC_GE: fprintf(stderr, "TAC_GE"); break;
      case TAC_EQ: fprintf(stderr, "TAC_EQ"); break;
      case TAC_DIF: fprintf(stderr, "TAC_DIF"); break;
      case TAC_AND: fprintf(stderr, "TAC_AND"); break;
      case TAC_OR: fprintf(stderr, "TAC_OR"); break;
      case TAC_NEG: fprintf(stderr, "TAC_NEG"); break;
      case TAC_COPY: fprintf(stderr, "TAC_COPY"); break;
      case TAC_COPY_VEC: fprintf(stderr, "TAC_COPY_VEC"); break;
      case TAC_FUNC_CALL: fprintf(stderr, "TAC_FUNC_CALL"); break;
      case TAC_FUNC_CALL_ARG: fprintf(stderr, "TAC_FUNC_CALL_ARG"); break;
      case TAC_INPUT: fprintf(stderr, "TAC_INPUT"); break;
      case TAC_TYPE_CHAR: fprintf(stderr, "TAC_TYPE_CHAR"); break;
      case TAC_TYPE_INT: fprintf(stderr, "TAC_TYPE_INT"); break;
      case TAC_TYPE_REAL: fprintf(stderr, "TAC_TYPE_REAL"); break;
      case TAC_TYPE_BOOL: fprintf(stderr, "TAC_TYPE_BOOL"); break;
      case TAC_VEC_ACCESS: fprintf(stderr, "TAC_VEC_ACCESS"); break;
      case TAC_VAR_ACCESS: fprintf(stderr, "TAC_VAR_ACCESS"); break;
      case TAC_VAR_DEC: fprintf(stderr, "TAC_VAR_DEC"); break;
      case TAC_VEC_DEC: fprintf(stderr, "TAC_VEC_DEC"); break;
      case TAC_VEC_INIT_VALUE: fprintf(stderr, "TAC_VEC_INIT_VALUE"); break;
      case TAC_BEGIN_FUNC: fprintf(stderr, "TAC_BEGIN_FUNC"); break;
      case TAC_END_FUNC: fprintf(stderr, "TAC_END_FUNC"); break;
      case TAC_ARG: fprintf(stderr, "TAC_ARG"); break;
      case TAC_IF: fprintf(stderr, "TAC_IF"); break;
      case TAC_IF_ELSE: fprintf(stderr, "TAC_IF_ELSE"); break;
      case TAC_IF_LOOP: fprintf(stderr, "TAC_IF_LOOP"); break;
      case TAC_JFALSE: fprintf(stderr, "TAC_JFALSE"); break;
      case TAC_JUMP: fprintf(stderr, "TAC_JUMP"); break;
      case TAC_LABEL: fprintf(stderr, "TAC_LABEL"); break;
      case TAC_OUTPUT: fprintf(stderr, "TAC_OUTPUT"); break;
      case TAC_OUTPUT_ARG: fprintf(stderr, "TAC_OUTPUT_ARG"); break;
      case TAC_RETURN: fprintf(stderr, "TAC_RETURN"); break;
      default: fprintf(stderr, "TAC_UNKOWN(%d)", tac->opcode); break;
    }
    fprintf(stderr, ",%s", (tac->res) ? tac->res->text : "");
    fprintf(stderr, ",%s", (tac->op1) ? tac->op1->text : "");
    fprintf(stderr, ",%s", (tac->op2) ? tac->op2->text : "");
    fprintf(stderr, ");\n");
  }
}

void tacPrintBackwards(TAC* tac)
{
  if (tac != 0)
  {
    tacPrintBackwards(tac->prev);
    tacPrint(tac);
  }
}

void tacPrintList(TAC* tac)
{
  if (tac != 0)
  {
    tacPrint(tac);
    tacPrintList(tac->next);
  }
}

// Appends tac1 to the end of 'prev' list of tac2
TAC* tacJoin(TAC* tac1, TAC* tac2)
{
  if (tac1 == 0) return tac2;
  if (tac2 == 0) return tac1;

  TAC* ptr;
  for (ptr = tac2; ptr->prev != 0; ptr = ptr->prev)
    ;
  ptr->prev = tac1;

  return tac2;
}

TAC* tacInvert(TAC* tac)
{
  TAC* ret = tac;
  if (tac != 0 && tac->prev != 0)
  {
    tac->prev->next = tac;
    ret = tacInvert(tac->prev);
  }

  return ret;
}


HASH_NODE* makeTemp()
{
  char temp_name[15];
  sprintf(temp_name, "_myTemp_%02d", temp_count);
  temp_count++;

  return hashInsert(temp_name, SYMBOL_VARIABLE, 0);
}

HASH_NODE* makeLabel()
{
  char label_name[15];
  sprintf(label_name, "_myLabel_%02d", label_count);
  label_count++;

  return hashInsert(label_name, SYMBOL_LABEL, 0);
}

// c0: expression, c1: [then] command
// tac_if
// c0
// jf -> label
// c1
// label
TAC* makeIf(TAC* c0, TAC* c1)
{
  HASH_NODE *label = makeLabel();
  TAC *tac_jfalse = tacCreate(TAC_JFALSE, label, c0 ? c0->res : 0, 0);
  tac_jfalse->prev = tacJoin(tacCreate(TAC_IF, 0, 0, 0), c0);
  TAC *tac_label = tacCreate(TAC_LABEL, label, 0, 0);
  tac_label->prev = c1;

  return tacJoin(tac_jfalse, tac_label);
}

// c0: expression, c1: [then] command, c2: [else] command
// c0
// jf -> label_else
// c1
// jump -> label_end
// label_else
// c2
// label_end
TAC* makeIfElse(TAC* c0, TAC* c1, TAC* c2)
{
  HASH_NODE *label_else = makeLabel();
  TAC *tac_jfalse = tacCreate(TAC_JFALSE, label_else, c0 ? c0->res : 0, 0);
  TAC *tac_label_else = tacCreate(TAC_LABEL, label_else, 0, 0);
  HASH_NODE *label_end = makeLabel();
  TAC *tac_jump = tacCreate(TAC_JUMP, label_end, 0, 0);
  TAC *tac_label_end = tacCreate(TAC_LABEL, label_end, 0, 0);
  tac_jfalse->prev = tacJoin(tacCreate(TAC_IF_ELSE, 0, 0, 0), c0);
  tac_jump->prev = c1;
  tac_label_else->prev = tac_jump;
  tac_label_end->prev = c2;

  return tacJoin(tacJoin(tac_jfalse, tac_label_else), tac_label_end);
}

// c0: expression, c1: [loop] command
// label_loop
// c0
// jf -> label_end
// c1
// jump -> label_loop
// label_end
TAC* makeIfLoop(TAC* c0, TAC* c1)
{
  HASH_NODE *label_loop = makeLabel();
  TAC *tac_jump = tacCreate(TAC_JUMP, label_loop, 0, 0);
  TAC *tac_label_loop = tacCreate(TAC_LABEL, label_loop, 0, 0);
  HASH_NODE *label_end = makeLabel();
  TAC *tac_jfalse = tacCreate(TAC_JFALSE, label_end, c0 ? c0->res : 0, 0);
  TAC *tac_label_end = tacCreate(TAC_LABEL, label_end, 0, 0);
  tac_label_loop->prev = tacCreate(TAC_IF_LOOP, 0, 0, 0);
  tac_jfalse->prev = tacJoin(tac_label_loop, c0);
  tac_jump->prev = c1;
  tac_label_end->prev = tac_jump;

  return tacJoin(tac_jfalse, tac_label_end);
}

TAC* makeBinaryOperation(int opcode, TAC* c0, TAC* c1)
{
  return tacJoin(tacJoin(c0, c1), tacCreate(opcode, makeTemp(), c0 ? c0->res : 0, c1 ? c1->res : 0));
}

// c0: kw_type, c1: symbol, c2: arg_list (or null), c3: cmd_block
// begin_func
// c2
// c3
// end_func
TAC* makeFuncDec(TAC* c0, TAC* c1, TAC* c2, TAC* c3)
{
  TAC *tac_begin_func = tacCreate(TAC_BEGIN_FUNC, c1->res, 0, 0);
  TAC *tac_end_func = tacCreate(TAC_END_FUNC, c1->res, 0, 0);

  return tacJoin(tacJoin(tacJoin(tac_begin_func, c2), c3), tac_end_func);
}

TAC* generateCode(AST* node)
{
  TAC* res = 0;

  // null pointer check
  if (node != 0)
  {
    TAC* code[MAX_SONS];
    // process sons (sub-trees) first
    for(int i=0; i < MAX_SONS; ++i)
      code[i] = generateCode(node->son[i]);

    switch (node->type)
    {
      case AST_SYMBOL:
      case AST_SYMBOL_CHAR:
      case AST_SYMBOL_STRING:
        res = tacCreate(TAC_SYMBOL, node->symbol, 0, 0);
        break;
      case AST_VAR_ACCESS:
        res = tacCreate(TAC_VAR_ACCESS, node->symbol, 0, 0);
        break;
      case AST_VEC_ACCESS:
        res = tacJoin(code[1], tacCreate(TAC_VEC_ACCESS, makeTemp(), code[0] ? code[0]->res : 0, code[1] ? code[1]->res : 0));
        break;
      case AST_ADD:
        res = makeBinaryOperation(TAC_ADD, code[0], code[1]);
        break;
      case AST_SUB:
        res = makeBinaryOperation(TAC_SUB, code[0], code[1]);
        break;
      case AST_MULT:
        res = makeBinaryOperation(TAC_MULT, code[0], code[1]);
        break;
      case AST_DIV:
        res = makeBinaryOperation(TAC_DIV, code[0], code[1]);
        break;
      case AST_BIGG:
        res = makeBinaryOperation(TAC_BIGG, code[0], code[1]);
        break;
      case AST_LESS:
        res = makeBinaryOperation(TAC_LESS, code[0], code[1]);
        break;
      case AST_LE:
        res = makeBinaryOperation(TAC_LE, code[0], code[1]);
        break;
      case AST_GE:
        res = makeBinaryOperation(TAC_GE, code[0], code[1]);
        break;
      case AST_EQ:
        res = makeBinaryOperation(TAC_EQ, code[0], code[1]);
        break;
      case AST_DIF:
        res = makeBinaryOperation(TAC_DIF, code[0], code[1]);
        break;
      case AST_AND:
        res = makeBinaryOperation(TAC_AND, code[0], code[1]);
        break;
      case AST_OR:
        res = makeBinaryOperation(TAC_OR, code[0], code[1]);
        break;
      case AST_NEG:
        res = tacCreate(TAC_NEG, makeTemp(), code[0] ? code[0]->res : 0, 0);
        break;
      case AST_FUNC_CALL:
        res = tacJoin(tacCreate(TAC_FUNC_CALL, node->son[0]->symbol, code[1] ? code[1]->res : 0, 0), code[1]);
        break;
      case AST_FUNC_CALL_ARG:
        res = tacJoin(tacCreate(TAC_FUNC_CALL_ARG, node->son[0]->symbol, 0, 0), code[1]);
        break;
      case AST_INPUT:
        res = tacJoin(code[0], tacCreate(TAC_INPUT, makeTemp(), code[0] ? code[0]->res : 0, 0));
        break;
      case AST_VAR_DEC:
        res = tacJoin(code[2], tacCreate(TAC_VAR_DEC, node->son[1]->symbol, code[2] ? code[2]->res : 0, 0));
        break;
      case AST_VEC_DEC:
        res = tacJoin(tacCreate(TAC_VEC_DEC, node->son[1]->symbol, code[2] ? code[2]->res : 0, 0), code[3]);
        break;
      case AST_VEC_TAIL:
        res = tacJoin(tacCreate(TAC_VEC_INIT_VALUE, node->son[0]->symbol, 0, 0), code[1]);
        break;
      case AST_FUNC_DEC:
        res = makeFuncDec(code[0], code[1], code[2], code[3]);
        break;
      case AST_ARG:
        res = tacJoin(code[0], tacCreate(TAC_ARG, node->son[1]->symbol, code[0] ? code[0]->res : 0, 0));
        break;
      case AST_VAR_ASSIGN:
        res = tacJoin(code[1], tacCreate(TAC_COPY, node->son[0]->symbol, code[1] ? code[1]->res : 0, 0));
        break;
      case AST_VEC_ASSIGN:
        res = tacJoin(tacJoin(code[1], code[2]), tacCreate(TAC_COPY_VEC, node->son[0]->symbol, code[1] ? code[1]->res : 0, code[2] ? code[2]->res : 0));
        break;
      case AST_IF:
        res = makeIf(code[0], code[1]);
        break;
      case AST_IF_ELSE:
        res = makeIfElse(code[0], code[1], code[2]);
        break;
      case AST_IF_LOOP:
        res = makeIfLoop(code[0], code[1]);
        break;
      case AST_OUTPUT:
        res = tacJoin(tacCreate(TAC_OUTPUT, 0, 0, 0), code[0]);
        break;
      case AST_OUTPUT_ARG:
        res = tacJoin(tacCreate(TAC_OUTPUT_ARG, node->son[0]->symbol ? node->son[0]->symbol : node->son[0]->son[0]->symbol, node->son[0]->son[1] ? node->son[0]->son[1]->symbol : 0, 0), tacJoin(code[0], code[1]));
        break;
      case AST_RETURN:
        res = tacJoin(code[0], tacCreate(TAC_RETURN, 0, code[0] ? code[0]->res : 0, 0));
        break;
      default:
        res = tacJoin(tacJoin(tacJoin(code[0], code[1]), code[2]), code[3]);
        break;
    }
  }

  return res;
}

