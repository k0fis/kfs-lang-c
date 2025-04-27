#include "expression.h"
#include "parser.h"
#include "lexer.h"
#include "value.h"
#include "eval.h"

#include <stdio.h>

int yyparse(Expression **expression, yyscan_t scanner);

Expression *getAST(const char *expr)
{
    Expression *expression;
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    if (yylex_init(&scanner)) {
        /* could not initialize */
        return NULL;
    }

    state = yy_scan_string(expr, scanner);

    if (yyparse(&expression, scanner)) {
        /* error parsing */
        return NULL;
    }

    yy_delete_buffer(state, scanner);

    yylex_destroy(scanner);

    return expression;
}


int main(void) {
   Value *v, *w, *q;
   v = value_new_list();
   value_list_add(v, value_new_int(1));
   value_list_add(v, value_new_int(2));
   value_list_add(v, value_new_double(2));
   value_list_add(v, value_new_string("youda"));
   value_print(v); //value_delete(v);
   NamedValue *nv = named_value_new("youda", v);
   printf("  ");named_value_print(nv); named_value_delete(nv);

   v = value_new_object();
   value_object_add(v, "s", value_new_string("pica"));
   value_object_add(v, "pako", value_new_bool(1));
   value_object_add(v, "db1", value_new_double(1));
   value_object_add(v, "db2", value_new_double(1));
   value_object_add(v, "db3", value_new_double(2));
   value_object_add(v, "db3", value_new_double(3));
   value_object_add(v, "db3", value_new_double(4));
   value_print(v); value_delete(v);

   v = value_new_list();value_print(v); value_delete(v);

   w = value_new_int(1); q = value_new_double(1.);
   v = value_eq(w,q);if (!v->iValue) printf("bad test eq"); value_delete(v);value_delete(w); value_delete(q);

   w = value_new_string("pako"); q = value_new_string("pako");
   v = value_eq(w,q); if (!v->iValue) printf("bad test string eq");; value_delete(v);value_delete(w); value_delete(q);

   w = value_new_list(); q = value_new_list();
   value_list_add(w, value_new_int(1));
   value_list_add(w, value_new_int(1));
   value_list_add(q, value_new_int(1));
   value_list_add(q, value_new_int(1));
   v = value_eq(w,q); if (!v->iValue) printf("bad test equals list eq");
   value_delete(v);value_delete(w); value_delete(q);

   w = value_new_list(); q = value_new_list();
   value_list_add(w, value_new_int(1));
   value_list_add(w, value_new_int(1));
   value_list_add(q, value_new_int(2));
   value_list_add(q, value_new_int(1));
   v = value_eq(w,q);if (v->iValue) printf("bad test disjunct list eq"); value_delete(v);value_delete(w); value_delete(q);

   w = value_new_list(); q = value_new_list();
   value_list_add(w, value_new_int(1));
   value_list_add(w, value_new_int(1));
   value_list_add(q, value_new_int(1));
   value_list_add(q, value_new_int(1));
   value_list_add(q, value_new_int(1));
   v = value_eq(w,q);if (v->iValue) printf("bad test list - different sizes eq"); value_delete(v);value_delete(w); value_delete(q);



    char test[] = " /* ha */ 4 - 2*10 / 3*( 5 % 2  ) // hu ";
    Expression *e = getAST(test);
    Value *value = eval_value(e);
    if (value->iValue != -2) { printf("BAD Result of '%s' is ", test); value_print(value); }
    value_delete(value);
    expression_delete(e);

    char test2[] = " /* ha */ 4 - 2*10 / 3.1*( 5 % 2  ) // hu ";
    e = getAST(test2);
    value = eval_value(e);
    printf("EVAL: %s: ", test2);value_print(value);
    value_delete(value);
    expression_delete(e);
    return 0;
}