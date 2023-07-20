// author: Willian R. Pramio (00262875)
// Semantic Verification

#ifndef SEMANTIC_HEADER
#define SEMANTIC_HEADER

#include "hash.h"
#include "ast.h"

extern int SemanticErrors;

void check_and_set_declarations(AST *node);
void check_expressions(AST *node);
void check_assignments(AST* node);
void check_undeclared();
int compatible_datatypes(int dt1, int dt2);
int valid_vector_init_values(AST *vec_dec);
int valid_return_datatype(AST *func_dec);
int valid_function_call_args(AST *func_call);

#endif
