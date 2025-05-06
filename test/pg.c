
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include "rt/utils.h"
#include "ll/ll.h"
#include "rt/kfs_lang_env.h"


Variables *variables_new();
void variables_delete(Variables *kv);
int variables_set(Variables *kv, char *name, Value *value, int mode);
VarStack *var_stack_new();

Variables *var_stack_add_variables(VarStack *vs);
int var_stack_remove_variables(VarStack *vs);
int var_stack_set(VarStack *vs, char *name, Value *value);
int var_stack_set(VarStack *vs, char *name, Value *value);
void var_stack_delete(VarStack *kvs);

void test_VarStack() {
  char *info;
  VarStack *vStack = var_stack_new();

  info = var_stack_to_string(vStack); KFS_INFO2("s1: %s", info); free(info);

  var_stack_add_variables(vStack);
  var_stack_set(vStack, strdup("dva-1"), value_new_int(2));
  var_stack_set(vStack, strdup("dva-2"), value_new_int(2));
  info = var_stack_to_string(vStack); KFS_INFO2("s2: %s", info); free(info);

  var_stack_add_variables(vStack);
  var_stack_set(vStack, strdup("dva-3"), value_new_int(3));
  var_stack_set(vStack, strdup("dva-2"), value_new_int(3));

  info = var_stack_to_string(vStack); KFS_INFO2("s3: %s", info); free(info);

  var_stack_remove_variables(vStack);
  info = var_stack_to_string(vStack); KFS_INFO2("s3: %s", info); free(info);

  var_stack_remove_variables(vStack);
  info = var_stack_to_string(vStack); KFS_INFO2("s3: %s", info); free(info);


  var_stack_delete(vStack);
}

void test_Vars() {
  Variables *v = variables_new();

  char *info = variables_to_string(v);
  KFS_INFO2("'%s'", info);
  free(info);

  int ret = variables_set(v, strdup("je to pica"), value_new_int(1), VARIABLE_ADD_MODE_FORCE_ADD);
  if (ret != VARIABLE_SET_RET_OK) {
    KFS_ERROR("Cannot set var(%i!", ret);
  }

  info = variables_to_string(v);
  KFS_INFO2("'%s'", info);
  free(info);

  ret = variables_set(v, strdup("dvje"), value_new_string("jo, dve pice"), VARIABLE_ADD_MODE_FORCE_ADD);
  if (ret != VARIABLE_SET_RET_OK) {
    KFS_ERROR("Cannot set var(%i!", ret);
  }

  info = variables_to_string(v);
  KFS_INFO2("'%s'", info);
  free(info);
  variables_delete(v);
}
void test_env() {
  KFS_INFO("test env start");
  KfsLangEnv *env = kfs_lang_env_new();

  kfs_lang_env_add_space(env);
  kfs_lang_env_remove_space(env);

  kfs_lang_env_delete(env);

  KFS_INFO("test env end");
}

int main() {
  test_VarStack();
}