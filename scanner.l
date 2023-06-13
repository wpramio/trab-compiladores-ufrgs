%{
  // author: Willian R. Pramio (00262875)

  #include "y.tab.h"
  #include "hash.c"

  char buf[100];
  char *s;
  int LineNumber = 1;
  int running = 1;
%}

%x STRING
%x COMMENT

%%

[ \t]                     {}
"\n"                      { LineNumber++; }

"char"                    { return KW_CHAR; }
"int"                     { return KW_INT; }
"real"                    { return KW_REAL; }
"bool"                    { return KW_BOOL; }
"if"                      { return KW_IF; }
"then"                    { return KW_THEN; }
"else"                    { return KW_ELSE; }
"loop"                    { return KW_LOOP; }
"input"                   { return KW_INPUT; }
"output"                  { return KW_OUTPUT; }
"return"                  { return KW_RETURN; }

[-+=*/%<>&|~,;()\[\]{}]   { return yytext[0]; }

"<="                      { return OPERATOR_LE; }
">="                      { return OPERATOR_GE; }
"=="                      { return OPERATOR_EQ; }
"!="                      { return OPERATOR_DIF; }

[0-9][0-9]*               { hashInsert(yytext, LIT_INT); return LIT_INT; }
[0-9]"."[0-9]+            { hashInsert(yytext, LIT_REAL); return LIT_REAL; }
'.'                       { s = buf; *s = 0; *s++ = yytext[1]; hashInsert(buf, LIT_CHAR); return LIT_CHAR; }

\"                        { BEGIN(STRING); s = buf; }
<STRING>\\n               { *s++ = '\n'; }
<STRING>\\t               { *s++ = '\t'; }
<STRING>\\\"              { *s++ = '\"'; }
<STRING>\"                { hashInsert(buf, LIT_STRING); *s = 0; BEGIN(INITIAL); return LIT_STRING; }
<STRING>\n                {}
<STRING>.                 { *s++ = *yytext; }

"\\\\\\"                  { BEGIN(COMMENT); }
<COMMENT>"///"            { BEGIN(INITIAL); }
<COMMENT>\n               { LineNumber++; }
<COMMENT>.                {}
"\\\\"[^\\].*             {}

[a-zA-Z\._]+              { hashInsert(yytext, TK_IDENTIFIER); return TK_IDENTIFIER; }

.                         { return TOKEN_ERROR; }

%%

int yywrap()
{
  running = 0;
  return 1;
}

int getLineNumber(void)
{
  return LineNumber;
}

void initMe(void)
{
}

int isRunning(void)
{
  return running;
}

#include "y.tab.c"
#include "main.c"
