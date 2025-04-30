#ifndef _kfs_lang_env_h
#define _kfs_lang_env_h

#include "hashmap/hashmap.h"
#include "expression.h"
#include "named_value.h"

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

typedef struct tagKfsVariables {
  struct hashmap *variables;
  ll_t lst;
} KfsVariables;

typedef struct tagKfsVarStack {
  KfsVariables *kfsVariables;
  ll_t lst;
} KfsVarStack;

typedef struct tagKfsLangEnv {
   Expression *expression;
   KfsVarStack *variables;
} KfsLangEnv;

KfsLangEnv *kfs_lang_env_new();
void kfs_lang_env_delete(KfsLangEnv *kfsLangEnv);

Value *eval_kfs_lang(KfsLangEnv *kfsLangEnv, char *code);

#if defined(__cplusplus)
}
#endif  // __cplusplus


#endif