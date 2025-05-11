%{

#include "utils.h"

int zzlex();
void zzerror(const char *s);

%}

%token LCURLY RCURLY LBRAC RBRAC COMMA COLON
%token VTRUE VFALSE VNULL
%token <string> STRING;
%token <decimal> DECIMAL;
%token <integer> INTEGER;

%union {
  char *string;
  double decimal;
  int integer;
}

%start json

%%

json:
    | value
    ;

value: object
     | STRING
     | DECIMAL
     | array
     | VTRUE
     | VFALSE
     | VNULL
     ;

object: LCURLY RCURLY
      | LCURLY members RCURLY
      ;

members: member
       | members COMMA member
       ;

member: STRING COLON value
      ;

array: LBRAC RBRAC
     | LBRAC values RBRAC
     ;

values: value
      | values COMMA value
      ;

%%

void zzerror(const char *s){
  KFS_ERROR("json-error: %s", s);
}