#include "expression.h"
#include "parser.h"
#include "lexer.h"
#include "value.h"
#include "eval.h"

#include "utils.h"

#include <math.h>

char outputBuffer[256];

void eval_i (char *code, int result) {
    Expression *e = eval_kfs_lang(code);
    Value *value = eval_value(e);
    if (value == NULL) {
      snprintf(outputBuffer, 255, "result is NULL for code : %s", code);
      KFS_ERROR(outputBuffer);
      return;
    }
    if (value->type != Int) {
      KFS_ERROR("Bad result type");
    } else {
      if (value->iValue != result) {
        printf("BAD Result of '%s' defined: %i, but ", code, result); value_print(value);
      }
    }
    value_delete(value);
    expression_delete(e);
}

void eval_b (char *code, int result) {
    Expression *e = eval_kfs_lang(code);
    Value *value = eval_value(e);
    if (value == NULL) {
      snprintf(outputBuffer, 255, "result is NULL for code : %s", code);
      KFS_ERROR(outputBuffer);
      return;
    }
    if (value->type != Bool) {
      KFS_INFO(code);
      KFS_ERROR("Bad result type");
    } else {
      if ( ((!value->iValue) ^ (!result))) {
        printf("BAD Result of '%s' defined: %s, but ", code, result?"true":"false"); value_print(value);
      }
    }
    value_delete(value);
    expression_delete(e);
}

void eval_d(char *code, double result, double prec) {
    Expression *e = eval_kfs_lang(code);
    Value *value = eval_value(e);
    if (value == NULL) {
      snprintf(outputBuffer, 255, "result is NULL for code : %s", code);
      KFS_ERROR(outputBuffer);
      return;
    }
    if (value->type != Double) {
      KFS_ERROR("Bad result type");
    } else {
      if (fabs(value->dValue-result) > prec) {
        printf("BAD Result of '%s' defined: %lf, but:  ", code, result); value_print(value);
      }
    }
    value_delete(value);
    expression_delete(e);
}

void eval_l(char *code) {
    Expression *e = eval_kfs_lang(code);
    Value *value = eval_value(e);
    if (value == NULL) {
      snprintf(outputBuffer, 255, "result is NULL for code : %s", code);
      KFS_ERROR(outputBuffer);
      return;
    }
    if (value->type != List) {
      KFS_ERROR("Bad result type");
    } else {
      KFS_INFO("Array: "); value_print(value);
    }
    value_delete(value);
    expression_delete(e);
}

void eval_o(char *code) {
  KFS_INFO(code);
  Expression *e = eval_kfs_lang(code);
  Value *value = eval_value(e);
  if (value == NULL) {
      snprintf(outputBuffer, 255, "result is NULL for code : %s", code);
      KFS_ERROR(outputBuffer);
    return;
  }
  if (value->type != Object) {
      snprintf(outputBuffer, 255, "%s - Bad result type : %i", code, value->type);
      KFS_ERROR(outputBuffer);
  } else {
    KFS_INFO("Object: "); value_print(value);
  }
  value_delete(value);
  expression_delete(e);
}

int main(void) {
   KFS_INFO("start");
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
   v = value_eq(w,q);if (!v->iValue) printf("bad test eq");
   value_delete(v);value_delete(w); value_delete(q);

   w = value_new_string("pako"); q = value_new_string("pako");
   v = value_eq(w,q); if (!v->iValue) printf("bad test string eq");
   value_delete(v);value_delete(w); value_delete(q);

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
   v = value_eq(w,q);if (v->iValue) printf("bad test disjunct list eq");
   value_delete(v);value_delete(w); value_delete(q);

   w = value_new_list(); q = value_new_list();
   value_list_add(w, value_new_int(1));
   value_list_add(w, value_new_int(1));
   value_list_add(q, value_new_int(1));
   value_list_add(q, value_new_int(1));
   value_list_add(q, value_new_int(1));
   v = value_eq(w,q);if (v->iValue) printf("bad test list - different sizes eq");
   value_delete(v);value_delete(w); value_delete(q);

   eval_i(" /* ha */ 4 - 2*10 / 3*( 5 % 2  ) // hu ", -2);
   eval_i(" 4-6 ", -2);
   eval_i(" -3 ", -3);
   eval_d(" /* ha */ 4 - 2*10 / 3.1*( 5 % 2  ) // hu ", -2.451613, 0.000001);
   eval_l("[]");
   eval_l("[1]");
   eval_l("[1, 2]");
   eval_l("[1, 2 , 3 ]");

   eval_b(" true", TRUE);
   eval_b(" false", FALSE);

   eval_b(" (5 != 3) && (3 > (2-5)) ", TRUE);
   eval_b("3 <= (2+1) ", TRUE);

   eval_o(" { a : 1 ; b: 12.0/3.1; c : (12+4)}  ");
   eval_d(" { a : 1 ; b: 12.0/3.1; c : (12+4)}.b  ", 3.87, 0.01);
   //eval_d(" 1.b  ", 3.87, 0.01);

   eval_d(" [1.0, 2.0][1] ", 2, 0);
   eval_d(" [1.0, {a : [2.0]}][1].a[0] -2 ", 0, 0);
}