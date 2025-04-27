%{

#include "expression.h"
#include "parser.h"

#include <stdio.h>

%}

%option outfile="../out/lexer.c" header-file="../out/lexer.h"
%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

IDENT   [a-zA-Z_][0-9a-zA-Z_]*
DIGIT   [0-9]
LONG   -?(0|[1-9][0-9]*)
DOUBLE -?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?

ESCHAR  ("\\n")|("\\t")|("\\r")|("\\\\")|("\\\"")
STRING_SQ  ("\'")(({ESCHAR})|([^\\\'\"]))*("\'")
STRING_DQ  ("\"")(({ESCHAR})|([^\\\'\"]))*("\"")
STRING  ({STRING_SQ})|({STRING_DQ})

%x comment

%%
"//"[^\n]*  { continue;  }
[ \r\t]     { continue; }
\n          { continue; }

{LONG}      { sscanf(yytext, "%d", &yylval->lValue); return TOKEN_NUMBER; }
{DOUBLE}    { sscanf(yytext, "%lf", &yylval->dValue); return TOKEN_DOUBLE; }
"true"      { yylval->lValue = 1; return TOKEN_BOOL; }
"false"     { yylval->lValue = 0; return TOKEN_BOOL; }
"*"         { return TOKEN_STAR; }
"+"         { return TOKEN_PLUS; }
"-"         { return TOKEN_MINUS; }
"/"         { return TOKEN_DIV; }
"%"         { return TOKEN_MOD; }
"^"         { return TOKEN_POW; }
"("         { return TOKEN_LPAREN; }
")"         { return TOKEN_RPAREN; }
"<"         { return TOKEN_LT;  }
"<="        { return TOKEN_LE;  }
"=="        { return TOKEN_EQ;  }
"!="        { return TOKEN_NE;  }
">"         { return TOKEN_GT;  }
">="        { return TOKEN_GE;  }
"&&"        { return TOKEN_AND; }
"||"        { return TOKEN_OR;  }
"!"         { return TOKEN_NOT; }

"/*"					BEGIN(comment);
<comment>[^*\n]*
<comment>"*"+[^*/\n]*
<comment>\n
<comment>"*"+"/"        BEGIN(INITIAL);

.           { fprintf(stderr, "Lexical Error: unrecognized literal '%s'", yytext); }

%%

int yyerror(Expression **expression, yyscan_t scanner, const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    return 0;
}
