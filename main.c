
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "ast.h"
#include "semantic.h"

//lex.yy.h
int yyparse();
extern char *yytext;
extern FILE *yyin;

void initMe(void);
int getLineNumber(void);

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    fprintf(stderr, "Call: ./etapa3 filename_in filename_out\n");
    exit(1);
  }

  if ( (yyin = fopen(argv[1], "r")) == 0 )
  {
    fprintf(stderr, "Cannot open file %s... \n", argv[1]);
    exit(2);
  }

  FILE *file_out;
  if ( (file_out = fopen(argv[2], "a")) == 0 )
  {
    fprintf(stderr, "Cannot create file %s... \n", argv[2]);
    exit(2);
  }

  initMe();

  int ret = yyparse();

  astDecompile(argv[2]);

  hashPrint();
  fprintf(stderr, "\nFile has %d lines.\n", getLineNumber());

  if (SemanticErrors > 0)
  {
    fprintf(stderr, "\nCompilation failed: source file has %d semantic errors.\n", SemanticErrors);
    exit(4);
  }
  else if (ret == 0)
    fprintf(stderr, "\nSuccess! Compilation completed.\n", getLineNumber());

  return 0;
}
