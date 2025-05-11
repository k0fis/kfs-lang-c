package kfs.lang.json;

import java_cup.runtime.SymbolFactory;
import java.lang.Exception;

%%
%cup
%public
%unicode
%line
%column
%class JsonLex
%{
    public JsonLex(java.io.Reader r, SymbolFactory sf){
        this(r);
        this.sf=sf;
    }
    private SymbolFactory sf;;
%}
%eofval{
    return sf.newSymbol("EOF",sym.EOF);
%eofval}

/*  A JSON char is either any Unicode character except " or \ or a control-character,	[^\\\"\u0000-\u001f]*
or it's one of the following:
    \"
    \\
    \/
    \b
    \f
    \n
    \r
    \t											\\[\"\\bfnrt\/]
    \u four-hex-digits									\\u[0-9A-Fa-f]{4}

Note that extra backslashes are required to terminate " in JFlex
*/
char = [^\\\"\u0000-\u001f]*|\\[\"\\bfnrt\/]|\\u[0-9A-Fa-f]{4}

string = \"{char}*\"

integer = -?(0|[1-9][0-9]*)
number  = -?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?

whitespace = [ \t\r\n\f]

%%
"," { return sf.newSymbol("Comma",sym.COMMA); }
":" { return sf.newSymbol("Colon",sym.COLON); }

"{" { return sf.newSymbol("Left Brace",sym.LBRACE); }
"}" { return sf.newSymbol("Right Brace",sym.RBRACE); }

"[" { return sf.newSymbol("Left Square Bracket",sym.LSQBRACKET); }
"]" { return sf.newSymbol("Right Square Bracket",sym.RSQBRACKET); }

"true" { return sf.newSymbol("Boolean True",sym.TRUE); }
"false" { return sf.newSymbol("Boolean False",sym.FALSE); }

"null" { return sf.newSymbol("Null",sym.NULL); }

{integer} { return sf.newSymbol("Integer",sym.INTEGER, Integer.parseInt(yytext())); }
{number}  { return sf.newSymbol("Number",sym.NUMBER, Double.parseDouble(yytext())); }
{string}  { return sf.newSymbol("String",sym.STRING, yylength()<=2?"":yytext().substring(1,yylength()-2)); }

{whitespace} {  }

// Pass error to CUP on encountering illegal characters, triggering failure exit code
. { throw new Error("Illegal character: " + yytext() + " at line " + (yyline + 1) + ", column " + (yycolumn + 1)); }