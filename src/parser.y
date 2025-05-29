%{

#include "options.h"
#include "kfs_lang_env.h"
#include "utils.h"
#include "parser.h"
#include "lexer.h"

int yyerror(KfsLangEnv *kfsLangEnv, yyscan_t scanner, Options* options, const char *msg);

%}

%code requires {
  typedef void* yyscan_t;
}

%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { KfsLangEnv *kfsLangEnv }
%parse-param { yyscan_t scanner }
%parse-param { Options * options }
%define parse.error verbose

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
%token TOKEN_ASSIGN   "="
%token TOKEN_IF       "if"
%token TOKEN_ELSE     "else"
%token TOKEN_WHILE    "while"
%token TOKEN_BREAK    "break"
%token TOKEN_CONTI    "continue"
%token TOKEN_RETURN   "return"
%token TOKEN_PRINT    "print"
%token TOKEN_EMPTY    "empty"

%token <lValue> TOKEN_NUMBER "number"
%token <dValue> TOKEN_DOUBLE "double"
%token <lValue> TOKEN_BOOL   "bool"
%token <buffer> TOKEN_IDENT  "ID"
%token <buffer> TOKEN_STR    "str"

%type <expression> expr input inpt command command_list command_list_backet expr_list named_list

%right "="
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

input: inpt { kfsLangEnv->expression = $1; }
;
inpt
    : expr[R]            { $$ = expression_delist($R); }
    | expr_list[R]       { $$ = expression_delist($R); }
    | command_list[C]         { $$ = expression_delist($C); }
    | command_list_backet[C]  { $$ = expression_delist($C); }
    ;

expr
    : expr[L] "["  expr[N] "]"  { $$ = expression_create_binary_operation(eARRAY_ACCESS, $L, $N); }
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
    | "empty""(""ID"[N]")"   { $$ = expression_create_is_empty($N); }
    | expr[L] "." "ID"[N]    { $$ = expression_create_dot_operation($L, $N); }
    | "number"               { $$ = expression_create_integer($1); }
    | "double"               { $$ = expression_create_double($1); }
    | "bool"                 { $$ = expression_create_bool($1); }
    | "str"                  { $$ = expression_create_string($1); }
    | "ID"[N]"(" named_list[P] ")" { $$ = expression_create_function_call($N, $P); }
    | "ID"[N]                { $$ = expression_create_variable($N); }
    ;

expr_list
    : expr_list[L] "," expr[R] { if ($L == NULL) $L = expression_create_list(); $$ = expression_add_list_item($L, $R); }
    | expr[R]                  { $$ = expression_add_list_item(expression_create_list(), $R); }
    |                          { $$ = expression_create_list(); }
    ;
command_list_backet
    : "{" command_list[R] "}"    { $$ = expression_create_new_block(expression_delist($R)); }
    ;
command_list
    : command_list[L] command[R] { if ($L == NULL) $L = expression_create_list(); $$ = expression_add_list_item($L, $R); }
    | command[R]                 { $$ = expression_add_list_item(expression_create_list(), $R); }
    |                            { $$ = expression_create_list(); }
    ;
named_list
    : named_list[L] ";" "ID"[N] ":" expr[R] { if ($L == NULL) $L = expression_create_object();
                            $$ = expression_add_object_item($L, $N, $R); }
    | "ID"[N] ":" expr[R] { $$ = expression_create_object(); expression_add_object_item($$, $N, $R); }
    |                     { $$ = expression_create_object(); }
    ;
command
    : "ID"[N] "=" "empty" ";"           { $$ = expression_create_variable_empty($N); }
    | "ID"[N] "=" expr[R] ";"           { $$ = expression_create_variable_assign($N, $R); }
    | "if" "(" expr[Q] ")" inpt[T]      { $$ = expression_create_if($Q, $T, NULL); }
    | "if" "(" expr[Q] ")" inpt[T] "else" inpt[F] {
                                          $$ = expression_create_if($Q, $T, $F); }
    | "while" "(" expr[Q] ")" inpt[B]   { $$ = expression_create_while($Q, $B); }
    | "break" ";"                       { $$ = expression_create_break(); }
    | "continue" ";"                    { $$ = expression_create_continue(); }
    | "print" "(" expr[Q] ")" ";"       { $$ = expression_create_print($Q); }
    | "return" expr_list[L] ";"         { $$ = expression_create_return($L); }
    | "ID"[N]"("")""{"inpt[C]"}"        { $$ = expression_create_function($N, $C); }
    | ";" {}
    ;
%%