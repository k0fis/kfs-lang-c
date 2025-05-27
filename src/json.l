%{
#include "value.h"
#include "options.h"
#include "json_parser.h"
#include "utils.h"
extern void zzerror(const char *s);
%}

%option noyywrap

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

integer   -?(0|[1-9][0-9]*)
number    -?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?

%%

{integer}  { sscanf(yytext, "%d", &yylval->integer);  return INTEGER; }
{number}   { sscanf(yytext, "%lf", &yylval->decimal); return DECIMAL; }
"{"        { return LCURLY; }
"}"        { return RCURLY; }
"["        { return LBRAC;  }
"]"        { return RBRAC;  }
","        { return COMMA;  }
":"        { return COLON;  }
"true"     { return VTRUE;  }
"false"    { return VFALSE; }
"null"     { return VNULL;  }

[ \t\r\n]+ {  }

\"[^"]*\"  { yylval->string = yytext+1; yylval->string[strlen(yylval->string)-1]='\0'; return STRING; }

.          { KFS_ERROR("Lexical Error: unrecognized literal '%s'", yytext); }

%%