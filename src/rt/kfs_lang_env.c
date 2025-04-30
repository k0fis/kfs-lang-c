#include "kfs_lang_env.h"
#include "parser.h"
#include "lexer.h"
#include "named_value.h"

//
KfsVariables *kfs_variables_new() {
  KFS_MALLOC(KfsVariables, kv);
  KFS_LST_INIT(kv->lst);
  kv->variables = named_value_create_hashmap();
  return kv;
}

void kfs_variables_delete(KfsVariables *kv) {
  hashmap_free(kv->variables);
  KfsVariables *inx, *tmp; list_for_each_entry_safe(inx, tmp, &kv->lst, lst) {
    list_del(&inx->lst);
    kfs_variables_delete(inx);
  }
  free(kv);
}

// add new vars -> new block -> new locals
// remove block
// get var -> all stack

//

KfsVarStack *kfs_var_stack_new(){
  KFS_MALLOC(KfsVarStack, kvs);
  kfsVariables = kfs_variables_new();
  KFS_LST_INIT(kvs->lst);
  return kvs;
}

void kfs_var_stack_delete(KfsVarStack *kvs) {
  kfs_variables_delete(kvs->kfsVariables);
  KfsVarStack *inx, *tmp; list_for_each_entry_safe(inx, tmp, &kv->lst, lst) {
    list_del(&inx->lst);
    kfs_var_stack_delete(inx);
  }
  free(kvs);
}

// add new var stack -> call fce
// remove stack -> out of fce
// find vars -> only in actual head

//

KfsLangEnv *kfs_lang_env_new() {
  KFS_MALLOC(KfsLangEnv, env);
  env->expression = NULL;
  env->variables = kfs_var_stack_new();
  return env;
}

void kfs_lang_env_delete(KfsLangEnv *kfsLangEnv) {
  if (kfsLangEnv != NULL) {
    if (kfsLangEnv->expression != NULL) {
      expression_delete(kfsLangEnv->expression);
    }
    kfs_var_stack_delete(kfsLangEnv->variables);
    free(kfsLangEnv);
  }
}


Value *eval_value(KfsLangEnv *kfsLangEnv, Expression *e) {
  if (e == NULL) {
    return NULL;
  }
  Value *lv, *rv, *result;
  NamedExpression *ne;
  Expression *expr;
  int iny;

  switch (e->type) {
    case eIntVALUE:
      return value_new_int(e->lValue);
    case eDoubleVALUE:
      return value_new_double(e->dValue);
    case eBoolVALUE:
      return value_new_bool(e->lValue);
    case eStringVALUE:
      return value_new_string(e->str);
    case eListVALUE:
      lv = value_new_list();
      if (lv == NULL) {
        KFS_ERROR("Cannot obtain new LIST value");
        return NULL;
      }
      Expression *inx; list_for_each_entry(inx, &e->lst, lst) {
        value_list_add(lv, eval_value(kfsLangEnv, inx));
      }
      return lv;
    case eObjectVALUE:
      lv = value_new_object();
      size_t iter = 0;
      void *item;
      while (hashmap_iter(e->object, &iter, &item)) {
        NamedExpression *ne = (NamedExpression*)item;
        value_object_add(lv, ne->name, eval_value(kfsLangEnv, ne->expression));
      }
      return lv;
    case eMULTIPLY:
      result = value_mul((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eADD:
      result = value_plus((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eMINUS:
      result = value_minus((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eDIVIDE:
      result = value_divide((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eMODULO:
      result = value_mod((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case ePOWER:
      result = value_power((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eLT:
      result = value_lt((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eLE:
      result = value_le((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eEQ:
      result = value_eq((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eNE:
      result = value_ne((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eGT:
      result = value_gt((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eGE:
      result = value_ge((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eAND:
      result = value_and((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eOR:
      result = value_or((lv = eval_value(kfsLangEnv, e->left)), (rv = eval_value(kfsLangEnv, e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eNOT:
      result = value_not((lv = eval_value(kfsLangEnv, e->left)));
      value_delete(lv);
      return result;
    case eUNARY_MINUS:
      result = eval_value(kfsLangEnv, e->left);
      if (result->type == Int) {
        result->iValue = -1*result->iValue;
      } else if (result->type == Double) {
        result->dValue = -1*result->dValue;
      }
      return result;
    case eDOT:
      lv = eval_value(kfsLangEnv, e->left);
      if (lv->type != Object) {
        KFS_ERROR("Object access to non-object value");
        value_delete(lv);
        return NULL;
      }
      result = named_value_get(lv->oValue, e->str);
      if (result == NULL) {
        KFS_ERROR("empty result");
      } else {
        result = value_copy(result);
        if (result == NULL) {
          KFS_ERROR("Cannot create value copy");
        }
      }
      value_delete(lv);
      return result;
    case eARRAY_ACCESS:
      rv = eval_value(kfsLangEnv, e->right);
      if (rv->type != Int) {
        KFS_ERROR("Array access - index type must be INT");
        value_delete(rv);
        return NULL;
      }
      int position = rv->iValue;
      value_delete(rv);
      lv = eval_value(kfsLangEnv, e->left);
      if (lv->type != List) {
        KFS_ERROR("Array access to non-array value");
        value_delete(lv);
        return NULL;
      }
      result = value_list_get(lv, position);
      if (result == NULL) {
        KFS_ERROR("empty value");
      } else {
        result = value_copy(result);
        if (result == NULL) {
          KFS_ERROR("Cannot create value copy");
        }
      }
      value_delete(lv);
      return result;
    case eINT:
      lv = eval_value(kfsLangEnv, e->left);
      switch (lv->type) {
        case String:
          KFS_ERROR("Cannot convert String into int");
          return NULL;
        case List:
          KFS_ERROR("Cannot convert Array into int");
          return NULL;
        case Object:
          KFS_ERROR("Cannot convert Object into int");
          return NULL;
        case Double:
          lv->iValue = (int)lv->dValue;
        case Bool:
          lv->type = Int;
        case Int: ;
      }
      return lv;
  }
  return NULL;
}

Value *eval_kfs_lang(KfsLangEnv *kfsLangEnv, char *code) {
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if (yylex_init(&scanner)) {
    KFS_ERROR("Cannot init yylex");
    return NULL;
  }
  state = yy_scan_string(code, scanner);
  if (yyparse(kfsLangEnv, scanner)) {
    KFS_ERROR("Cannot parse code");
    return NULL;
  }
  yy_delete_buffer(state, scanner);
  yylex_destroy(scanner);
  Value *result = eval_value(kfsLangEnv, kfsLangEnv->expression);
  return result;
}