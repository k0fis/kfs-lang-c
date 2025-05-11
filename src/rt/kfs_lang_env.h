#ifndef _kfs_lang_env_h
#define _kfs_lang_env_h

#include <regex.h>

#include "kfs_dict.h"
#include "expression.h"
#include "value.h"

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

typedef struct tagVariables {
  Dictionary *variables;
  ll_t lstVs; // list toggler for Variables in stack/list
} Variables;

#define VARIABLE_SET_RET_OK       0
#define VARIABLE_SET_RET_ERROR   -1
#define VARIABLE_SET_RET_NOT_SET  1

#define VARIABLE_ADD_MODE_FORCE_ADD 1
#define VARIABLE_ADD_MODE_ADD_ONLY_OVERRIDE 2

char *variables_to_string(Variables *kv);

typedef struct tagVarStack {
  ll_t variablesStack;  // head of Variables
  ll_t lstEnv; // stack toggle for list in Kfs Env Lang
} VarStack;

char *var_stack_to_string(VarStack *vs);

typedef struct tagKfsLangEnv {
   Expression *expression;
   ll_t varStackStack;
   regex_t stringSysReplace;
   int useStringSysReplace;

    ll_t functions;
    Value *empty;
} KfsLangEnv;

KfsLangEnv *kfs_lang_env_new();
void kfs_lang_env_delete(KfsLangEnv *kfsLangEnv);

VarStack *kfs_lang_env_add_space(KfsLangEnv *kfsLangEnv, Value *variables);
void kfs_lang_env_remove_space(KfsLangEnv *kfsLangEnv);

void kfs_lang_env_space_add_vars(KfsLangEnv *kfsLangEnv, Value *value);
void kfs_lang_env_space_del_vars(KfsLangEnv *kfsLangEnv);

Value *kfs_lang_get_var(KfsLangEnv *kfsLangEnv, char *name);
void kfs_lang_set_var(KfsLangEnv *kfsLangEnv, char *name, Value *value);

#define KLVTS_ONLY_ACTUAL_SPACE 0x1
#define KLVTS_ALL_SPACES        0x2

char *kfs_lang_vars_to_string(KfsLangEnv *kfsLangEnv, int mode);

#define KLE_EVAL_NORMAL   0x1
#define KLE_EVAL_FCE_CALL 0x2

Value *kfs_lang_eval(KfsLangEnv *kfsLangEnv, char *code);

#if defined(__cplusplus)
}
#endif  // __cplusplus


#endif