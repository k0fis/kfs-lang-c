#include "kfs_lang_env.h"
#include "parser.h"
#include "lexer.h"

//
Variables *variables_new() {
  KFS_MALLOC(Variables, kv);
  KFS_LST_INIT(kv->lstVs);
  kv->variables = dict_new((element_free)value_delete);
  return kv;
}

void variables_delete(Variables *kv) {
  KFS_TRACE("variables_delete - 1", NULL);
  dict_delete(kv->variables);
  KFS_TRACE("variables_delete - 2", NULL);
  free(kv);
  KFS_TRACE("variables_delete - 3", NULL);
}

int variables_set(Variables *kv, char *name, Value *value, int mode) {
  int ret = dict_set(kv->variables, name, value,
      mode==VARIABLE_ADD_MODE_FORCE_ADD?KFS_DICT_SET_NORMAL:KFS_DICT_SET_DO_NOT_CREATE_NEW);
  if (ret == KFS_DICT_SET_RET_OK) {
    return VARIABLE_SET_RET_OK;
  } else if (ret == KFS_DICT_SET_RET_NOT_SET) {
    return VARIABLE_SET_RET_NOT_SET;
  }
  return VARIABLE_SET_RET_ERROR;
}

char *variables_to_string(Variables *kv){
  KFS_MALLOC_CHAR(ret,3);
  strcat(ret, "{ ");
  DictItem *inx; list_for_each_entry(inx, &kv->variables->lst, lst) {
    ret = realloc(ret, strlen(ret)+strlen(inx->name)+3);
    strcat(ret, inx->name); strcat(ret, ": ");
    char *vStr = value_to_string((Value *)inx->data, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
    ret = realloc(ret, strlen(ret)+strlen(vStr)+3);
    strcat(ret, vStr); strcat(ret, "; ");
    free(vStr);
  }
  if (strlen(ret) > 2) {
    ret[strlen(ret)-2] = ' ';
  }
  ret[strlen(ret)-1] = '}';
  return ret;
}

//

Variables *var_stack_add_variables(VarStack *vs) {
  Variables *vars = variables_new();
  list_add(&vars->lstVs, &vs->variablesStack);
  return vars;
}

int var_stack_remove_variables(VarStack *vs) {
  KFS_TRACE("var_stack_remove_variables - 1", NULL);
  if (vs->variablesStack.next != &vs->variablesStack) {
    KFS_TRACE("var_stack_remove_variables - 2", NULL);
    Variables *inx = list_first_entry(&vs->variablesStack, Variables, lstVs);
    KFS_TRACE("var_stack_remove_variables - 3", NULL);
    list_del(&inx->lstVs);
    KFS_TRACE("var_stack_remove_variables - 4", NULL);
    variables_delete(inx);
  }
  KFS_TRACE("var_stack_remove_variables - 10", NULL);
  return 0;
}

VarStack *var_stack_new(){
  KFS_MALLOC(VarStack, kvs);
  KFS_LST_INIT(kvs->variablesStack);
  KFS_LST_INIT(kvs->lstEnv);
  var_stack_add_variables(kvs);
  return kvs;
}

void var_stack_delete(VarStack *kvs) {
  int traceInx = 0;
  Variables *inx, *tmp; list_for_each_entry_safe(inx, tmp, &kvs->variablesStack, lstVs) {
    list_del(&inx->lstVs);
    variables_delete(inx);
  }
  free(kvs);
}

int var_stack_set(VarStack *vs, char *name, Value *value) {
  Variables *inx; list_for_each_entry(inx, &vs->variablesStack, lstVs) {
    if (variables_set(inx, name, value, VARIABLE_ADD_MODE_ADD_ONLY_OVERRIDE) == VARIABLE_SET_RET_OK) {
      return VARIABLE_SET_RET_OK;
    }
  }
  inx = list_first_entry(&vs->variablesStack, Variables, lstVs);
  if (inx == NULL) {
    KFS_ERROR("Bas Var stack state!", NULL);
    return VARIABLE_SET_RET_NOT_SET;
  }
  if (variables_set(inx, name, value, VARIABLE_ADD_MODE_FORCE_ADD) == VARIABLE_SET_RET_OK) {
    return VARIABLE_SET_RET_OK;
  }
  KFS_ERROR("Cannot set variable %s!", name);
  return VARIABLE_SET_RET_NOT_SET;
}

char *var_stack_to_string(VarStack *vs) {
  KFS_MALLOC_CHAR(ret, 2);
  strcat(ret, "[ ");
  Variables *inx; list_for_each_entry(inx, &vs->variablesStack, lstVs) {
    char *vStr = variables_to_string(inx);
    ret = realloc(ret, strlen(ret)+strlen(vStr) + 3);
    strcat(ret, vStr); strcat(ret, ", ");
    free(vStr);
  }
  if (strlen(ret) > 2) {
    ret[strlen(ret)-2] = ' ';
  }
  ret[strlen(ret)-1] = ']';
  return ret;
}

// add new var stack -> call fce
// remove stack -> out of fce
// find vars -> only in actual head

//

KfsLangEnv *kfs_lang_env_new() {
  KFS_MALLOC(KfsLangEnv, env);
  KFS_LST_INIT(env->variablesStack);
  env->expression = NULL;
  env->useStringSysReplace = 1;
  char *regexStr = "{{[^ }]+}}";
  if (regcomp(&(env->stringSysReplace), regexStr, REG_EXTENDED)) {
    KFS_ERROR("Could not compile regex: %s", regexStr);
    env->useStringSysReplace = 0;
  }
  kfs_lang_env_add_space(env);
  return env;
}

void kfs_lang_env_delete(KfsLangEnv *kfsLangEnv) {
  if (kfsLangEnv != NULL) {
    if (kfsLangEnv->expression != NULL) {
      expression_delete(kfsLangEnv->expression);
    }
    regfree(&(kfsLangEnv->stringSysReplace));
    VarStack *inx, *tmp; list_for_each_entry_safe(inx, tmp, &kfsLangEnv->variablesStack, lstEnv) {
      list_del(&inx->lstEnv);
      var_stack_delete(inx);
    }
    free(kfsLangEnv);
  }
}

VarStack *kfs_lang_env_add_space(KfsLangEnv *kfsLangEnv) {
  VarStack *vs = var_stack_new();
  if (vs == NULL) {
    KFS_ERROR("Cannot initialize new VarStack", NULL);
    return NULL;
  }
  list_add(&vs->lstEnv, &kfsLangEnv->variablesStack);
  return vs;
}

void kfs_lang_env_remove_space(KfsLangEnv *kfsLangEnv) {
  if (kfsLangEnv->variablesStack.next != &kfsLangEnv->variablesStack) {
    VarStack *lastVs = list_first_entry(&kfsLangEnv->variablesStack, VarStack, lstEnv);
    list_del(&lastVs->lstEnv);
    var_stack_delete(lastVs);
  } else {
    KFS_ERROR("Cannot remove last VSStack, its empty!", NULL);
  }
}

void kfs_lang_env_space_add_vars(KfsLangEnv *kfsLangEnv) {

}

void kfs_lang_env_space_del_vars(KfsLangEnv *kfsLangEnv) {

}


/////////

char *replace_system_props(regex_t regex, char *input) {
  KFS_TRACE("start replace_system_props('%s')", input);
  int result;
  regmatch_t match;
  result = regexec(&regex, input, 1, &match, 0);
  if (!result) {
    char *cursor = input;
    KFS_MALLOC_CHAR(output, 1);
    while (match.rm_so >= 0) {
      int len1 = (int)(match.rm_so)+1;
      if (len1 > 1) {
        int outlen = strlen(output);
        output = realloc(output, outlen + len1);
        snprintf(output+outlen, len1, "%.*s", len1-1, cursor);
      }
      int outlen = strlen(output);
      int nameLen = (int)(match.rm_eo - match.rm_so)-4 + 1;
      KFS_MALLOC_CHAR(name, nameLen);
      snprintf(name, nameLen, "%.*s", nameLen-1, cursor+match.rm_so+2);
      char *replace = getenv(name);
      KFS_TRACE("name: '%s', replace: '%s'", name, replace);
      free(name);
      if (replace != NULL) {
        output = realloc(output, outlen + strlen(replace)+1);
        strcat(output, replace);
      } else {
        output = realloc(output, outlen + nameLen + 5);
        sprintf(output+outlen,"{{%.*s}}", nameLen-1, cursor+match.rm_so+2);
      }

      cursor += match.rm_eo;
      result = regexec(&regex, cursor, 1, &match, 0);
      if (result) {
        int len = strlen(cursor);
        if (len > 0) {
          int outlen = strlen(output);
          output = realloc(output, outlen + len + 1);
          strcat(output,  cursor);
        }
        break;
      }
    }
    KFS_TRACE("start replace_system_props('%s') -> '%s'", input, output);
    return output;
  } else if (result == REG_NOMATCH) {
    KFS_DEBUG("No match: %s", input);
  } else {
    char msgbuf[1024];
    regerror(result, &regex, msgbuf, sizeof(msgbuf));
    KFS_ERROR("Regex match failed: %s", msgbuf);
  }
  KFS_TRACE("start replace_system_props('%s') w/o changes", input);
  return input;
}

Value *evalString(KfsLangEnv *env, char *value) {
  int evalSys = value[0]=='"';
  value+=1;value[strlen(value)-1] = '\0';
  KFS_TRACE("evalString: %i && %i = %i", env->useStringSysReplace, evalSys, env->useStringSysReplace && evalSys);
  if (env->useStringSysReplace && evalSys)
    return value_new_string(replace_system_props(env->stringSysReplace, value));
  return value_new_string(value);
}

Value *eval_value(KfsLangEnv *kfsLangEnv, Expression *e) {
  if (e == NULL) {
    return NULL;
  }
  Value *lv, *rv, *result;
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
      return evalString(kfsLangEnv, e->str);
    case eListVALUE:
      lv = value_new_list();
      if (lv == NULL) {
        KFS_ERROR("Cannot obtain new LIST value(%i)", 0);
        return NULL;
      }
      Expression *inx; list_for_each_entry(inx, &e->lst, lst) {
        value_list_add(lv, eval_value(kfsLangEnv, inx));
      }
      return lv;
    case eObjectVALUE:
      lv = value_new_object();
      DictItem *iny; list_for_each_entry(iny, &e->object->lst, lst) {
        value_object_add(lv, iny->name, eval_value(kfsLangEnv, (Expression *)iny->data));
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
        KFS_ERROR("Object access to non-object value (%i)", lv->type);
        value_delete(lv);
        return NULL;
      }
      result = (Value *)dict_get(lv->oValue, e->str);
      if (result == NULL) {
        KFS_ERROR("empty result", NULL);
      } else {
        result = value_copy(result);
        if (result == NULL) {
          KFS_ERROR("Cannot create value copy", NULL);
        }
      }
      value_delete(lv);
      return result;
    case eARRAY_ACCESS:
      rv = eval_value(kfsLangEnv, e->right);
      if (rv->type != Int) {
        KFS_ERROR("Array access - index type must be INT (%i)", rv->type);
        value_delete(rv);
        return NULL;
      }
      int position = rv->iValue;
      value_delete(rv);
      lv = eval_value(kfsLangEnv, e->left);
      if (lv->type != List) {
        KFS_ERROR("Array access to non-array value (%i)", lv->type);
        value_delete(lv);
        return NULL;
      }
      result = value_list_get(lv, position);
      if (result == NULL) {
        KFS_ERROR("empty value", NULL);
      } else {
        result = value_copy(result);
        if (result == NULL) {
          KFS_ERROR("Cannot create value copy", NULL);
        }
      }
      value_delete(lv);
      return result;
    case eINT:
      lv = eval_value(kfsLangEnv, e->left);
      switch (lv->type) {
        case String:
          KFS_ERROR("Cannot convert String into int", NULL);
          return NULL;
        case List:
          KFS_ERROR("Cannot convert Array into int", NULL);
          return NULL;
        case Object:
          KFS_ERROR("Cannot convert Object into int", NULL);
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
    KFS_ERROR("Cannot init yylex", NULL);
    return NULL;
  }
  state = yy_scan_string(code, scanner);
  if (yyparse(kfsLangEnv, scanner)) {
    KFS_ERROR("Cannot parse code", NULL);
    return NULL;
  }
  yy_delete_buffer(state, scanner);
  yylex_destroy(scanner);
  Value *result = eval_value(kfsLangEnv, kfsLangEnv->expression);
  return result;
}