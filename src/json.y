%{

#include "value.h"
#include "json_parser.h"
#include "json_lexer.h"
#include "utils.h"

void zzerror(Value **output, yyscan_t scanner, const char *msg);

%}

%code requires {
  typedef void* zzscan_t;
}

%define api.pure
%lex-param   { zzscan_t scanner }
%parse-param { Value  **output  }
%parse-param { zzscan_t scanner }

%union {
  char *string;
  double decimal;
  int integer;
  Value *value;
}

%token LCURLY RCURLY LBRAC RBRAC COMMA COLON
%token VTRUE VFALSE VNULL
%token <string> STRING;
%token <decimal> DECIMAL;
%token <integer> INTEGER;

%type <value> value o_members a_members

%start json

%%

json:
    | value   { *output = $1; }
    ;

value
     : LCURLY o_members[M] RCURLY { $$ = $M; }
     | STRING                     { $$ = value_new_string($1);}
     | INTEGER                    { $$ = value_new_int($1); }
     | DECIMAL                    { $$ = value_new_double($1); }
     | LBRAC a_members[A] RBRAC   { $$ = $A; }
     | VTRUE                      { $$ = value_new_bool(1); }
     | VFALSE                     { $$ = value_new_bool(0); }
     | VNULL                      { $$ = value_new_list(); }
     ;

o_members : o_members[L] COMMA STRING[N] COLON value[V] {
                if ($L == NULL) $L = value_new_object(); value_object_add($L, $N, $V); $$ = $L; }
          | STRING[N] COLON value[V] { $$ = value_new_object(); value_object_add($$, $N, $V); }
          |  { $$ = value_new_object(); }
          ;

a_members:                     { $$ = value_new_list(); }
      | value                  { $$ = value_new_list(); value_list_add($$, $1); }
      | a_members COMMA value  { if ($1 == NULL) $1 = value_new_list(); value_list_add($1, $3); $$ = $1; }
      ;

%%

void zzerror(Value **output, yyscan_t scanner, const char *msg){
  KFS_ERROR("json-error: %s", msg);
}