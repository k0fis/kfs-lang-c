#include "expression.h"
#include "kfs_lang_env.h"
#include "parser.h"
#include "lexer.h"
#include "value.h"

#include "utils.h"

#include <math.h>

void eval_s (KfsLangEnv *kfsLangEnv, char *code, char *result) {
    Value *value = eval_kfs_lang(kfsLangEnv, code);
    if (value == NULL) {
      KFS_ERROR("result is NULL for code : %s", code);
      return;
    }
    if (value->type != String) {
      KFS_ERROR("Bad result type (%i)", value->type);
    } else {
      if (strcmp(value->sValue, result)) {
        KFS_ERROR("BAD result %s : %s x %s ", code, value->sValue, result);
      }
    }
    value_delete(value);
}

void eval_i (KfsLangEnv *kfsLangEnv, char *code, int result) {
    Value *value = eval_kfs_lang(kfsLangEnv, code);
    if (value == NULL) {
      KFS_ERROR("result is NULL for code : %s", code);
      return;
    }
    if (value->type != Int) {
      KFS_ERROR("Bad result type(%i)", value->type);
    } else {
      if (value->iValue != result) {
        KFS_ERROR("BAD Result of '%s' defined: %i, but ", code, result); value_print(value);
      }
    }
    value_delete(value);
}

void eval_b (KfsLangEnv *kfsLangEnv, char *code, int result) {
    Value *value = eval_kfs_lang(kfsLangEnv, code);
    if (value == NULL) {
      KFS_ERROR( "result is NULL for code : %s", code);
      return;
    }
    if (value->type != Bool) {
      KFS_INFO(code);
      KFS_ERROR("Bad result type (%i)", value->type);
    } else {
      if ( ((!value->iValue) ^ (!result))) {
        KFS_ERROR("BAD Result of '%s' defined: %s, but ", code, result?"true":"false"); value_print(value);
      }
    }
    value_delete(value);
}

void eval_d(KfsLangEnv *kfsLangEnv, char *code, double result, double prec) {
    Value *value = eval_kfs_lang(kfsLangEnv, code);
    if (value == NULL) {
      KFS_ERROR( "result is NULL for code : %s", code);
      return;
    }
    if (value->type != Double) {
      KFS_ERROR("Bad result type(%i)", value->type);
    } else {
      if (fabs(value->dValue-result) > prec) {
        KFS_ERROR("BAD Result of '%s' defined: %lf, but:  ", code, result); value_print(value);
      }
    }
    value_delete(value);
}

void eval_l(KfsLangEnv *kfsLangEnv, char *code) {
    Value *value = eval_kfs_lang(kfsLangEnv, code);
    if (value == NULL) {
      KFS_ERROR("result is NULL for code : %s", code);
      return;
    }
    if (value->type != List) {
      KFS_ERROR("Bad result type(%i)", value->type);
    } else {
      KFS_INFO("Array: "); value_print(value);
    }
    value_delete(value);
}

void eval_o(KfsLangEnv *kfsLangEnv, char *code) {
  KFS_INFO(code);
  Value *value = eval_kfs_lang(kfsLangEnv, code);
  if (value == NULL) {
      KFS_ERROR( "result is NULL for code : %s", code);
    return;
  }
  if (value->type != Object) {
      KFS_ERROR( "%s - Bad result type : %i", code, value->type);
  } else {
    KFS_INFO("Object: "); value_print(value);
  }
  value_delete(value);
}

int main(void) {

   KFS_INFO("start test");
   KfsLangEnv *kfsLangEnv = kfs_lang_env_new();

   eval_l(kfsLangEnv, "[]");
   eval_l(kfsLangEnv, "[1]");
   eval_l(kfsLangEnv, "[1, 2]");
   eval_l(kfsLangEnv, "[1, 2 , 3 ]");

   eval_b(kfsLangEnv, " true", TRUE);
   eval_b(kfsLangEnv, " false", FALSE);

   eval_b(kfsLangEnv, " (5 != 3) && (3 > (2-5)) ", TRUE);
   eval_b(kfsLangEnv, "3 <= (2+1) ", TRUE);

   eval_o(kfsLangEnv, " { a : 1 ; b: 12.0/3.1; c : (12+4)}  ");
   eval_d(kfsLangEnv, " { a : 1 ; b: 12.0/3.1; c : (12+4)}.b  ", 3.87, 0.01);

   eval_d(kfsLangEnv, " [1.0, 2.0][1] ", 2, 0);
   eval_i(kfsLangEnv, "int( [1.0, {a : [2.0]}][1].a[0] -2) ", 0);

   eval_s(kfsLangEnv, "  \"pr\"+'d' ", "prd");

   setenv("napicu", "popici", 0);
   eval_s(kfsLangEnv, " \"{{napicu}} pako je defo a je to -{{napicu}}- a nebo taky -{{napicu2}}{{napicu}}- debil\" ", "popici pako je defo a je to -popici- a nebo taky -{{napicu2}}popici- debil");


   KFS_INFO("end test");
   kfs_lang_env_delete(kfsLangEnv);
}