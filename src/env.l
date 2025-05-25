%{
#include "options.h"
#include "env_parser.h"
#include "utils.h"
extern void enverror(const char *s);
%}

%option reentrant noyywrap never-interactive nounistd bison-bridge

key     [a-zA-Z_]+[a-zA-Z0-9_]*
value   =[^\n#]*

%%

"#"[^\n]*  { continue;  }
[ \r\t]    { continue; }
\n         { continue; }

{key}      { strncpy(yylval->string, yytext, 1023); return KEY; }
{value}    { strncpy(yylval->string, yytext+1, 1023); return VALUE;  }

[ \t\r\n]+ {  }

.          { KFS_ERROR("Lexical Error: unrecognized literal '%s'", yytext); }

%%