%{
  // author: Willian R. Pramio (00262875)

  #include "ast.h"
  #include "hash.h"
  #include "y.tab.h"

  char buf[100];
  int i = 0;
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

[0-9][0-9]*               { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_INT); return LIT_INT; }
[0-9]"."[0-9]+            { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_REAL); return LIT_REAL; }
'.'                       { i = 0; buf[0] = yytext[1]; buf[1] = '\0'; yylval.symbol = hashInsert(buf, SYMBOL_LIT_CHAR); return LIT_CHAR; }

\"                        { BEGIN(STRING); i = 0; }
<STRING>\\t               { buf[i++] = '\t'; }
<STRING>\\\"              { buf[i++] = '\"'; }
<STRING>\"                { buf[i] = '\0'; yylval.symbol = hashInsert(buf, SYMBOL_LIT_STRING); BEGIN(INITIAL); return LIT_STRING; }
<STRING>\n                {}
<STRING>.                 { buf[i++] = *yytext; }

"\\\\\\"                  { BEGIN(COMMENT); }
<COMMENT>"///"            { BEGIN(INITIAL); }
<COMMENT>\n               { LineNumber++; }
<COMMENT>.                {}
"\\\\"[^\\].*             {}

[a-zA-Z\._]+              { yylval.symbol = hashInsert(yytext, SYMBOL_IDENTIFIER); return TK_IDENTIFIER; }

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
