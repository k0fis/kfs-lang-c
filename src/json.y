package kfs.lang.json;

import java_cup.runtime.*;
import kfs.lang.syntax.*;

scan with {: return lex.next_token(); :};

parser code {:
  JsonLex lex;
  ExpressionList expressionList;
  public Parser(JsonLex lex, ExpressionList expressionList) {
   super(lex);
   this.lex = lex;
   this.expressionList = expressionList;
  }
:}

terminal COMMA, COLON, LSQBRACKET, RSQBRACKET, LBRACE, RBRACE, STRING, NUMBER, INTEGER, TRUE, FALSE, NULL;

non terminal IExpression value, json;
non terminal MapExpression key_value_list, object;
non terminal ArrayExpression value_list, array;

json ::= value:v {: expressionList.add(v); :}
;

value ::= STRING:s  {: RESULT = new ValueExpressionString((String)s); :}
        | INTEGER:i {: RESULT = new ValueExpressionInt((Integer)i); :}
        | NUMBER:n  {: RESULT = new ValueExpressionDouble((Double)n); :}
        | object:o  {: RESULT = o; :}
        | array:a   {: RESULT = a; :}
        | TRUE      {: RESULT = new ValueExpressionBool(Boolean.TRUE); :}
        | FALSE     {: RESULT = new ValueExpressionBool(Boolean.FALSE); :}
        | NULL      {: RESULT = new ArrayExpression(); :}
;

object ::= LBRACE RBRACE                    {: RESULT = new MapExpression(); :}
         | LBRACE key_value_list:o RBRACE   {: RESULT = o; :}
;

array ::= LSQBRACKET RSQBRACKET               {: RESULT = new ArrayExpression(); :}
        | LSQBRACKET value_list:a RSQBRACKET  {: RESULT = a; :}
;

key_value_list ::= key_value_list:l COMMA STRING:n COLON value:v
                                           {: if (l==null) l = new MapExpression(); RESULT = l.add(n.toString(),v); :}
                 | STRING:n COLON value:v  {: RESULT = new MapExpression().add(n.toString(), v); :}
 ;

value_list ::= value_list:l COMMA value:v
                        {: if (l==null) l = new ArrayExpression(); RESULT = l.add(v); :}
             | value:v  {: RESULT = new ArrayExpression().add(v); :}
 ;