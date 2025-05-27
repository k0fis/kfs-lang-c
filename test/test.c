#include "options.h"
#include "expression.h"
#include "kfs_lang_env.h"
#include "parser.h"
#include "lexer.h"
#include "value.h"

#include "utils.h"

#include <math.h>

#define CE(name, res) if (res) { KFS_ERROR("%s : %i", name, res); }

int eval(KfsLangEnv *kfsLangEnv, char *code, char *sExpected, int *iExpected, double *dExpected, double precision, Options* options) {
  int ret = 0;
  Value *value = kfs_lang_eval(kfsLangEnv, code, options);
  if (value == NULL) {
    KFS_ERROR("result is NULL for code : %s", code);
    return -1;
  }
  if (value->type == String) {
    if (sExpected != NULL) {
      if (strcmp(value->sValue, sExpected)) {
        KFS_ERROR("BAD result for code: %s : result: <%s>  but expected: <%s> ", code, value->sValue, sExpected);
        ret = -2;
      }
    } else {
      KFS_INFO2("result is: %s", value->sValue);
    }
  } else if (value->type == Int) {
    if (iExpected != NULL) {
      if (*iExpected != value->iValue) {
        char *valStr = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
        KFS_ERROR("BAD Result of '%s' expected: %i, but %s", code, *iExpected, valStr);
        free(valStr);
        ret = -3;
      }
    } else {
      KFS_INFO2("result is: %i", value->iValue);
    }
  } else if (value->type == Double) {
    if (dExpected != NULL) {
      if (fabs(value->dValue-(*dExpected)) > precision) {
        char *valStr = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
        KFS_ERROR("BAD Result of '%s' defined: %lf (+-%lf), but: %s ", code, *dExpected, precision, valStr);
        free(valStr);
        ret = -4;
      }
    } else {
      KFS_INFO2("result is: %lf", value->dValue);
    }
  } else if (value->type == List) {
    char *valStr = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
    KFS_INFO2("Array: %s", valStr);
    free(valStr);
  } else if (value->type == Object) {
    char *valStr = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
    KFS_INFO2("Object: %s", valStr);
    free(valStr);
  } else if (value->type == Bool) {
    if (iExpected != NULL) {
      if ( ((!value->iValue) ^ (!*iExpected)) ) {
        char *valStr = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
        KFS_ERROR("BAD Result of '%s' defined: %s, but %s", code, *iExpected?"true":"false", valStr);
        free(valStr);
        ret = -5;
      }
    } else {
      KFS_INFO2("result is: %i", value->iValue);
    }
  } else {
    char *info = value_to_string(value, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
    KFS_ERROR("Bad result type (%i): %s ->  %s", value->type, code, info);
    free(info);
    ret = -20;
  }
  value_delete(value);
  return ret;
}


int main(int argc, char *argv[]) {
   int iExpected; double dExpected;
   KFS_INFO("start test");

  Options *options; options_create(&options);
  int res = options_fulfill(options, argc, argv);
  if (res != RET_OK) {
    options_delete(options);
    return res;
  }

   KfsLangEnv *kfsLangEnv = kfs_lang_env_new();

   CE("Array", eval(kfsLangEnv, "[[[[[]]]], []]", NULL, NULL, NULL, 0, options));
   CE("Array empty", eval(kfsLangEnv, "[]", NULL, NULL, NULL, 0, options));
   iExpected =  1; CE("De-Array", eval(kfsLangEnv, "[1]", NULL, &iExpected, NULL, 0, options));
   CE("Array", eval(kfsLangEnv, "[1, 2]", NULL, NULL, NULL, 0, options));
   iExpected = !0; CE("true", eval(kfsLangEnv, " true", NULL, &iExpected, NULL, 0, options));
   iExpected =  0; CE("false", eval(kfsLangEnv, " false", NULL, &iExpected, NULL, 0, options));
   iExpected = !0; CE("true", eval(kfsLangEnv, " (5 != 3) && (3 > (2-5)) ", NULL, &iExpected, NULL, 0, options));
   iExpected = !0; CE("true", eval(kfsLangEnv, "3 <= (2+1) ", NULL, &iExpected, NULL, 0, options));

   eval(kfsLangEnv, " { a : 1 ; b: 12.0/3.1; c : (12+4)}  ", NULL, NULL, NULL, 0, options);
   dExpected = 3.87;  CE("dbl", eval(kfsLangEnv, " { a : 1 ; b: 12.0/3.1; c : (12+4)}.b  ", NULL, NULL, &dExpected, 0.01, options));

   dExpected = 2; CE("dbl", eval(kfsLangEnv, " [1.0, 2.0][1] ", NULL, NULL, &dExpected, 0, options));
   iExpected = 0; CE("int()", eval(kfsLangEnv, "int( [1.0, {a : [2.0]}][1].a[0] -2) ", NULL, &iExpected, NULL, 0, options));

   CE("string", eval(kfsLangEnv, "  \"pr\"+'d' ", "prd", NULL, NULL, 0, options));

   setenv("napicu", "popici", 0);
   CE("string with replace sys env", eval(kfsLangEnv, " \"{{napicu}} pako je defo a je to -{{napicu}}- a nebo taky -{{napicu2}}{{napicu}}- debil\" ", "popici pako je defo a je to -popici- a nebo taky -{{napicu2}}popici- debil", NULL, NULL, 0, options));
   CE("string w/o replace sys", eval(kfsLangEnv, " '{{napicu}} pako je defo a je to -{{napicu}}- a nebo taky -{{napicu2}}{{napicu}}- debil' ", "{{napicu}} pako je defo a je to -{{napicu}}- a nebo taky -{{napicu2}}{{napicu}}- debil", NULL, NULL, 0, options));

   iExpected = 1; CE("assign", eval(kfsLangEnv, "a = 1;", NULL, &iExpected, NULL, 0, options));
   iExpected = 3; CE("if", eval(kfsLangEnv, "if (false) b = 2; else b = 3;", NULL, &iExpected, NULL, 0, options));
   iExpected = 2; CE("if", eval(kfsLangEnv, "if (true) b = 2; else b = 3;", NULL, &iExpected, NULL, 0, options));
   iExpected = 3; CE("vars", eval(kfsLangEnv, "a = 1; n=2; return a+n;", NULL, &iExpected, NULL, 0, options));
   CE("in block assign", eval(kfsLangEnv, "{ a = 1; n=2; }",  NULL, NULL, NULL, 0, options));

   iExpected = 14; CE("set vars", eval(kfsLangEnv, "a=12; if (true) {a = 2;} else {a=3;} c = 1+a; return a+12;", NULL, &iExpected, NULL, 0, options));
   iExpected = 15; CE("cycle", eval(kfsLangEnv, "a = 5; b = 0; while (a > 0) {b = b+a; a = a-1;} return b;", NULL, &iExpected, NULL, 0, options));
   iExpected = 12; CE("cycle + break", eval(kfsLangEnv, "a = 5; b = 0; while (a > 0) {b = b+a; a = a-1; if (a<3) {break;} } return b;", NULL, &iExpected, NULL, 0, options));
   iExpected = 10; CE("function", eval(kfsLangEnv, "a = 1; pako() {return a+5; } b = pako(a:5); return b;", NULL, &iExpected, NULL, 0, options));
   iExpected =  6; CE("function multi return", eval(kfsLangEnv, "pako() {c = 2; return a+c,a,c; } b= pako(a:4); return b[0];", NULL, &iExpected, NULL, 0, options));
   iExpected = 31; CE("empty var", eval(kfsLangEnv, "pako() {if (empty(c)) c = 11; c=c+2; return a+c,a,c; } return pako(a:18)[0];", NULL, &iExpected, NULL, 0, options));

   KFS_INFO("end test");
   kfs_lang_env_delete(kfsLangEnv);
   options_delete(options);
}