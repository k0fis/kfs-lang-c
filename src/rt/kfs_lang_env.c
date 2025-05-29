#include "kfs_lang_env.h"
#include "options.h"
#include "parser.h"
#include "lexer.h"

Variables *variables_new(Value *values) {
  KFS_MALLOC(Variables, kv);
  KFS_LST_INIT(kv->lstVs);
  kv->variables = dict_new((element_free)value_delete);
  if (values != NULL) {
    // setup vars
    if (values->type != Object) {
      KFS_ERROR("Expected type for setup variables is Object, but its %i", values->type);
    } else {
      DictItem *inx; list_for_each_entry(inx, &values->oValue->lst, lst) {
        dict_set(kv->variables, strdup(inx->name), value_copy((Value*)inx->data),KFS_DICT_SET_NORMAL );
      }
    }
  }
  return kv;
}

void variables_delete(Variables *kv) {
  dict_delete(kv->variables);
  free(kv);
}

Value *variables_get(Variables *kv, char *name) {
  return (Value *)dict_get(kv->variables, name);
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

void var_stack_add_variables(VarStack *vs, Value *value) {
  list_add(&variables_new(value)->lstVs, &vs->variablesStack);
}

int var_stack_remove_variables(VarStack *vs) {
  if (vs->variablesStack.next != &vs->variablesStack) {
    Variables *inx = list_first_entry(&vs->variablesStack, Variables, lstVs);
    list_del(&inx->lstVs);
    variables_delete(inx);
  }
  return 0;
}

VarStack *var_stack_new(Value *variables){
  KFS_MALLOC(VarStack, kvs);
  KFS_LST_INIT(kvs->variablesStack);
  KFS_LST_INIT(kvs->lstEnv);
  var_stack_add_variables(kvs, variables);
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

Value *var_stack_get(VarStack *vs, char *name) {
  Variables *inx; list_for_each_entry(inx, &vs->variablesStack, lstVs) {
    Value *ret = variables_get(inx, name);
    if (ret != NULL) {
      return ret;
    }
  }
  return NULL;
}

int var_stack_set(VarStack *vs, char *name, Value *value) {
  Variables *inx; list_for_each_entry(inx, &vs->variablesStack, lstVs) {
    if (variables_set(inx, name, value, VARIABLE_ADD_MODE_ADD_ONLY_OVERRIDE) == VARIABLE_SET_RET_OK) {
      return VARIABLE_SET_RET_OK;
    }
  }
  inx = list_first_entry(&vs->variablesStack, Variables, lstVs);
  if (inx == NULL) {
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
  KFS_LST_INIT(env->varStackStack);
  KFS_LST_INIT(env->functions);
  env->expression = NULL;
  env->empty = value_new(Empty);
  env->useStringSysReplace = 1;
  char *regexStr = "{{[^ }]+}}";
  if (regcomp(&(env->stringSysReplace), regexStr, REG_EXTENDED)) {
    KFS_ERROR("Could not compile regex: %s", regexStr);
    env->useStringSysReplace = 0;
  }
  kfs_lang_env_add_space(env, NULL);
  return env;
}

void varStackStack_cleanup(KfsLangEnv *kfsLangEnv) {
  VarStack *inx, *tmp; list_for_each_entry_safe(inx, tmp, &kfsLangEnv->varStackStack, lstEnv) {
    list_del(&inx->lstEnv);
    var_stack_delete(inx);
  }
}

void kfs_lang_env_cleanup_functions(KfsLangEnv *kfsLangEnv) {
  // exprtession will be deleted in expression list
  Expression *inx, *tmp; list_for_each_entry_safe(inx, tmp, &kfsLangEnv->functions, upHandle) {
    list_del(&inx->upHandle);
  }
}

void kfs_lang_env_delete(KfsLangEnv *kfsLangEnv) {
  if (kfsLangEnv != NULL) {
    kfs_lang_env_cleanup_functions(kfsLangEnv);
    if (kfsLangEnv->expression != NULL) {
      expression_delete(kfsLangEnv->expression);
    }
    regfree(&(kfsLangEnv->stringSysReplace));
    varStackStack_cleanup(kfsLangEnv);
    value_delete(kfsLangEnv->empty);
    free(kfsLangEnv);
  }
}

VarStack *kfs_lang_env_add_space(KfsLangEnv *kfsLangEnv, Value *variables) {
  VarStack *vs = var_stack_new(variables);
  if (vs == NULL) {
    KFS_ERROR("Cannot initialize new VarStack", NULL);
    return NULL;
  }
  list_add(&vs->lstEnv, &kfsLangEnv->varStackStack);
  return vs;
}

void kfs_lang_env_remove_space(KfsLangEnv *kfsLangEnv) {
  if (kfsLangEnv->varStackStack.next != &kfsLangEnv->varStackStack) {
    VarStack *lastVs = list_first_entry(&kfsLangEnv->varStackStack, VarStack, lstEnv);
    list_del(&lastVs->lstEnv);
    var_stack_delete(lastVs);
  } else {
    KFS_ERROR("Cannot remove last VSStack, its empty!", NULL);
  }
}

VarStack *kfs_lang_env_actual_space(KfsLangEnv *kfsLangEnv) {
  return list_first_entry(&kfsLangEnv->varStackStack, VarStack, lstEnv);
}

void kfs_lang_env_space_add_vars(KfsLangEnv *kfsLangEnv, Value *value) {
  VarStack *lastVs = kfs_lang_env_actual_space(kfsLangEnv);
  if (lastVs == NULL) {
    KFS_ERROR("Cannot find last variable spaces", NULL);
  } else {
    var_stack_add_variables(lastVs, value);
  }
}

void kfs_lang_env_space_del_vars(KfsLangEnv *kfsLangEnv) {
  VarStack *lastVs = kfs_lang_env_actual_space(kfsLangEnv);
  if (lastVs == NULL) {
    KFS_ERROR("Cannot find last variable spaces", NULL);
  } else {
    var_stack_remove_variables(lastVs);
  }
}

Value *kfs_lang_get_var(KfsLangEnv *kfsLangEnv, char *name) {
  Value *ret = var_stack_get(kfs_lang_env_actual_space(kfsLangEnv), name);
  if (ret == NULL) {
    return kfsLangEnv->empty;
  }
  return ret;
}

void kfs_lang_set_var(KfsLangEnv *kfsLangEnv, char *name, Value *value) {
  var_stack_set(kfs_lang_env_actual_space(kfsLangEnv), name, value);
}


char *kfs_lang_vars_to_string(KfsLangEnv *kfsLangEnv, int mode) {
  char *tmp;
  VarStack *inx;
  KFS_MALLOC_CHAR(ret, 3);
  strcat(ret, "< ");
  if (mode & KLVTS_ONLY_ACTUAL_SPACE) {
    inx = kfs_lang_env_actual_space(kfsLangEnv);
    tmp = var_stack_to_string(inx);
    ret = realloc(ret, strlen(ret)+strlen(tmp)+3);
    strcat(ret, tmp);
    strcat(ret, ", ");
    free(tmp);
  } else {
    list_for_each_entry(inx, &kfsLangEnv->varStackStack, lstEnv) {
      tmp = var_stack_to_string(inx);
      ret = realloc(ret, strlen(ret)+strlen(tmp)+3);
      strcat(ret, tmp);
      strcat(ret, ", ");
      free(tmp);
    }
  }
  ret[strlen(ret)-2] = ' ';
  ret[strlen(ret)-1] = '>';
  return ret;
}

/////////

char *replace_system_props(regex_t regex, char *input) {
  //KFS_TRACE("start replace_system_props('%s')", input);
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
      //KFS_TRACE("name: '%s', replace: '%s'", name, replace);
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
    //KFS_TRACE("start replace_system_props('%s') -> '%s'", input, output);
    return output;
  } else if (result == REG_NOMATCH) {
    //KFS_DEBUG("No match: %s", input);
  } else {
    char msgbuf[1024];
    regerror(result, &regex, msgbuf, sizeof(msgbuf));
    KFS_ERROR("Regex match failed: %s", msgbuf);
  }
  //KFS_TRACE("start replace_system_props('%s') w/o changes", input);
  return strdup(input);
}

Value *eval_string(KfsLangEnv *env, char *value) {
  int evalSys = value[0]=='"';
  value+=1;value[strlen(value)-1] = '\0';
  //KFS_TRACE("eval_string: %i && %i = %i", env->useStringSysReplace, evalSys, env->useStringSysReplace && evalSys);
  if (env->useStringSysReplace && evalSys) {
    char *copy_value = replace_system_props(env->stringSysReplace, value);
    Value * ret = value_new_string(copy_value);
    free(copy_value);
    return ret;
  }
  return value_new_string(value);
}

Value *kfs_lang_eval_value(KfsLangEnv *kfsLangEnv, Expression *e, int options) {
  if (e == NULL) {
    return NULL;
  }
  Value *lv, *rv, *result;
  Expression *expr;
  char *str;

  switch (e->type) {
    case eBREAK:
      return value_new(FC_Break);
    case eCONTI:
      return value_new(FC_Conti);
    case eIntVALUE:
      return value_new_int(e->lValue);
    case eDoubleVALUE:
      return value_new_double(e->dValue);
    case eBoolVALUE:
      return value_new_bool(e->lValue);
    case eStringVALUE:
      return eval_string(kfsLangEnv, e->str);
    case eListVALUE:
      lv = value_new_list();
      if (lv == NULL) {
        KFS_ERROR("Cannot obtain new LIST value(%i)", 0);
        return NULL;
      }
      Expression *inx; list_for_each_entry(inx, &e->list, upHandle) {
        rv = kfs_lang_eval_value(kfsLangEnv, inx, KLE_EVAL_NORMAL);
        if (rv != NULL) {
          //if ((options & KLE_EVAL_FCE_CALL) && (rv->type == FC_Return)) {
            // remove return
          //}
          if ((rv->type == FC_Break) || (rv->type == FC_Conti) || (rv->type == FC_Return)) {
            value_delete(lv);
            return rv;
          }
          value_list_add(lv, rv);
        }
      }
      return lv;
    case eObjectVALUE:
      lv = value_new_object();
      DictItem *iny; list_for_each_entry(iny, &e->object->lst, lst) {
        value_object_add(lv, iny->name, kfs_lang_eval_value(kfsLangEnv, (Expression *)iny->data, KLE_EVAL_FCE_CALL));
      }
      return lv;
    case eMULTIPLY:
      result = value_mul((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eADD:
      result = value_plus((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eMINUS:
      result = value_minus((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eDIVIDE:
      result = value_divide((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eMODULO:
      result = value_mod((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case ePOWER:
      result = value_power((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eLT:
      result = value_lt((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eLE:
      result = value_le((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eEQ:
      result = value_eq((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eNE:
      result = value_ne((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eGT:
      result = value_gt((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eGE:
      result = value_ge((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eAND:
      result = value_and((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eOR:
      result = value_or((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)), (rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL)));
      value_delete(lv); value_delete(rv);
      return result;
    case eNOT:
      result = value_not((lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL)));
      value_delete(lv);
      return result;
    case eUNARY_MINUS:
      result = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL);
      if (result->type == Int) {
        result->iValue = -1*result->iValue;
      } else if (result->type == Double) {
        result->dValue = -1*result->dValue;
      }
      return result;
    case eDOT:
      lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL);
      if (lv == NULL) {
        KFS_ERROR("Cannot evaluate object", NULL);
        return NULL;
      }
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
      rv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL);
      if (rv->type != Int) {
        KFS_ERROR("Array access - index type must be INT (%i)", rv->type);
        value_delete(rv);
        return NULL;
      }
      int position = rv->iValue;
      value_delete(rv);
      lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL);
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
      lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL);
      switch (lv->type) {
        case FC_Break:
        case FC_Return:
        case FC_Conti:
        case Empty:
          KFS_ERROR("Cannot convert Flow-Controll into int", NULL);
          return NULL;
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
    case eVAR:
      return value_copy(kfs_lang_get_var(kfsLangEnv, e->str));
    case eASSIGN_VAR:
      lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL);
      if (lv != NULL) {
        kfs_lang_set_var(kfsLangEnv, strdup(e->str), value_copy(lv));
      } else {
        KFS_ERROR("Cannot assign value to variable: %s", e->str);
      }
      return lv;
    case eBLOCK:
      kfs_lang_env_space_add_vars(kfsLangEnv, NULL);
      lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL);
      kfs_lang_env_space_del_vars(kfsLangEnv);
      return lv;
    case eIF:
      result = kfs_lang_eval_value(kfsLangEnv, e->next, KLE_EVAL_NORMAL);
      int block = 0; // false
      if (result == NULL) {
        KFS_ERROR("Empty result", NULL);
      } else if (result->type != Bool) {
        KFS_ERROR("Object access to non-boolean value (%i)", result->type);
        //value_delete(result);
        //return  NULL;
      } else if (result->iValue) {
        block = 1;
      }
      value_delete(result);
      if (block)
        return kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL);
      return kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL);
    case eWHILE:
      result = kfs_lang_eval_value(kfsLangEnv, e->next, KLE_EVAL_NORMAL);
      if (result == NULL) {
        KFS_ERROR("Empty result", NULL);
        return NULL;
      }
      if (result->type != Bool) {
        KFS_ERROR("Object access to non-boolean value (%i)", result->type);
        value_delete(result);
        return NULL;
      }
      while (result->iValue) {
        value_delete(result); result = NULL;
        lv = kfs_lang_eval_value(kfsLangEnv, e->right, KLE_EVAL_NORMAL);
        if (lv->type == FC_Return) {
          return lv;
        }
        if (lv->type == FC_Break) {
          value_delete(lv);
          break;
        }
        value_delete(lv);
        result = kfs_lang_eval_value(kfsLangEnv, e->next, KLE_EVAL_NORMAL);
      }
      value_delete(result);
      return NULL;
    case ePRINT:
      lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL);
      str = value_to_string(lv, VALUE_TO_STRING_STR_DEFAULT);
      printf("%s",str);
      free(str);
      value_delete(lv);
      return NULL;
    case eDUMP:
      str = kfs_lang_vars_to_string(kfsLangEnv, KLVTS_ALL_SPACES);
      printf("%s\n",str);
      free(str);
      return NULL;
    case eRETURN:
      lv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL);
      if (lv->type != List) {
        rv = value_new_list();
        value_list_add(rv, lv);
      } else {
        rv = lv;
      }
      rv->type = FC_Return; // a little hack, let's will see some problems
      return rv;
    case eFUNCTION_DEF:
      // remove old definition
      list_for_each_entry(expr, &kfsLangEnv->functions, fceListHandle) {
        if (!strcmp(e->str, expr->str)) {
          list_del(&expr->fceListHandle);
          break;
        }
      }
      list_add(&e->fceListHandle, &kfsLangEnv->functions);
      return NULL;
    case eFUNCTION_CALL:
      expr = NULL; list_for_each_entry(expr, &kfsLangEnv->functions, fceListHandle) {
        if (!strcmp(e->str, expr->str)) {
          break;
        }
      }
      if (expr == NULL) {
        KFS_ERROR("Cannot find function %s", e->str);
        return NULL;
      }
      rv = kfs_lang_eval_value(kfsLangEnv, e->left, KLE_EVAL_NORMAL);
      kfs_lang_env_add_space(kfsLangEnv, rv);
      value_delete(rv);
      lv = kfs_lang_eval_value(kfsLangEnv, expr->left, KLE_EVAL_FCE_CALL);
      kfs_lang_env_remove_space(kfsLangEnv);

      if (lv != NULL) {
        if (lv->type == FC_Return) {
          lv->type = List; // remove hack
        }
        return value_delist(lv);
      }
      return NULL;
    case eSET_EMPTY:
      //
      return NULL;
    case eIS_EMPTY:
      return value_new_bool(kfs_lang_get_var(kfsLangEnv, e->str)->type == Empty);
  }
  return NULL;
}

Value *kfs_lang_eval(KfsLangEnv *kfsLangEnv, char *code, Options* options) {
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  // remove last expressions
  if (kfsLangEnv->expression != NULL) {
    expression_delete(kfsLangEnv->expression);
    kfsLangEnv->expression = NULL;
  }

  // remove last variables
  varStackStack_cleanup(kfsLangEnv);
  // and prepare new space
  kfs_lang_env_add_space(kfsLangEnv, NULL);


  if (yylex_init(&scanner)) {
    KFS_ERROR("Cannot init yylex", NULL);
    return NULL;
  }
  state = yy_scan_string(code, scanner);
  if (yyparse(kfsLangEnv, scanner, options)) {
    KFS_ERROR("Cannot parse code %s", code);
    return NULL;
  }
  yy_delete_buffer(state, scanner);
  yylex_destroy(scanner);
  Value *result = kfs_lang_eval_value(kfsLangEnv, kfsLangEnv->expression, KLE_EVAL_NORMAL);
  if (result->type == FC_Return) {
    result->type = List; // remove hack
  }
  if (kfsLangEnv->expression != NULL) {
    kfs_lang_env_cleanup_functions(kfsLangEnv);
    expression_delete(kfsLangEnv->expression);
    kfsLangEnv->expression = NULL;
  }


  return value_delist(result);
}