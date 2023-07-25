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
            fprintf(stderr, "Semantic Error (line %d): variable '%s' already declared\n", node->linenum, node->son[1]->symbol->text);
            SemanticErrors++;
          }
          if (!compatible_datatypes(node->son[0]->datatype, node->son[2]->symbol->datatype))
          {
            fprintf(stderr, "Semantic Error (line %d): variable '%s' initialized with incompatible value\n", node->linenum, node->son[1]->symbol->text);
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
        node->linenum = node->son[0]->linenum;
        if (node->son[1]->symbol)
        {
          if (node->son[1]->symbol->type != SYMBOL_IDENTIFIER)
          {
            fprintf(stderr, "Semantic Error (line %d): function '%s' already declared\n", node->linenum, node->son[1]->symbol->text);
            SemanticErrors++;
          }

          // set type on hash_node
          node->son[1]->symbol->type = SYMBOL_FUNCTION;
          // set datatype on hash_node
          node->son[1]->symbol->datatype = node->son[0]->datatype;
          // set datatype on AST_SYMBOL node
          node->son[1]->datatype = node->son[0]->datatype;
          // set the hash_node's void pointer to keep the AST_FUNC_DEC node
          node->son[1]->symbol->ast_dec_node = node;
        }
        break;
      case AST_VEC_DEC:
        if (node->son[1]->symbol)
        {
          if (node->son[1]->symbol->type != SYMBOL_IDENTIFIER)
          {
            fprintf(stderr, "Semantic Error (line %d): vector '%s' already declared\n", node->linenum, node->son[1]->symbol->text);
            SemanticErrors++;
          }
          // the parser already ensures the vector size is an integer, but I check again to be sure
          if (node->son[2]->symbol->datatype != DATATYPE_INT)
          {
            fprintf(stderr, "Semantic Error (line %d): size of vector '%s' has non-integer datatype\n", node->linenum, node->son[1]->symbol->text);
            SemanticErrors++;
          }
          if (!valid_vector_init_values(node))
          {
            fprintf(stderr, "Semantic Error (line %d): vector '%s' initialized with invalid datatype\n", node->linenum, node->son[1]->symbol->text);
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
          fprintf(stderr, "Semantic Error (line %d): symbol '%s' being assigned is not a variable\n", node->linenum, node->son[0]->symbol->text);
          SemanticErrors++;
        }
        else if (!compatible_datatypes(node->son[0]->symbol->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): variable '%s' assigned with incompatible datatype\n", node->linenum, node->son[0]->symbol->text);
          SemanticErrors++;
        }
        break;
      case AST_VEC_ASSIGN:
        if (node->son[0]->symbol->type != SYMBOL_VECTOR)
        {
          fprintf(stderr, "Semantic Error (line %d): symbol '%s' being assigned is not a vector\n", node->linenum, node->son[0]->symbol->text);
          SemanticErrors++;
        }
        else
        {
          if (!compatible_datatypes(node->son[0]->symbol->datatype, node->son[2]->datatype))
          {
            fprintf(stderr, "Semantic Error (line %d): vector '%s' assigned with incompatible datatype\n", node->linenum, node->son[0]->symbol->text);
            SemanticErrors++;
          }
          if (node->son[1]->datatype != DATATYPE_INT && node->son[1]->datatype != DATATYPE_CHAR)
          {
            fprintf(stderr, "Semantic Error (line %d): vector '%s' assignment index has invalid datatype\n", node->linenum, node->son[0]->symbol->text);
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
          fprintf(stderr, "Semantic Error (line %d): function '%s' called without arguments list\n", node->linenum, node->symbol->text);
          SemanticErrors++;
        }
        if (node->symbol->type == SYMBOL_VECTOR)
        {
          fprintf(stderr, "Semantic Error (line %d): vector '%s' accessed without index value\n", node->linenum, node->symbol->text);
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
          fprintf(stderr, "Semantic Error (line %d): symbol '%s' is not a vector\n", node->linenum, node->son[0]->symbol->text);
          SemanticErrors++;
        }
        else if (node->son[1]->datatype != DATATYPE_INT && node->son[1]->datatype != DATATYPE_CHAR)
        {
          fprintf(stderr, "Semantic Error (line %d): vector '%s' access index is not an integer nor char\n", node->linenum, node->son[0]->symbol->text);
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
          fprintf(stderr, "Semantic Error (line %d): called symbol '%s' is not a function\n", node->linenum, node->son[0]->symbol->text);
          SemanticErrors++;
        }
        else if (!valid_function_call_args(node))
          SemanticErrors++;
        node->datatype = node->son[0]->symbol->datatype;
        break;
      case AST_INPUT:
        node->datatype = node->son[0]->datatype;
        break;
      case AST_ADD:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for ADD\n", node->linenum);
          SemanticErrors++;
        }
        // all good, set the expression datatype
        node->datatype = node->son[0]->datatype;
        break;
      case AST_SUB:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for SUB\n", node->linenum);
          SemanticErrors++;
        }
        node->datatype = node->son[0]->datatype;
        break;
      case AST_MULT:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for MULT\n", node->linenum);
          SemanticErrors++;
        }
        node->datatype = node->son[0]->datatype;
        break;
      case AST_DIV:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for DIV\n", node->linenum);
          SemanticErrors++;
        }
        node->datatype = node->son[0]->datatype;
        break;
      case AST_BIGG:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for BIGG\n", node->linenum);
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_LESS:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for LESS\n", node->linenum);
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_LE:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for LE\n", node->linenum);
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_GE:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for GE\n", node->linenum);
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_EQ:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for EQ\n", node->linenum);
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_DIF:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for DIF\n", node->linenum);
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_AND:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for AND\n", node->linenum);
          SemanticErrors++;
        }
        node->datatype = DATATYPE_BOOL;
        break;
      case AST_OR:
        if (!compatible_datatypes(node->son[0]->datatype, node->son[1]->datatype))
        {
          fprintf(stderr, "Semantic Error (line %d): incompatible operands for OR\n", node->linenum);
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
          fprintf(stderr, "Semantic Error (line %d): function '%s' has an incompatible return datatype\n", node->linenum, node->son[1]->symbol->text);
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

int valid_function_call_args(AST *func_call)
{
  // assumes that ast_dec_node was previously set on `check_and_set_declarations`
  AST *func_dec = func_call->son[0]->symbol->ast_dec_node;
  // func_dec->son[2] := AST_ARG_LIST or null
  // AST_ARG_LIST:
  // son[0] := AST_ARG
  //  son[0]->son[1] := AST_SYMBOL
  // son[1] := next AST_ARG_LIST or null
  AST *arg_iter = func_dec->son[2];
  // func_call->son[1] := AST_FUNC_CALL_ARG or null
  // AST_FUNC_CALL_ARG:
  // son[0] := expression
  // son[1] := next AST_FUNC_CALL_ARG or null
  AST *call_arg_iter = func_call->son[1];
  int arg_count = 0;
  int call_arg_count = 0;
  while(arg_iter != 0 || call_arg_iter != 0)
  {
    if (arg_iter != 0 && call_arg_iter != 0)
      if (!compatible_datatypes(arg_iter->son[0]->son[1]->symbol->datatype, call_arg_iter->son[0]->datatype))
      {
        fprintf(stderr, "Semantic Error (line %d): incompatible argument datatype provided on call for '%s' function\n", func_call->linenum, func_dec->son[1]->symbol->text);
        return 0;
      }
    if (arg_iter != 0)
    {

      arg_count++;
      arg_iter = arg_iter->son[1];
    }
    if (call_arg_iter != 0)
    {

      call_arg_count++;
      call_arg_iter = call_arg_iter->son[1];
    }
  }
  if (arg_count < call_arg_count)
  {
    fprintf(stderr, "Semantic Error (line %d): too many arguments provided on call for '%s' function (%d expected, %d provided)\n", func_call->linenum, func_dec->son[1]->symbol->text, arg_count, call_arg_count);
    return 0;
  }
  if (arg_count > call_arg_count)
  {
    fprintf(stderr, "Semantic Error (line %d): too few arguments provided on call for '%s' function (%d expected, %d provided)\n", func_call->linenum, func_dec->son[1]->symbol->text, arg_count, call_arg_count);
    return 0;
  }

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