%{
  int yyerror (char const *s);
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
%left '.' '&'
%left '<' '>' OPERATOR_EQ OPERATOR_DIF OPERATOR_GE OPERATOR_LE
%left '+' '-'
%left '*' '/'

%%

program: global_declaration_list
       ;


global_declaration_list: global_declaration global_declaration_list
                       |
                       ;


global_declaration: var_declaration
                  | func_declaration
                  ;

var_declaration: kw_type TK_IDENTIFIER '=' literal ';'
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

vector_declaration: kw_type TK_IDENTIFIER '[' LIT_INT ']' vector_tail ';'
                  ;

vector_tail: literal vector_tail
           |
           ;

func_declaration: kw_type TK_IDENTIFIER '(' arg_list ')' command_block
                ;

arg_list: arg
        | arg ',' arg_list
        |
        ;

arg: kw_type TK_IDENTIFIER
   ;

command_block: '{' command_list '}'
             ;

command_list: command command_list
            |
            ;

command: TK_IDENTIFIER '=' expression
       | KW_IF expression command
       | KW_IF expression command KW_ELSE command
       /* |  isso gera conflitos, olhar depois*/
       ;

expression: LIT_INT
        | TK_IDENTIFIER
        | expression '+' expression
        | expression '-' expression
        | expression '*' expression
        | expression '/' expression
        | expression '>' expression
        | expression '<' expression
        | expression '.' expression
        | expression '&' expression
        | expression OPERATOR_EQ expression
        | '(' expression ')'
        /* e varios outros */
        ;
%%

int yyerror(char const *s)
{
  fprintf(stderr, "Syntax error at line %d.\n", getLineNumber());
  exit(3);
}
