#include "value.h"

#define BUFF_SIZE 50

Value *value_plus(Value *left, Value *right) {
  char buffer[BUFF_SIZE];
  if (left->type == Empty) {
    return value_copy(right);
  }
  if (left->type == Int) {
    if (right->type == Empty) {
      return value_copy(left);
    }
    if (right->type == Int) {
      return value_new_int(left->iValue + right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->iValue + right->dValue);
    }
    if (right->type == String) {
      snprintf(buffer, BUFF_SIZE, "%i", left->iValue);
      KFS_MALLOC_CHAR(ns, (int)(1+strlen(buffer)+strlen(right->sValue)));
      strcat(ns, buffer);
      strcat(ns, right->sValue);
      Value *ret =value_new_string(ns);
      free(ns);
      return ret;
    }
  }
  if (left->type == Double) {
    if (right->type == Empty) {
      return value_copy(left);
    }
    if (right->type == Int) {
      return value_new_double(left->dValue + right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->dValue + right->dValue);
    }
    if (right->type == String) {
      snprintf(buffer, BUFF_SIZE, "%lf", left->dValue);
      KFS_MALLOC_CHAR(ns, (int)(1+strlen(buffer)+strlen(right->sValue)));
      strcat(ns, buffer);
      strcat(ns, right->sValue);
      Value *ret = value_new_string(ns);
      free(ns);
      return ret;
    }
  }
  if (left->type == Bool) {
      if (right->type == Empty) {
        return value_copy(left);
      }
      if (right->type == Bool) {
         return value_new_bool( left->iValue || right->iValue );
      }
      if (right->type == String) {
        snprintf(buffer, BUFF_SIZE, "%s", right->iValue?"true":"false");
        KFS_MALLOC_CHAR(ns, (int)(strlen(buffer) + strlen(left->sValue)+1));
        strcat(ns, buffer);
        strcat(ns, right->sValue);
        Value *ret = value_new_string(ns);
        free(ns);
        return ret;
      }
  }
  if (left->type == String) {
    if (right->type == Empty) {
      return value_copy(left);
    }
      buffer[0] = '\0';
      if (right->type == Int) {
        snprintf(buffer, BUFF_SIZE, "%i", right->iValue);
      }
      if (right->type == Double) {
        snprintf(buffer, BUFF_SIZE, "%lf", right->dValue);
      }
      if (right->type == Bool) {
        snprintf(buffer, BUFF_SIZE, "%s", right->iValue?"true":"false");
      }
      if (strlen(buffer) > 0) {
        KFS_MALLOC_CHAR(ns, (int)(strlen(buffer) + strlen(left->sValue)+1));
        strcat(ns, left->sValue);
        strcat(ns, buffer);
        Value *ret = value_new_string(ns);
        free(ns);
        return ret;
      }
      if (right->type == String) {
        KFS_MALLOC_CHAR(ns, (int)(strlen(right->sValue) + strlen(left->sValue)+1));
        strcat(ns, left->sValue);
        strcat(ns, right->sValue);
        Value *ret = value_new_string(ns);
        free(ns);
        return ret;
      }
      char * str = value_to_string(right, VALUE_TO_STRING_STR_DEFAULT);
      KFS_MALLOC_CHAR(ns, (int)(strlen(str) + strlen(left->sValue)+1));
      strcat(ns, left->sValue);
      strcat(ns, str);
      Value *ret = value_new_string(ns);
      free(ns);
      free(str);
      return ret;
  }
  return NULL;
}

Value *value_minus(Value *left, Value *right) {
  if (left->type == Empty) {
    return value_copy(right);
  }
  if (left->type == Int) {
    if (right->type == Empty) {
      return value_copy(left);
    }
    if (right->type == Int) {
      return value_new_int(left->iValue - right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->iValue - right->dValue);
    }
  }
  if (left->type == Double) {
    if (right->type == Empty) {
      return value_copy(left);
    }
    if (right->type == Int) {
      return value_new_double(left->dValue - right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->dValue - right->dValue);
    }
  }
  return NULL;
}

Value *value_mul(Value *left, Value *right) {
  if (left->type == Empty) {
    return value_copy(right);
  }
  if (left->type == Int) {
    if (right->type == Empty) {
      return value_copy(left);
    }
    if (right->type == Int) {
      return value_new_int(left->iValue * right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->iValue * right->dValue);
    }
  }
  if (left->type == Double) {
    if (right->type == Empty) {
      return value_copy(left);
    }
    if (right->type == Int) {
      return value_new_double(left->dValue * right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->dValue * right->dValue);
    }
  }
  if (left->type == Bool) {
      if (right->type == Bool) {
         return value_new_bool( left->iValue && right->iValue );
      }
  }
  return NULL;
}

Value *value_divide(Value *left, Value *right) {
  if (left->type == Empty) {
    return value_copy(right);
  }
  if (left->type == Int) {
    if (right->type == Empty) {
      return value_copy(left);
    }
    if (right->type == Int) {
      return value_new_int(left->iValue / right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->iValue / right->dValue);
    }
  }
  if (left->type == Double) {
    if (right->type == Empty) {
      return value_copy(left);
    }
    if (right->type == Int) {
      return value_new_double(left->dValue / right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->dValue / right->dValue);
    }
  }
  return NULL;
}

Value *value_power(Value *left, Value *right) {
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_int(left->iValue ^ right->iValue);
    }
  }
  return NULL;
}

Value *value_mod(Value *left, Value *right) {
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_int(left->iValue % right->iValue);
    }
  }
  return NULL;
}

Value *value_lt(Value *left, Value *right) {
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_bool(left->iValue < right->iValue);
    }
    if (right->type == Double) {
      return value_new_bool(left->iValue < right->dValue);
    }
  }
  if (left->type == Double) {
    if (right->type == Int) {
      return value_new_bool(left->dValue < right->iValue);
    }
    if (right->type == Double) {
      return value_new_bool(left->dValue < right->dValue);
    }
  }
  return NULL;
}

Value *value_le(Value *left, Value *right) {
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_bool(left->iValue <= right->iValue);
    }
    if (right->type == Double) {
      return value_new_bool(left->iValue <= right->dValue);
    }
  }
  if (left->type == Double) {
    if (right->type == Int) {
      return value_new_bool(left->dValue <= right->iValue);
    }
    if (right->type == Double) {
      return value_new_bool(left->dValue <= right->dValue);
    }
  }
  return NULL;
}

Value *value_eq(Value *left, Value *right) {
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_bool(left->iValue == right->iValue);
    }
    if (right->type == Double) {
      return value_new_bool(left->iValue == right->dValue);
    }
  }
  if (left->type == Double) {
    if (right->type == Int) {
      return value_new_bool(left->dValue == right->iValue);
    }
    if (right->type == Double) {
      return value_new_bool(left->dValue == right->dValue);
    }
  }
  if ((left->type == Bool) && (right->type == Bool)) {
    return value_new_bool(left->iValue == right->iValue);
  }

  if ((left->type == String) && (right->type == String)) {
    return value_new_bool(!strcmp(left->sValue, right->sValue));
  }
  if ((left->type == List) && (right->type == List)) {
    Value *inx = NULL, *iny = NULL;
    int eq = 0;
    //_value_print(left, " - cmp : ", " x"); value_print(right);
    for (inx = list_entry((&left->listValue)->next, __typeof__(*inx), handle),
         iny = list_entry((&right->listValue)->next, __typeof__(*iny), handle)
        ;
          (&(inx->handle) != &(left->listValue)) && (&(iny->handle) != &(right->listValue))
        ;
          inx = list_entry(inx->handle.next, __typeof__(*inx), handle),
          iny = list_entry(iny->handle.next, __typeof__(*iny), handle)
      ) {
         //_value_print(inx, "- 1  ", " -"); value_print(iny);
          Value *eqVal = value_eq(inx, iny);
          eq = (eqVal->iValue)?1:0;
          value_delete(eqVal);
          if (!eq) break;
    }
    //printf("- 5 %i\n", eq);
    if (!eq) return value_new_bool(0);
    if ((&inx->handle == &(left->listValue)) && (&iny->handle == &(right->listValue))) {
      // both list have same size
      return value_new_bool(1);
    }
    return value_new_bool(0);
  }
  if ((left->type == Object) && (right->type == Object)) {
    if (dict_count(left->oValue) != dict_count(right->oValue)) {
      return value_new_bool(0);
    }
    int eq;
    DictItem *inx; list_for_each_entry(inx, &left->oValue->lst, lst) {
       Value *itemy = dict_get(right->oValue, inx->name);
       if (itemy == NULL) { eq = 0; break; }
       Value *eqVal = value_eq((Value *)inx->data, itemy);
       eq = eqVal->iValue;
       value_delete(eqVal);
       if (!eq) break;
    }
    return value_new_bool(eq);
  }

  return NULL;
}

Value *value_ne(Value *left, Value *right) {
  Value *ret = value_eq(left, right);
  if (ret != NULL) {
    ret->iValue = !ret->iValue;
  }
  return ret;
}

Value *value_gt(Value *left, Value *right) {
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_bool(left->iValue > right->iValue);
    }
    if (right->type == Double) {
      return value_new_bool(left->iValue > right->dValue);
    }
  }
  if (left->type == Double) {
    if (right->type == Int) {
      return value_new_bool(left->dValue > right->iValue);
    }
    if (right->type == Double) {
      return value_new_bool(left->dValue > right->dValue);
    }
  }
  return NULL;
}

Value *value_ge(Value *left, Value *right) {
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_bool(left->iValue >= right->iValue);
    }
    if (right->type == Double) {
      return value_new_bool(left->iValue >= right->dValue);
    }
  }
  if (left->type == Double) {
    if (right->type == Int) {
      return value_new_bool(left->dValue >= right->iValue);
    }
    if (right->type == Double) {
      return value_new_bool(left->dValue >= right->dValue);
    }
  }
  return NULL;
}

Value *value_and(Value *left, Value *right) {
  if (left->type == Bool) {
    if (right->type == Bool) {
      return value_new_bool(left->iValue && right->iValue);
    }
  }
  return NULL;
}

Value *value_or(Value *left, Value *right) {
  if (left->type == Bool) {
    if (right->type == Bool) {
      return value_new_bool(left->iValue || right->iValue);
    }
  }
  return NULL;
}

Value *value_not(Value *left) {
  if (left->type == Bool) {
    return value_new_bool(!left->iValue);
  }
  return NULL;
}