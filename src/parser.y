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
%token TOKEN_LT  LT
%token TOKEN_LE  LE
%token TOKEN_EQ  EQ
%token TOKEN_NE  NE
%token TOKEN_GT  GT
%token TOKEN_GE  GE
%token TOKEN_AND AND
%token TOKEN_OR  OR
%token TOKEN_NOT NOT

%token <lValue> TOKEN_NUMBER "number"
%token <dValue> TOKEN_DOUBLE "double"
%token <lValue> TOKEN_BOOL   "bool"

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
%right NOT

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
    | expr[L] LT  expr[R] { $$ = expression_create_binary_operation( eLT, $L, $R ); }
    | expr[L] LE  expr[R] { $$ = expression_create_binary_operation( eLE, $L, $R ); }
    | expr[L] EQ  expr[R] { $$ = expression_create_binary_operation( eEQ, $L, $R ); }
    | expr[L] NE  expr[R] { $$ = expression_create_binary_operation( eNE, $L, $R ); }
    | expr[L] GT  expr[R] { $$ = expression_create_binary_operation( eGT, $L, $R ); }
    | expr[L] GE  expr[R] { $$ = expression_create_binary_operation( eGE, $L, $R ); }
    | expr[L] AND expr[R] { $$ = expression_create_binary_operation( eAND, $L, $R); }
    | expr[L] OR  expr[R] { $$ = expression_create_binary_operation( eOR, $L, $R);  }
    |         NOT expr[L] { $$ = expression_create_binary_operation( eNOT, $L, NULL); }
              %prec NOT
    | "(" expr[E] ")"     { $$ = $E; }
    | "number"            { $$ = expression_create_integer($1); }
    | "double"            { $$ = expression_create_double($1); }
    | "bool"              { $$ = expression_create_bool($1); }
    ;

%%