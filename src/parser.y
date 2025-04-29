%{

#include "utils.h"
#include "parser.h"
#include "lexer.h"

int yyerror(Expression **expression, yyscan_t scanner, const char *msg);

%}

%code requires {
  typedef void* yyscan_t;
}

%output  "../out/parser.c"
%defines "../out/parser.h"

%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { Expression **expression }
%parse-param { yyscan_t scanner }

%union {
    int  lValue;
    double dValue;
    char buffer[1024];
    Expression *expression;
}

%token TOKEN_LPAREN   "("
%token TOKEN_LBRACK   "["
%token TOKEN_LCURLY   "{"
%token TOKEN_RPAREN   ")"
%token TOKEN_RBRACK   "]"
%token TOKEN_RCURLY   "}"
%token TOKEN_PLUS     "+"
%token TOKEN_MINUS    "-"
%token TOKEN_STAR     "*"
%token TOKEN_DIV      "/"
%token TOKEN_MOD      "%"
%token TOKEN_POW      "^"
%token TOKEN_LT       "<"
%token TOKEN_LE       "<="
%token TOKEN_EQ       "=="
%token TOKEN_NE       "!="
%token TOKEN_GT       ">"
%token TOKEN_GE       ">="
%token TOKEN_AND      "&&"
%token TOKEN_OR       "||"
%token TOKEN_NOT      "!"
%token TOKEN_DOT      "."
%token TOKEN_COMA     ","
%token TOKEN_COLON    ":"
%token TOKEN_SEMI     ";"
%token TOKEN_INT      "int"

%token <lValue> TOKEN_NUMBER "number"
%token <dValue> TOKEN_DOUBLE "double"
%token <lValue> TOKEN_BOOL   "bool"
%token <buffer> TOKEN_IDENT  "ID"
%token <buffer> TOKEN_STR  "str"

%type <expression> expr
%type <expression> expr_list
%type <expression> named_list

/* Precedence (increasing) and associativity:
   a+b+c is (a+b)+c: left associativity
   a+b*c is a+(b*c): the precedence of "*" is higher than that of "+". */
   %right "["
   %right "]"

%left "+"
%left "-"
%left "*"
%left "/"
%left "%"
%left "^"
%right "!"
%right "!="
%right "=="
%right "<"
%right "<="
%right ">"
%right ">="

%%

input
    : expr { *expression = $1; }
    ;

expr
    : expr[L] "["  expr[N] "]" { $$ = expression_create_binary_operation(eARRAY_ACCESS, $L, $N); }
    | expr[L] "+"  expr[R] { $$ = expression_create_binary_operation( eADD, $L, $R ); }
    | expr[L] "-"  expr[R] { $$ = expression_create_binary_operation( eMINUS, $L, $R ); }
    | expr[L] "*"  expr[R] { $$ = expression_create_binary_operation( eMULTIPLY, $L, $R ); }
    | expr[L] "/"  expr[R] { $$ = expression_create_binary_operation( eDIVIDE, $L, $R ); }
    | expr[L] "%"  expr[R] { $$ = expression_create_binary_operation( eMODULO, $L, $R ); }
    | expr[L] "^"  expr[R] { $$ = expression_create_binary_operation( ePOWER, $L, $R ); }
    | expr[L] "<"  expr[R] { $$ = expression_create_binary_operation( eLT, $L, $R ); }
    | expr[L] "<=" expr[R] { $$ = expression_create_binary_operation( eLE, $L, $R ); }
    | expr[L] "==" expr[R] { $$ = expression_create_binary_operation( eEQ, $L, $R ); }
    | expr[L] "!=" expr[R] { $$ = expression_create_binary_operation( eNE, $L, $R ); }
    | expr[L] ">"  expr[R] { $$ = expression_create_binary_operation( eGT, $L, $R ); }
    | expr[L] ">=" expr[R] { $$ = expression_create_binary_operation( eGE, $L, $R ); }
    | expr[L] "&&" expr[R] { $$ = expression_create_binary_operation( eAND, $L, $R); }
    | expr[L] "||" expr[R] { $$ = expression_create_binary_operation( eOR, $L, $R);  }
    |         "!"  expr[L] { $$ = expression_create_binary_operation( eNOT, $L, NULL); }
              %prec "!"
    |         "-"  expr[L]   { $$ = expression_create_binary_operation( eUNARY_MINUS, $L, NULL); }
              %prec "!"
    | "int" "(" expr[E] ")"  { $$ = expression_create_binary_operation( eINT, $E, NULL); }
    | "(" expr[E] ")"        { $$ = $E; }
    | "[" expr_list[E] "]"   { $$ = $E; }
    | "{" named_list[E] "}"  { $$ = $E; }
    | expr[L] "." "ID"[N]    { $$ = expression_create_dot_operation($L, $N); }
    | "number"               { $$ = expression_create_integer($1); }
    | "double"               { $$ = expression_create_double($1); }
    | "bool"                 { $$ = expression_create_bool($1); }
    | "str"                  { $$ = expression_create_string($1); }
    ;

expr_list
    : expr_list[L] "," expr[R] { if ($L == NULL) $L = expression_create_list(); $$ = expression_add_list_item($L, $R); }
    | expr[R]                  { $$ = expression_create_list(); expression_add_list_item($$, $R); }
    |                          { $$ = expression_create_list(); }
    ;

named_list
    : named_list[L] ";" "ID"[N] ":" expr[R] { if ($L == NULL) $L = expression_create_object();
                            $$ = expression_add_object_item($L, $N, $R); }
    | "ID"[N] ":" expr[R] { $$ = expression_create_object(); expression_add_object_item($$, $N, $R); }
    |                     { $$ = expression_create_object(); }
    ;
%%