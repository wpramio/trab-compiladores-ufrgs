
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

//lex.yy.h
int yyparse();
extern char *yytext;
extern FILE *yyin;

void initMe(void);
int getLineNumber(void);

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    fprintf(stderr, "Call: ./etapa2 file_name\n");
    exit(1);
  }

  if ( (yyin = fopen(argv[1], "r")) == 0 )
  {
    fprintf(stderr, "Cannot open file %s... \n", argv[1]);
    exit(1);
  }

  initMe();

  int ret = yyparse();

  hashPrint();
  if (ret == 0)
    fprintf(stderr, "\n Success! File has %d lines.\n", getLineNumber());

  return ret;
}
