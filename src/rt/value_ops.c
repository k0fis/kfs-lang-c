#include "value.h"

Value *value_plus(Value *left, Value *right) {
  char buffer[50];
  if (left->type == Int) {
    if (right->type == Int) {
      return value_new_int(left->iValue + right->iValue);
    }
    if (right->type == Double) {
      return value_new_double(left->iValue + right->dValue);
    }
    if (right->type == String) {
      sprintf(buffer, "%i", left->iValue);
      char *ns = malloc(sizeof(char)*(1+strlen(buffer)+strlen(right->sValue)));
      ns[0] = '\0';
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
      sprintf(buffer, "%lf", left->dValue);
      char *ns = malloc(sizeof(char)*(1+strlen(buffer)+strlen(right->sValue)));
      ns[0] = '\0';
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
         sprintf(buffer, "%s", right->iValue?"true":"false");
         char *ns = malloc(strlen(buffer) + strlen(left->sValue)+1);
         ns[0] = '\0';
         strcat(ns, buffer);
         strcat(ns, right->sValue);
         return value_new_string(ns);
      }
  }
  if (left->type == String) {
      buffer[0] = '\0';
      if (right->type == Int) {
        sprintf(buffer, "%i", right->iValue);
      }
      if (right->type == Double) {
        sprintf(buffer, "%lf", right->dValue);
      }
      if (right->type == Bool) {
        sprintf(buffer, "%s", right->iValue?"true":"false");
      }
      if (strlen(buffer) > 0) {
        char *ns = malloc(strlen(buffer) + strlen(left->sValue)+1);
        ns[0] = '\0';
        strcat(ns, left->sValue);
        strcat(ns, buffer);
        return value_new_string(ns);
      }
      if (right->type == String) {
        char *ns = malloc(strlen(right->sValue) + strlen(left->sValue)+1);
        ns[0] = '\0';
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
