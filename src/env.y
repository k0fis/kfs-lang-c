%{

#include "options.h"
#include "env_parser.h"
#include "env_lexer.h"
#include "env.h"
#include "utils.h"

void enverror(envscan_t scanner, Options*, const char *msg);

%}

%code requires {
  typedef void* envscan_t;
}

%define api.pure
%lex-param   { envscan_t scanner }
%parse-param { envscan_t scanner }
%parse-param { Options *options }
%define parse.error verbose

%union {
  char string[1024];
}

%token <string> KEY
%token <string> VALUE

%type <string> assign

%start env

%%

env : assign
    | env assign
    ;

assign: KEY[k] VALUE[v]   { env_set_env($k, $v, options); }
%%

void enverror(envscan_t scanner, Options *options, const char *msg){
  KFS_ERROR("env-error: %s", msg);
}