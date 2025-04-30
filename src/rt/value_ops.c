#include "value.h"
#include "named_value.h"

#define BUFF_SIZE 50

Value *value_plus(Value *left, Value *right) {
  char buffer[BUFF_SIZE];
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_int(left->iValue + right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->iValue + right->dValue);
    }
    if (right->type == String) {
      snprintf(buffer, BUFF_SIZE, "%i", left->iValue);
      KFS_MALLOC_CHAR(ns, 1+strlen(buffer)+strlen(right->sValue));
      strcat(ns, buffer);
      strcat(ns, right->sValue);
      return value_new_string(ns);
    }
  }
  if (left->type == Double) {
    if (right->type == Int) {
      return value_new_double(left->dValue + right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->dValue + right->dValue);
    }
    if (right->type == String) {
      snprintf(buffer, BUFF_SIZE, "%lf", left->dValue);
      KFS_MALLOC_CHAR(ns, 1+strlen(buffer)+strlen(right->sValue));
      strcat(ns, buffer);
      strcat(ns, right->sValue);
      return value_new_string(ns);
    }
  }
  if (left->type == Bool) {
      if (right->type == Bool) {
         return value_new_bool( left->iValue || right->iValue );
      }
      if (right->type == String) {
         snprintf(buffer, BUFF_SIZE, "%s", right->iValue?"true":"false");
         KFS_MALLOC_CHAR(ns, strlen(buffer) + strlen(left->sValue)+1);
         strcat(ns, buffer);
         strcat(ns, right->sValue);
         return value_new_string(ns);
      }
  }
  if (left->type == String) {
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
        KFS_MALLOC_CHAR(ns, strlen(buffer) + strlen(left->sValue)+1);
        strcat(ns, left->sValue);
        strcat(ns, buffer);
        return value_new_string(ns);
      }
      if (right->type == String) {
        KFS_MALLOC_CHAR(ns, strlen(right->sValue) + strlen(left->sValue)+1);
        strcat(ns, left->sValue);
        strcat(ns, right->sValue);
        return value_new_string(ns);
      }
  }
  return NULL;
}

Value *value_minus(Value *left, Value *right) {
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_int(left->iValue - right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->iValue - right->dValue);
    }
  }
  if (left->type == Double) {
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
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_int(left->iValue * right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->iValue * right->dValue);
    }
  }
  if (left->type == Double) {
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
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_int(left->iValue / right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->iValue / right->dValue);
    }
  }
  if (left->type == Double) {
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
    for (inx = list_entry((&left->lValue)->next, __typeof__(*inx), lValue),
         iny = list_entry((&right->lValue)->next, __typeof__(*iny), lValue)
        ;
          (&(inx->lValue) != &(left->lValue)) && (&(iny->lValue) != &(right->lValue))
        ;
          inx = list_entry(inx->lValue.next, __typeof__(*inx), lValue),
          iny = list_entry(iny->lValue.next, __typeof__(*iny), lValue)
      ) {
         //_value_print(inx, "- 1  ", " -"); value_print(iny);
          Value *eqVal = value_eq(inx, iny);
          eq = (eqVal->iValue)?1:0;
          value_delete(eqVal);
          if (!eq) break;
    }
    //printf("- 5 %i\n", eq);
    if (!eq) return value_new_bool(0);
    if ((&inx->lValue == &(left->lValue)) && (&iny->lValue == &(right->lValue))) {
      // both list have same size
      return value_new_bool(1);
    }
    return value_new_bool(0);
  }
  if ((left->type == Object) && (right->type == Object)) {
    if (hashmap_count(left->oValue) != hashmap_count(right->oValue)) {
      return value_new_bool(0);
    }
    int eq;
    size_t iter = 0;
    void *item;
    while (hashmap_iter(left->oValue, &iter, &item)) {
       NamedValue *itemx = (NamedValue *)item;
       Value *itemy = named_value_get(right->oValue, itemx->name);
       if (itemy == NULL) { eq = false; break; }
       Value *eqVal = value_eq(itemx->value, itemy);
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