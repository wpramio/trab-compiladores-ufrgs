%{
  // author: Willian R. Pramio (00262875)

  #include "hash.h"

  int yyerror (char const *s);
  int getLineNumber();
  int yylex();
%}

// types
%token KW_CHAR
%token KW_INT
%token KW_REAL
%token KW_BOOL

// commands
%token KW_IF
%token KW_THEN
%token KW_ELSE
%token KW_LOOP
%token KW_INPUT
%token KW_OUTPUT
%token KW_RETURN

// operators
%token OPERATOR_LE
%token OPERATOR_GE
%token OPERATOR_EQ
%token OPERATOR_DIF

// identifier
%token TK_IDENTIFIER

// literals
%token LIT_INT
%token LIT_REAL
%token LIT_CHAR
%token LIT_STRING

// token error
%token TOKEN_ERROR

// operator precedence
%left '~' '&' '|'
%left '<' '>' OPERATOR_EQ OPERATOR_DIF OPERATOR_GE OPERATOR_LE
%left '+' '-'
%left '*' '/'

%%

program: global_declaration_list
       ;


global_declaration_list: global_declaration global_declaration_list
                       |
                       ;


global_declaration: var_declaration ';'
                  | func_declaration
                  ;

var_declaration: kw_type TK_IDENTIFIER '=' literal
               | vector_declaration
               ;

kw_type: KW_CHAR
       | KW_INT
       | KW_REAL
       | KW_BOOL
       ;

literal: LIT_INT
       | LIT_REAL
       | LIT_CHAR
       ;

vector_declaration: kw_type TK_IDENTIFIER '[' LIT_INT ']' vector_tail
                  ;

vector_tail: literal vector_tail
           |
           ;

func_declaration: kw_type TK_IDENTIFIER '(' arg_list ')' command_block
                | kw_type TK_IDENTIFIER '(' ')' command_block
                ;

arg_list: arg
        | arg ',' arg_list
        ;

arg: kw_type TK_IDENTIFIER
   ;

command_block: '{' command_list '}'
             ;

command_list: command command_list
            |
            ;

command: TK_IDENTIFIER '=' expression ';'
       | TK_IDENTIFIER '[' expression ']' '=' expression ';'
       | KW_IF '(' expression ')' command
       | KW_IF '(' expression ')' command KW_ELSE command
       | KW_IF '(' expression ')' KW_LOOP command
       | KW_OUTPUT output_arg_list ';'
       | KW_RETURN expression ';'
       | command_block
       | ';'
       ;

output_arg_list: output_arg
               | output_arg ',' output_arg_list
               ;

output_arg: LIT_STRING
          | expression
          ;

expression: literal
          | TK_IDENTIFIER
          | TK_IDENTIFIER '[' expression ']'
          | expression '+' expression
          | expression '-' expression
          | expression '*' expression
          | expression '/' expression
          | expression '>' expression
          | expression '<' expression
          | expression OPERATOR_LE expression
          | expression OPERATOR_GE expression
          | expression OPERATOR_EQ expression
          | expression OPERATOR_DIF expression
          | expression '&' expression
          | expression '|' expression
          | expression '~' expression
          | '(' expression ')'
          | TK_IDENTIFIER '(' ')'
          | TK_IDENTIFIER '(' func_call_arg_list ')'
          | KW_INPUT '(' kw_type ')'
          ;

func_call_arg_list: expression
                  | expression ',' func_call_arg_list
                  ;

%%

int yyerror(char const *s)
{
  fprintf(stderr, "Syntax error at line %d.\n", getLineNumber());
  exit(3);
}
