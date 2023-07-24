// author: Willian R. Pramio (00262875)
// Semantic Verification

#include "semantic.h"

int SemanticErrors = 0;

void check_and_set_declarations(AST *node)
{
  // null pointer check
  if (node != 0)
  {
    // check and set all sons (sub-trees) first
    for(int i=0; i < MAX_SONS; ++i)
      check_and_set_declarations(node->son[i]);

    switch (node->type)
    {
      case AST_SYMBOL:
        node->datatype = node->symbol->datatype; break;
      case AST_VAR_DEC:
        if (node->son[1]->symbol)
        {
          if (node->son[1]->symbol->type != SYMBOL_IDENTIFIER)
          {
            fprintf(stderr, "Semantic Error: variable '%s' already declared\n", node->son[1]->symbol->text);
            SemanticErrors++;
          }
          if (!compatible_datatypes(node->son[0]->datatype, node->son[2]->symbol->datatype))
          {
            fprintf(stderr, "Semantic Error: variable '%s' initialized with incompatible value\n", node->son[1]->symbol->text);
            SemanticErrors++;
          }

          // set type on hash_node
          node->son[1]->symbol->type = SYMBOL_VARIABLE;
          // set datatype on hash_node
          node->son[1]->symbol->datatype = node->son[0]->datatype;
          // set datatype on AST_SYMBOL node
          node->son[1]->datatype = node->son[0]->datatype;
        }
        break;
      case AST_FUNC_DEC:
        if (node->son[1]->symbol)
        {
          if (node->son[1]->symbol->type != SYMBOL_IDENTIFIER)
          {
            fprintf(stderr, "Semantic Error: function '%s' already declared\n", node->son[1]->symbol->text);
            SemanticErrors++;
          }

          // set type on hash_node
          node->son[1]->symbol->type = SYMBOL_FUNCTION;
          // set datatype on hash_node
          node->son[1]->symbol->datatype = node->son[0]->datatype;
          // set datatype on AST_SYMBOL node
          node->son[1]->datatype = node->son[0]->datatype;
        }
        break;
      case AST_VEC_DEC:
        if (node->son[1]->symbol)
        {
          if (node->son[1]->symbol->type != SYMBOL_IDENTIFIER)
          {
            fprintf(stderr, "Semantic Error: vector '%s' already declared\n", node->son[1]->symbol->text);
            SemanticErrors++;
          }
          // the parser already ensures the vector size is an integer, but I check again to be sure
          if (node->son[2]->symbol->datatype != DATATYPE_INT)
          {
            fprintf(stderr, "Semantic Error: size of vector '%s' has non-integer datatype\n", node->son[1]->symbol->text);
            SemanticErrors++;
          }
          if (!valid_vector_init_values(node))
          {
            fprintf(stderr, "Semantic Error: vector '%s' initialized with invalid datatype\n", node->son[1]->symbol->text);
            SemanticErrors++;
          }

          // set type on hash_node
          node->son[1]->symbol->type = SYMBOL_VECTOR;
          // set datatype on hash_node
          node->son[1]->symbol->datatype = node->son[0]->datatype;
          // set datatype on AST_SYMBOL node
          node->son[1]->datatype = node->son[0]->datatype;
        }
        break;
      case AST_ARG:
        node->son[1]->symbol->type = SYMBOL_VARIABLE;
        node->son[1]->symbol->datatype = node->son[0]->datatype;
        node->son[1]->datatype = node->son[0]->datatype;
        break;
      case AST_TYPE_INT:
        node->datatype = DATATYPE_INT;
        break;
      case AST_TYPE_REAL:
        node->datatype = DATATYPE_REAL;
        break;
      case AST_TYPE_CHAR:
        node->datatype = DATATYPE_CHAR;
        break;
      case AST_TYPE_BOOL:
        node->datatype = DATATYPE_BOOL;
        break;
    }
  }
}

void check_assignments(AST* node)
{
  if (node != 0)
  {
    for(int i=0; i < MAX_SONS; ++i)
      check_assignments(node->son[i]);

    switch (node->type)
    {
      case AST_VAR_ASSIGN:
        if (node->son[0]->symbol->type != SYMBOL_VARIABLE)
        {
          fprintf(stderr, "Semantic Error: symbol '%s' being assigned is not a variable\n", node->son[0]->symbol->text);
          SemanticErrors++;
        }
        else if (!compatible_datatypes(node->son[0]->symbol->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: variable '%s' assigned with incompatible datatype\n", node->son[0]->symbol->text);
          SemanticErrors++;
        }
        break;
      case AST_VEC_ASSIGN:
        if (node->son[0]->symbol->type != SYMBOL_VECTOR)
        {
          fprintf(stderr, "Semantic Error: symbol '%s' being assigned is not a vector\n", node->son[0]->symbol->text);
          SemanticErrors++;
        }
        else
        {
          if (!compatible_datatypes(node->son[0]->symbol->datatype, node->son[2]->datatype))
          {
            fprintf(stderr, "Semantic Error: vector '%s' assigned with incompatible datatype\n", node->son[0]->symbol->text);
            SemanticErrors++;
          }
          if (node->son[1]->datatype != DATATYPE_INT && node->son[1]->datatype != DATATYPE_CHAR)
          {
            fprintf(stderr, "Semantic Error: vector '%s' assignment index has invalid datatype\n", node->son[0]->symbol->text);
            SemanticErrors++;
          }
        }
        break;
    }
  }
}

void check_expressions(AST* node)
{
  if (node != 0)
  {
    for(int i=0; i < MAX_SONS; ++i)
      check_expressions(node->son[i]);

    switch (node->type)
    {
      case AST_VAR_ACCESS:
        if (node->symbol->type == SYMBOL_FUNCTION)
        {
          fprintf(stderr, "Semantic Error: function '%s' called without arguments list\n", node->symbol->text);
          SemanticErrors++;
        }
        if (node->symbol->type == SYMBOL_VECTOR)
        {
          fprintf(stderr, "Semantic Error: vector '%s' accessed without index value\n", node->symbol->text);
          SemanticErrors++;
        }
        node->datatype = node->symbol->datatype;
        break;
      case AST_SYMBOL_CHAR:
        node->datatype = DATATYPE_CHAR;
        break;
      case AST_VEC_ACCESS:
        if (node->son[0]->symbol->type != SYMBOL_VECTOR)
        {
          fprintf(stderr, "Semantic Error: symbol '%s' is not a vector\n", node->son[0]->symbol->text);
          SemanticErrors++;
        }
        if (node->son[1]->datatype != DATATYPE_INT && node->son[1]->datatype != DATATYPE_CHAR)
        {
          fprintf(stderr, "Semantic Error: vector '%s' access index is not an integer nor char\n", node->son[0]->symbol->text);
          SemanticErrors++;
        }
        // all good, set the ast_node datatype
        node->datatype = node->son[0]->symbol->datatype;
        break;
      case AST_NESTED_EXPR:
        node->datatype = node->son[0]->datatype;
        break;
      case AST_FUNC_CALL:
        if (node->son[0]->symbol->type != SYMBOL_FUNCTION)
        {
          fprintf(stderr, "Semantic Error: called symbol '%s' is not a function\n", node->son[0]->symbol->text);
          SemanticErrors++;
        }
        if (!valid_function_call_args(node))
        {
          fprintf(stderr, "Semantic Error: invalid arguments for function %s\n", node->son[0]->symbol->text);
          SemanticErrors++;
        }
        node->datatype = node->son[0]->symbol->datatype;
        break;
      case AST_INPUT:
        node->datatype = node->son[0]->datatype;
        break;
      case AST_ADD:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for ADD\n");
          SemanticErrors++;
        }
        // all good, set the expression datatype
        node->datatype = node->son[0]->datatype;
        break;
      case AST_SUB:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for SUB\n");
          SemanticErrors++;
        }
        node->datatype = node->son[0]->datatype;
        break;
      case AST_MULT:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for MULT\n");
          SemanticErrors++;
        }
        node->datatype = node->son[0]->datatype;
        break;
      case AST_DIV:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for DIV\n");
          SemanticErrors++;
        }
        node->datatype = node->son[0]->datatype;
        break;
      case AST_BIGG:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for BIGG\n");
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_LESS:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for LESS\n");
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_LE:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for LE\n");
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_GE:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for GE\n");
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_EQ:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for EQ\n");
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_DIF:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for DIF\n");
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_AND:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for AND\n");
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_OR:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error: incompatible operands for OR\n");
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_NEG:
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_FUNC_DEC:
        if (!valid_return_datatype(node))
        {
          fprintf(stderr, "Semantic Error: function '%s' has an incompatible return datatype\n", node->son[1]->symbol->text);
          SemanticErrors++;
        }
        break;
    }
  }
}

void check_undeclared()
{
  HASH_NODE* node;
  for(int i=0; i < HASH_SIZE; ++i)
    for (node = Table[i]; node; node = node->next)
      if (node->type == SYMBOL_IDENTIFIER)
      {
        fprintf(stderr, "Semantic Error: symbol '%s' undeclared\n", node->text);
        SemanticErrors++;
      }
}

int compatible_datatypes(int dt1, int dt2)
{
  // fprintf(stderr, "%d vs %d\n", dt1, dt2);
  if (dt1 == dt2 || (dt1 == DATATYPE_INT && dt2 == DATATYPE_CHAR) || (dt1 == DATATYPE_CHAR && dt2 == DATATYPE_INT))
    return 1;
  else
    return 0;
}

int valid_vector_init_values(AST *vec_dec)
{
  AST *vec_tail = vec_dec->son[3];
  while(vec_tail != 0)
  {
    if (!compatible_datatypes(vec_tail->son[0]->symbol->datatype, vec_dec->son[0]->datatype))
      return 0;
    else
      vec_tail = vec_tail->son[1];
  }

  return 1;
}

// TODO
int valid_function_call_args(AST *func_call)
{
  // AST *vec_tail = node->son[3];
  // while(vec_tail != 0)
  // {
  //   if (!compatible_datatypes(vec_tail->son[0]->symbol->datatype, node->son[0]->datatype))
  //     return 0;
  //   else
  //     vec_tail = vec_tail->son[1];
  // }

  return 1;
}

// check if there's some AST_RETURN inside the cmd_block, and compare its datatype with the function's datatype
int valid_return_datatype(AST *func_dec)
{
  AST* cmd_list = func_dec->son[3]->son[0];
  while(cmd_list != 0)
  {
    // cmd_list->son[0] := command
    // cmd_list->son[0]->son[0] := expression
    if (cmd_list->son[0]->type == AST_RETURN && !compatible_datatypes(func_dec->son[1]->symbol->datatype, cmd_list->son[0]->son[0]->datatype))
      return 0;
    else
      cmd_list = cmd_list->son[1];
  }

  return 1;
}