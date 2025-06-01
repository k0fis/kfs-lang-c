%{

#include "kfs_dict.h"
#include "kfs_lang_env.h"
#include "options.h"
#include "utils.h"
#include "parser.h"

#include <stdio.h>
%}

%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

IDENT   [a-zA-Z_][0-9a-zA-Z_]*
DIGIT   [0-9]
LONG    (0|[1-9][0-9]*)
DOUBLE  (0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?

ESCHAR  ("\\n")|("\\t")|("\\r")|("\\\\")|("\\\"")
STRING_SQ  ("\'")(({ESCHAR})|([^\\\'\"]))*("\'")
STRING_DQ  ("\"")(({ESCHAR})|([^\\\'\"]))*("\"")
STRING  ({STRING_SQ})|({STRING_DQ})

%x comment
%x string

%%
"//"[^\n]*  { continue;  }
[ \r\t]     { continue; }
\n          { continue; }

{LONG}      { sscanf(yytext, "%d", &yylval->lValue); return TOKEN_NUMBER; }
{DOUBLE}    { sscanf(yytext, "%lf", &yylval->dValue); return TOKEN_DOUBLE; }
"true"      { yylval->lValue = TRUE; return TOKEN_BOOL; }
"false"     { yylval->lValue = FALSE; return TOKEN_BOOL; }
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
"["         { return TOKEN_LBRACK; }
"]"         { return TOKEN_RBRACK; }
","         { return TOKEN_COMA; }
":"         { return TOKEN_COLON; }
";"         { return TOKEN_SEMI; }
"."         { return TOKEN_DOT; }
"{"         { return TOKEN_LCURLY; }
"}"         { return TOKEN_RCURLY; }
"int"       { return TOKEN_INT; }
"if"        { return TOKEN_IF; }
"else"      { return TOKEN_ELSE; }
"while"     { return TOKEN_WHILE; }
"break"     { return TOKEN_BREAK; }
"continue"  { return TOKEN_CONTI; }
"return"    { return TOKEN_RETURN; }
"print"     { return TOKEN_PRINT; }
"dump"      { return TOKEN_DUMP; }
"json"      { return TOKEN_JSON; }
"eval"      { return TOKEN_EVAL; }
"readfile"  { return TOKEN_READ_FILE; }
"empty"     { return TOKEN_EMPTY; }
{IDENT}     { strncpy(yylval->buffer, yytext, 1023); return TOKEN_IDENT; }
{STRING}    { strncpy(yylval->buffer, yytext, 1023); return TOKEN_STR; }
"="         { return TOKEN_ASSIGN; }

"/*"					BEGIN(comment);
<comment>[^*\n]*
<comment>"*"+[^*/\n]*
<comment>\n
<comment>"*"+"/"        BEGIN(INITIAL);

.           { fprintf(stderr, "Lexical Error: unrecognized literal '%s'", yytext); }

%%

int yyerror(KfsLangEnv *kfsLangEnv, yyscan_t scanner, Options *options, const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    return 0;
}
