// author: Willian R. Pramio (00262875)
// AST - Abstract Syntax Tree

#include "ast.h"

AST *global_ast;

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
    switch (node->type)
    {
      case AST_SYMBOL: fprintf(stderr, "AST_SYMBOL"); break;
      case AST_ADD: fprintf(stderr, "AST_ADD"); break;
      case AST_SUB: fprintf(stderr, "AST_SUB"); break;
      case AST_MULT: fprintf(stderr, "AST_MULT"); break;
      case AST_DIV: fprintf(stderr, "AST_DIV"); break;
      case AST_BIGG: fprintf(stderr, "AST_BIGG"); break;
      case AST_LESS: fprintf(stderr, "AST_LESS"); break;
      case AST_LE: fprintf(stderr, "AST_LE"); break;
      case AST_GE: fprintf(stderr, "AST_GE"); break;
      case AST_EQ: fprintf(stderr, "AST_EQ"); break;
      case AST_DIF: fprintf(stderr, "AST_DIF"); break;
      case AST_AND: fprintf(stderr, "AST_AND"); break;
      case AST_OR: fprintf(stderr, "AST_OR"); break;
      case AST_NEG: fprintf(stderr, "AST_NEG"); break;
      case AST_NESTED_EXPR: fprintf(stderr, "AST_NESTED_EXPR"); break;
      case AST_FUNC_CALL: fprintf(stderr, "AST_FUNC_CALL"); break;
      case AST_FUNC_CALL_ARG: fprintf(stderr, "AST_FUNC_CALL_ARG"); break;
      case AST_INPUT: fprintf(stderr, "AST_INPUT"); break;
      case AST_TYPE_CHAR: fprintf(stderr, "AST_TYPE_CHAR"); break;
      case AST_TYPE_INT: fprintf(stderr, "AST_TYPE_INT"); break;
      case AST_TYPE_REAL: fprintf(stderr, "AST_TYPE_REAL"); break;
      case AST_TYPE_BOOL: fprintf(stderr, "AST_TYPE_BOOL"); break;
      case AST_VEC_ACCESS: fprintf(stderr, "AST_VEC_ACCESS"); break;
      case AST_VAR_DEC: fprintf(stderr, "AST_VAR_DEC"); break;
      case AST_VEC_DEC: fprintf(stderr, "AST_VEC_DEC"); break;
      case AST_VEC_TAIL: fprintf(stderr, "AST_VEC_TAIL"); break;
      case AST_FUNC_DEC: fprintf(stderr, "AST_FUNC_DEC"); break;
      case AST_ARG_LIST: fprintf(stderr, "AST_ARG_LIST"); break;
      case AST_ARG: fprintf(stderr, "AST_ARG"); break;
      case AST_CMD_BLOCK: fprintf(stderr, "AST_CMD_BLOCK"); break;
      case AST_CMD_LIST: fprintf(stderr, "AST_CMD_LIST"); break;
      case AST_VAR_ASSIGN: fprintf(stderr, "AST_VAR_ASSIGN"); break;
      case AST_VEC_ASSIGN: fprintf(stderr, "AST_VEC_ASSIGN"); break;
      case AST_IF: fprintf(stderr, "AST_IF"); break;
      case AST_IF_ELSE: fprintf(stderr, "AST_IF_ELSE"); break;
      case AST_IF_LOOP: fprintf(stderr, "AST_IF_LOOP"); break;
      case AST_OUTPUT: fprintf(stderr, "AST_OUTPUT"); break;
      case AST_RETURN: fprintf(stderr, "AST_RETURN"); break;
      case AST_EMPTY_CMD: fprintf(stderr, "AST_EMPTY_CMD"); break;
      case AST_OUTPUT_ARG: fprintf(stderr, "AST_OUTPUT_ARG"); break;
      case AST_GLOBAL_DEC_LIST: fprintf(stderr, "AST_GLOBAL_DEC_LIST"); break;
      case AST_SYMBOL_CHAR: fprintf(stderr, "AST_SYMBOL_CHAR"); break;
      case AST_SYMBOL_STRING: fprintf(stderr, "AST_SYMBOL_STRING"); break;
      default: fprintf(stderr, "AST_UNKOWN(%d)", node->type); break;
    }
    if (node->symbol != 0)
      fprintf(stderr, ", %s\n", node->symbol->text);
    else
      fprintf(stderr, "\n");
    for(int i=0; i < MAX_SONS; ++i)
    {
      astPrint(node->son[i], level + 1);
    }
  }
}

void *astDecompile(char* filename_out)
{
  if (global_ast == 0 || global_ast->son[0] == 0)
    fprintf(stderr, "Null global_ast\n");

  FILE* file_out = fopen(filename_out, "w");
  astDecompileNode(global_ast, file_out);
  fclose(file_out);
}

// must receive an open FILE handler, otherwise race conditions may occur
void astDecompileNode(AST *node, FILE* file_out)
{
  // won't print if it's a null pointer
  if (node != 0)
  {
    switch (node->type)
    {
      case AST_SYMBOL:
        fprintf(file_out, "%s", node->symbol->text); break;
      case AST_SYMBOL_CHAR:
        fprintf(file_out, "'%s'", node->symbol->text); break;
      case AST_SYMBOL_STRING:
        fprintf(file_out, "\"%s\"", node->symbol->text); break;
      case AST_ADD:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " + ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_SUB:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " - ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_MULT:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " * ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_DIV:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " / ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_BIGG:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " > ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_LESS:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " < ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_LE:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " <= ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_GE:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " >= ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_EQ:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " == ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_DIF:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " + ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_AND:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " & ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_OR:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " | ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_NEG:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " ~ ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_NESTED_EXPR:
        fprintf(file_out, "(");
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, ")");
        break;
      case AST_FUNC_CALL:
        if (node->son[1] == 0)
        {
          astDecompileNode(node->son[0], file_out);
          fprintf(file_out, "()");
        }
        else
        {
          astDecompileNode(node->son[0], file_out);
          fprintf(file_out, "(");
          astDecompileNode(node->son[1], file_out);
          fprintf(file_out, ")");
        }
        break;
      case AST_FUNC_CALL_ARG:
        if (node->son[1] == 0)
          astDecompileNode(node->son[0], file_out);
        else
        {
          astDecompileNode(node->son[0], file_out);
          fprintf(file_out, ", ");
          astDecompileNode(node->son[1], file_out);
        }
        break;
      case AST_INPUT:
        fprintf(file_out, "input(");
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, ")");
        break;
      case AST_TYPE_CHAR:
        fprintf(file_out, "char"); break;
      case AST_TYPE_INT:
        fprintf(file_out, "int"); break;
      case AST_TYPE_REAL:
        fprintf(file_out, "real"); break;
      case AST_TYPE_BOOL:
        fprintf(file_out, "bool"); break;
      case AST_VEC_ACCESS:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, "[");
        astDecompileNode(node->son[1], file_out);
        fprintf(file_out, "]");
        break;
      case AST_VAR_DEC:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " ");
        astDecompileNode(node->son[1], file_out);
        fprintf(file_out, " = ");
        astDecompileNode(node->son[2], file_out);
        fprintf(file_out, ";\n");
        break;
      case AST_VEC_DEC:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " ");
        astDecompileNode(node->son[1], file_out);
        fprintf(file_out, "[");
        astDecompileNode(node->son[2], file_out);
        fprintf(file_out, "]");
        if (node->son[3] != 0)
          fprintf(file_out, " ");
        astDecompileNode(node->son[3], file_out);
        fprintf(file_out, ";\n");
        break;
      case AST_VEC_TAIL:
        astDecompileNode(node->son[0], file_out);
        if (node->son[1] != 0)
        {
          fprintf(file_out, " ");
          astDecompileNode(node->son[1], file_out);
        }
        break;
      case AST_FUNC_DEC:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " ");
        astDecompileNode(node->son[1], file_out);
        fprintf(file_out, " (");
        if (node->son[2] != 0)
          astDecompileNode(node->son[2], file_out);
        fprintf(file_out, ") \n");
        astDecompileNode(node->son[3], file_out);
        fprintf(file_out, "\n");
        break;
      case AST_ARG_LIST:
        astDecompileNode(node->son[0], file_out);
        if (node->son[1] != 0)
        {
          fprintf(file_out, ", ");
          astDecompileNode(node->son[1], file_out);
        }
        break;
      case AST_ARG:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_CMD_BLOCK:
        fprintf(file_out, "{");
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, "}");
        break;
      case AST_CMD_LIST:
        astDecompileNode(node->son[0], file_out);
        if (node->son[1] != 0)
        {
          fprintf(file_out, "\n");
          astDecompileNode(node->son[1], file_out);
        }
        break;
      case AST_VAR_ASSIGN:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, " = ");
        astDecompileNode(node->son[1], file_out);
        fprintf(file_out, ";");
        break;
      case AST_VEC_ASSIGN:
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, "[");
        astDecompileNode(node->son[1], file_out);
        fprintf(file_out, "] = ");
        astDecompileNode(node->son[2], file_out);
        fprintf(file_out, ";");
        break;
      case AST_IF:
        fprintf(file_out, "if (");
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, ") ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_IF_ELSE:
        fprintf(file_out, "if (");
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, ") ");
        astDecompileNode(node->son[1], file_out);
        fprintf(file_out, " else ");
        astDecompileNode(node->son[2], file_out);
        break;
      case AST_IF_LOOP:
        fprintf(file_out, "if (");
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, ") loop ");
        astDecompileNode(node->son[1], file_out);
        break;
      case AST_OUTPUT:
        fprintf(file_out, "output ");
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, ";");
        break;
      case AST_RETURN:
        fprintf(file_out, "return ");
        astDecompileNode(node->son[0], file_out);
        fprintf(file_out, ";");
        break;
      case AST_OUTPUT_ARG:
        astDecompileNode(node->son[0], file_out);
        if (node->son[1] != 0)
        {
          fprintf(file_out, ", ");
          astDecompileNode(node->son[1], file_out);
        }
        break;
      case AST_EMPTY_CMD:
        fprintf(file_out, ";");
        break;
      case AST_GLOBAL_DEC_LIST:
        astDecompileNode(node->son[0], file_out);
        if (node->son[1] != 0)
          astDecompileNode(node->son[1], file_out);
        break;
      default: fprintf(file_out, "AST_UNKOWN(%d)", node->type); break;
    }
  }
}