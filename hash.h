// author: Willian R. Pramio (00262875)

#ifndef HASH_HEADER
#define HASH_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SYMBOL_LIT_INT    1
#define SYMBOL_LIT_REAL   2
#define SYMBOL_LIT_CHAR   3
#define SYMBOL_LIT_BOOL   4
#define SYMBOL_LIT_STRING 5
#define SYMBOL_IDENTIFIER 6
#define SYMBOL_VARIABLE   7
#define SYMBOL_VECTOR     8
#define SYMBOL_FUNCTION   9

#define DATATYPE_INT      1
#define DATATYPE_REAL     2
#define DATATYPE_CHAR     3
#define DATATYPE_BOOL     4

#define HASH_SIZE 997

typedef struct hash_node
{
  int type;
  int datatype;
  char *text;
  void *ast_dec_node;
  struct hash_node * next;
} HASH_NODE;

// global hash table
HASH_NODE *Table[HASH_SIZE];

void hashInit(void);
int hashAddress(char *text);
HASH_NODE *hashFind(char *text);
HASH_NODE *hashInsert(char *text, int type, int datatype);
void hashPrint(void);

#endif
