%{
#include "json_parser.h"
#include "utils.h"
extern void zzerror(const char *s);
%}

%option noyywrap
%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

integer   -?(0|[1-9][0-9]*)
number    -?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?

%%

{integer}  { sscanf(yytext, "%d", &zzlval.integer);  return INTEGER; }
{number}   { sscanf(yytext, "%lf", &zzlval.decimal); return DECIMAL; }
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

\"[^"]*\"  { zzlval.string = strdup(yytext); return STRING; }

.          { KFS_ERROR("Lexical Error: unrecognized literal '%s'", yytext); }

%%