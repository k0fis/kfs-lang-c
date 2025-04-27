%{

#include "expression.h"
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
    Expression *expression;
}

%token TOKEN_LPAREN   "("
%token TOKEN_RPAREN   ")"
%token TOKEN_PLUS     "+"
%token TOKEN_MINUS    "-"
%token TOKEN_STAR     "*"
%token TOKEN_DIV      "/"
%token TOKEN_MOD      "%"
%token TOKEN_POW      "^"
%token <lValue> TOKEN_NUMBER "number"
%token <dValue> TOKEN_DOUBLE "double"

%type <expression> expr

/* Precedence (increasing) and associativity:
   a+b+c is (a+b)+c: left associativity
   a+b*c is a+(b*c): the precedence of "*" is higher than that of "+". */
%left "+"
%left "-"
%left "*"
%left "/"
%left "%"
%left "^"

%%

input
    : expr { *expression = $1; }
    ;

expr
    : expr[L] "+" expr[R] { $$ = expression_create_binary_operation( eADD, $L, $R ); }
    | expr[L] "-" expr[R] { $$ = expression_create_binary_operation( eMINUS, $L, $R ); }
    | expr[L] "*" expr[R] { $$ = expression_create_binary_operation( eMULTIPLY, $L, $R ); }
    | expr[L] "/" expr[R] { $$ = expression_create_binary_operation( eDIVIDE, $L, $R ); }
    | expr[L] "%" expr[R] { $$ = expression_create_binary_operation( eMODULO, $L, $R ); }
    | expr[L] "^" expr[R] { $$ = expression_create_binary_operation( ePOWER, $L, $R ); }
    | "(" expr[E] ")"     { $$ = $E; }
    | "number"            { $$ = expression_create_integer($1); }
    | "double"            { $$ = expression_create_double($1); }
    ;

%%