%{
  // author: Willian R. Pramio (00262875)

  #include "hash.h"
  #include "ast.h"
  #include "semantic.h"
  #include "tac.h"

  extern AST *global_ast;
  int yyerror (char const *s);
  int getLineNumber();
  int yylex();
%}

%union
{
       HASH_NODE *symbol;
       AST *ast;
}

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
%token<symbol> TK_IDENTIFIER

// literals
%token<symbol> LIT_INT
%token<symbol> LIT_REAL
%token<symbol> LIT_CHAR
%token<symbol> LIT_STRING

// token error
%token TOKEN_ERROR

%type<ast> program
%type<ast> global_declaration_list
%type<ast> global_declaration
%type<ast> kw_type
%type<ast> literal
%type<ast> expression
%type<ast> func_call_arg_list
%type<ast> var_declaration
%type<ast> vector_declaration
%type<ast> vector_tail
%type<ast> func_declaration
%type<ast> arg_list
%type<ast> arg
%type<ast> command_block
%type<ast> command_list
%type<ast> command
%type<ast> output_arg_list
%type<ast> output_arg

// operator precedence
%left '~' '&' '|'
%left '<' '>' OPERATOR_EQ OPERATOR_DIF OPERATOR_GE OPERATOR_LE
%left '+' '-'
%left '*' '/'

%%

program: global_declaration_list          {
                                          $$ = $1;
                                          global_ast = $$;
                                          check_and_set_declarations(global_ast);
                                          check_undeclared();
                                          check_expressions(global_ast);
                                          check_assignments(global_ast);
                                          astPrint(global_ast, 0);
                                          tacPrintBackwards(generateCode(global_ast));
                                          }
       ;


global_declaration_list: global_declaration global_declaration_list   { $$ = astCreate(AST_GLOBAL_DEC_LIST, 0, $1, $2, 0, 0); }
                       |                                              { $$ = 0; }
                       ;


global_declaration: var_declaration ';'          { $$ = $1; }
                  | func_declaration             { $$ = $1; }
                  ;

var_declaration: kw_type TK_IDENTIFIER '=' literal      { $$ = astCreate(AST_VAR_DEC, 0, $1, astCreate(AST_SYMBOL, $2, 0, 0, 0, 0), $4, 0); }
               | vector_declaration                     { $$ = $1; }
               ;

kw_type: KW_CHAR            { $$ = astCreate(AST_TYPE_CHAR, 0, 0, 0, 0, 0); }
       | KW_INT             { $$ = astCreate(AST_TYPE_INT,  0, 0, 0, 0, 0); }
       | KW_REAL            { $$ = astCreate(AST_TYPE_REAL, 0, 0, 0, 0, 0); }
       | KW_BOOL            { $$ = astCreate(AST_TYPE_BOOL, 0, 0, 0, 0, 0); }
       ;

literal: LIT_INT            { $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
       | LIT_REAL           { $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
       | LIT_CHAR           { $$ = astCreate(AST_SYMBOL_CHAR, $1, 0, 0, 0, 0); }
       ;

vector_declaration: kw_type TK_IDENTIFIER '[' LIT_INT ']' vector_tail        { $$ = astCreate(AST_VEC_DEC, 0, $1, astCreate(AST_SYMBOL, $2, 0, 0, 0, 0), astCreate(AST_SYMBOL, $4, 0, 0, 0, 0), $6); }
                  ;

vector_tail: literal vector_tail   { $$ = astCreate(AST_VEC_TAIL, 0, $1, $2, 0, 0); }
           |                       { $$ = 0; }
           ;

func_declaration: kw_type TK_IDENTIFIER '(' arg_list ')' command_block       { $$ = astCreate(AST_FUNC_DEC, 0, $1, astCreate(AST_SYMBOL, $2, 0, 0, 0, 0), $4, $6); }
                | kw_type TK_IDENTIFIER '(' ')' command_block                { $$ = astCreate(AST_FUNC_DEC, 0, $1, astCreate(AST_SYMBOL, $2, 0, 0, 0, 0), 0, $5); }
                ;

arg_list: arg                                    { $$ = astCreate(AST_ARG_LIST, 0, $1, 0, 0, 0); }
        | arg ',' arg_list                       { $$ = astCreate(AST_ARG_LIST, 0, $1, $3, 0, 0); }
        ;

arg: kw_type TK_IDENTIFIER                       { $$ = astCreate(AST_ARG, 0, $1, astCreate(AST_SYMBOL, $2, 0, 0, 0, 0), 0, 0); }
   ;

command_block: '{' command_list '}'              { $$ = astCreate(AST_CMD_BLOCK, 0, $2, 0, 0, 0); }
             ;

command_list: command command_list               { $$ = astCreate(AST_CMD_LIST, 0, $1, $2, 0, 0); }
            |                                    { $$ = 0; }
            ;

command: TK_IDENTIFIER '=' expression ';'                      { $$ = astCreate(AST_VAR_ASSIGN, 0, astCreate(AST_SYMBOL, $1, 0, 0, 0, 0), $3, 0, 0); }
       | TK_IDENTIFIER '[' expression ']' '=' expression ';'   { $$ = astCreate(AST_VEC_ASSIGN, 0, astCreate(AST_SYMBOL, $1, 0, 0, 0, 0), $3, $6, 0); }
       | KW_IF '(' expression ')' command                      { $$ = astCreate(AST_IF, 0, $3, $5, 0, 0); }
       | KW_IF '(' expression ')' command KW_ELSE command      { $$ = astCreate(AST_IF_ELSE, 0, $3, $5, $7, 0); }
       | KW_IF '(' expression ')' KW_LOOP command              { $$ = astCreate(AST_IF_LOOP, 0, $3, $6, 0, 0); }
       | KW_OUTPUT output_arg_list ';'                         { $$ = astCreate(AST_OUTPUT, 0, $2, 0, 0, 0); }
       | KW_RETURN expression ';'                              { $$ = astCreate(AST_RETURN, 0, $2, 0, 0, 0); }
       | command_block                                         { $$ = $1; }
       | ';'                                                   { $$ = astCreate(AST_EMPTY_CMD, 0, 0, 0, 0, 0); }
       ;

output_arg_list: output_arg                             { $$ = astCreate(AST_OUTPUT_ARG, 0, $1, 0, 0, 0); }
               | output_arg ',' output_arg_list         { $$ = astCreate(AST_OUTPUT_ARG, 0, $1, $3, 0, 0); }
               ;

output_arg: LIT_STRING                                  { $$ = astCreate(AST_SYMBOL_STRING, $1, 0, 0, 0, 0); }
          | expression                                  { $$ = $1; }
          ;

expression: literal                                     { $$ = $1; }
          | TK_IDENTIFIER                               { $$ = astCreate(AST_VAR_ACCESS, $1, 0, 0, 0, 0); }
          | TK_IDENTIFIER '[' expression ']'            { $$ = astCreate(AST_VEC_ACCESS, 0, astCreate(AST_SYMBOL, $1, 0, 0, 0, 0), $3, 0, 0); }
          | expression '+' expression                   { $$ = astCreate(AST_ADD, 0, $1, $3, 0, 0); }
          | expression '-' expression                   { $$ = astCreate(AST_SUB, 0, $1, $3, 0, 0); }
          | expression '*' expression                   { $$ = astCreate(AST_MULT, 0, $1, $3, 0, 0); }
          | expression '/' expression                   { $$ = astCreate(AST_DIV, 0, $1, $3, 0, 0); }
          | expression '>' expression                   { $$ = astCreate(AST_BIGG, 0, $1, $3, 0, 0); }
          | expression '<' expression                   { $$ = astCreate(AST_LESS, 0, $1, $3, 0, 0); }
          | expression OPERATOR_LE expression           { $$ = astCreate(AST_LE, 0, $1, $3, 0, 0); }
          | expression OPERATOR_GE expression           { $$ = astCreate(AST_GE, 0, $1, $3, 0, 0); }
          | expression OPERATOR_EQ expression           { $$ = astCreate(AST_EQ, 0, $1, $3, 0, 0); }
          | expression OPERATOR_DIF expression          { $$ = astCreate(AST_DIF, 0, $1, $3, 0, 0); }
          | expression '&' expression                   { $$ = astCreate(AST_AND, 0, $1, $3, 0, 0); }
          | expression '|' expression                   { $$ = astCreate(AST_OR, 0, $1, $3, 0, 0); }
          | '~' expression                              { $$ = astCreate(AST_NEG, 0, $2, 0, 0, 0); }
          | '(' expression ')'                          { $$ = astCreate(AST_NESTED_EXPR, 0, $2, 0, 0, 0); }
          | TK_IDENTIFIER '(' ')'                       { $$ = astCreate(AST_FUNC_CALL, 0, astCreate(AST_SYMBOL, $1, 0, 0, 0, 0), 0, 0, 0); }
          | TK_IDENTIFIER '(' func_call_arg_list ')'    { $$ = astCreate(AST_FUNC_CALL, 0, astCreate(AST_SYMBOL, $1, 0, 0, 0, 0), $3, 0, 0); }
          | KW_INPUT '(' kw_type ')'                    { $$ = astCreate(AST_INPUT, 0, $3, 0, 0, 0); }
          ;

func_call_arg_list: expression                          { $$ = astCreate(AST_FUNC_CALL_ARG, 0, $1, 0, 0, 0); }
                  | expression ',' func_call_arg_list   { $$ = astCreate(AST_FUNC_CALL_ARG, 0, $1, $3, 0, 0); }
                  ;

%%

int yyerror(char const *s)
{
  fprintf(stderr, "\nCompilation failed: syntax error at line %d.\n", getLineNumber());
  exit(3);
}
