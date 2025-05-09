#include "expression.h"
#include "kfs_lang_env.h"
#include "parser.h"
#include "lexer.h"
#include "value.h"

#include "utils.h"

#include <math.h>

#define CE(name, res) if (res) { KFS_DEBUG("%s : %i", name, res); return res; }

int eval_s (KfsLangEnv *kfsLangEnv, char *code, char *expected) {
    int ret = 0;
    Value *value = kfs_lang_eval(kfsLangEnv, code);
    if (value == NULL) {
      KFS_ERROR("result is NULL for code : %s", code);
      return -1;
    }
    if (value->type != String) {
      char *info = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
      KFS_ERROR("Bad result type (%i): %s ->  %s", value->type, code, info);
      free(info);
      ret = -2;
    } else {
      if (strcmp(value->sValue, expected)) {
        KFS_ERROR("BAD result for code: %s : result: <%s>  expected: <%s> ", code, value->sValue, expected);
        ret = -3;
      }
    }
    value_delete(value);
    return ret;
}

void eval_i (KfsLangEnv *kfsLangEnv, char *code, int result) {
    Value *value = kfs_lang_eval(kfsLangEnv, code);
    if (value == NULL) {
      KFS_ERROR("result is NULL for code : %s", code);
      return;
    }
    if (value->type != Int) {
      char *info = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
      KFS_ERROR("Bad result type (%i): %s ->  %s", value->type, code, info);
      free(info);
    } else {
      if (value->iValue != result) {
        char *valStr = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
        KFS_ERROR("BAD Result of '%s' defined: %i, but %s", code, result, valStr);
        free(valStr);
      }
    }
    value_delete(value);
}

void eval_b (KfsLangEnv *kfsLangEnv, char *code, int result) {
    Value *value = kfs_lang_eval(kfsLangEnv, code);
    if (value == NULL) {
      KFS_ERROR( "result is NULL for code : %s", code);
      return;
    }
    if (value->type != Bool) {
      KFS_INFO(code);
      char *info = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
      KFS_ERROR("Bad result type (%i): %s", value->type, info);
      free(info);
    } else {
      if ( ((!value->iValue) ^ (!result))) {
        char *valStr = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
        KFS_ERROR("BAD Result of '%s' defined: %s, but %s", code, result?"true":"false", valStr);
        free(valStr);
      }
    }
    value_delete(value);
}

void eval_d(KfsLangEnv *kfsLangEnv, char *code, double result, double prec) {
    Value *value = kfs_lang_eval(kfsLangEnv, code);
    if (value == NULL) {
      KFS_ERROR( "result is NULL for code : %s", code);
      return;
    }
    if (value->type != Double) {
      char *info = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
      KFS_ERROR("Bad result type (%i): %s ->  %s", value->type, code, info);
      free(info);
    } else {
      if (fabs(value->dValue-result) > prec) {
        char *valStr = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
        KFS_ERROR("BAD Result of '%s' defined: %lf, but: %s ", code, result, valStr);
        free(valStr);
      }
    }
    value_delete(value);
}

void eval_l(KfsLangEnv *kfsLangEnv, char *code) {
    Value *value = kfs_lang_eval(kfsLangEnv, code);
    if (value == NULL) {
      KFS_ERROR("result is NULL for code : %s", code);
      return;
    }
    if (value->type != List) {
      char *info = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
      KFS_ERROR("Bad result type (%i): %s ->  %s", value->type, code, info);
      free(info);
    } else {
      char *valStr = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
      KFS_INFO2("Array: %s", valStr);
      free(valStr);
    }
    value_delete(value);
}

void eval_o(KfsLangEnv *kfsLangEnv, char *code) {
  Value *value = kfs_lang_eval(kfsLangEnv, code);
  if (value == NULL) {
      KFS_ERROR( "result is NULL for code : %s", code);
    return;
  }
  if (value->type != Object) {
      char *info = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
      KFS_ERROR("Bad result type (%i): %s ->  %s", value->type, code, info);
      free(info);
    } else {
    char *valStr = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
    KFS_INFO2("Object: %s", valStr);
    free(valStr);
  }
  value_delete(value);
}

int main(void) {

   KFS_INFO("start test");
   KfsLangEnv *kfsLangEnv = kfs_lang_env_new();

   eval_l(kfsLangEnv, "[[[[[]]]], []]");
   eval_l(kfsLangEnv, "[]");
   eval_i(kfsLangEnv, "[1]", 1);
   eval_l(kfsLangEnv, "[[],[]]");
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

   CE("string", eval_s(kfsLangEnv, "  \"pr\"+'d' ", "prd"));

   setenv("napicu", "popici", 0);
//   CE("string with replace sys env", eval_s(kfsLangEnv, " \"{{napicu}} pako je defo a je to -{{napicu}}- a nebo taky -{{napicu2}}{{napicu}}- debil\" ", "popici pako je defo a je to -popici- a nebo taky -{{napicu2}}popici- debil"));
//   CE("string w/o replace sys", eval_s(kfsLangEnv, " '{{napicu}} pako je defo a je to -{{napicu}}- a nebo taky -{{napicu2}}{{napicu}}- debil' ", "{{napicu}} pako je defo a je to -{{napicu}}- a nebo taky -{{napicu2}}{{napicu}}- debil"));

    eval_i(kfsLangEnv, "a = 1;", 1);
    eval_i(kfsLangEnv, "if (false) b = 2; else b = 3;", 3);
    eval_i(kfsLangEnv, "if (true) b = 2; else b = 3;", 2);
    eval_l(kfsLangEnv, "a = 1; n=2;");
    eval_l(kfsLangEnv, "{ a = 1; n=2; }");

    eval_l(kfsLangEnv, "if (true) {a = 2;} else {a=3;} c = 1+a; d=a+c;");

   KFS_INFO("end test");
   kfs_lang_env_delete(kfsLangEnv);
}