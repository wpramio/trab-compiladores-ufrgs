
#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

//lex.yy.h
int yylex();
extern char *yytext;
extern FILE *yyin;

int isRunning(void);
void initMe(void);
int getLineNumber(void);

int main(int argc, char **argv)
{
  int token;

  if (argc < 2)
  {
    fprintf(stderr, "Call: ./etapa1 file_name\n");
    exit(1);
  }

  yyin = fopen(argv[1], "r");

  yylex();

  while(isRunning()) {
    token = yylex();

    if (!isRunning())
      break;

    switch(token) {
      case KW_CHAR: printf("KW CHAR found\n"); break;
      case KW_INT: printf("KW INT found\n"); break;
      default: printf("TOKEN ERROR!\n"); break;
    }

  }
  return 0;
}