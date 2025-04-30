#include "value.h"
#include "named_value.h"

#define defaultPrefix  "  "

Value *value_new(ValueType type) {
  KFS_MALLOC(Value, value);
  value->type = type;
  value->lValue.next = &value->lValue;
  value->lValue.prev = &value->lValue;
  return value;
}

Value *value_new_int(int iValue) {
  Value *value = value_new(Int);
  value->iValue = iValue;
  return value;
}

Value *value_new_double(double dValue) {
  Value *value = value_new(Double);
  value->dValue = dValue;
  return value;
}

Value *value_new_bool(int bValue){
  Value *value = value_new(Bool);
  value->iValue = bValue;
  return value;
}

Value *value_new_string(char *sValue) {
  Value *value = value_new(String);
  value->sValue = strdup(sValue);
  return value;
}

Value *value_new_list(){
  Value *value = value_new(List);
  return value;
}


Value *value_new_object() {
  Value *value = value_new(Object);
  value->oValue = named_value_create_hashmap();
  return value;
}

Value *value_copy(Value *value) {
  if (value == NULL) {
    KFS_ERROR("Cannot create copy of NULL");
    return NULL;
  }
  Value *result, *inx;
  size_t iter; void *item;
  switch (value->type) {
    case Bool:
    case Int:
      return value_new_int(value->iValue);
    break;
    case Double:
      return value_new_double(value->dValue);
    break;
    case String:
      return value_new_string(value->sValue);
    break;
    case List:
      result = value_new_list();
      inx = NULL; list_for_each_entry(inx, &value->lValue, lValue) {
        value_list_add(result, value_copy(inx));
      }
      return result;
    break;
    case Object:
       result = value_new_object();
        iter = 0; while (hashmap_iter(value->oValue, &iter, &item)) {
            value_object_add(result, ((NamedValue *)item)->name, value_copy(((NamedValue *)item)->value));
        }
      return result;
    break;
  }
  KFS_ERROR("Error in copying values");
  return NULL;
}

int value_object_add(Value *obj, char *name, Value *val) {
  if (obj->type != Object) {
    return -2;
  }
  named_value_set(obj->oValue, name, val);
  return 0;
}

Value *value_object_get(Value *obj, char *name) {
  if (obj->type == Object) {
    return named_value_get(obj->oValue, name);
  }
  return NULL;
}

int value_list_add(Value *list, Value *value) {
  if (list->type != List) {
    KFS_ERROR("Try add item into array, but expression is not array");
    return -1;
  }
  list_add_tail(&value->lValue, &list->lValue);
  return 0;
}

Value *value_list_get(Value *list, int index) {
  if (index < 0) {
    KFS_ERROR("Try to access out of bounds, index cannot be negative");
    return NULL;
  }
  if (list->type != List) {
    KFS_ERROR("Try add item into array, but expression is not array");
    return NULL;
  }
  int iny = 0; Value *inx = NULL; list_for_each_entry(inx, &list->lValue, lValue) {
     if (iny == index) {
        return inx;
     }
     iny++;
  }
  KFS_ERROR("Try to access out of bounds");
  return NULL;
}

void value_delete(Value *value) {
   if (value->type == String) {
      free(value->sValue);
   }
   if (value->type == List) {
      Value *inx, *tmp; list_for_each_entry_safe(inx, tmp, &value->lValue, lValue) {
          list_del(&inx->lValue);
          value_delete(inx);
      }
   }
   if (value->type == Object) {
      hashmap_free(value->oValue);
   }
   free(value);
}

void _value_print(Value *value, char *prefix, char *postfix) {
  if (value == NULL) {
    printf("%sNULL%s", prefix, postfix);
  } else {
    switch (value->type) {
      case Int:
        printf("%s%d%s", prefix, value->iValue, postfix);
        break;
      case Double:
        printf("%s%lf%s", prefix, value->dValue, postfix);
        break;
      case Bool:
        if (value->iValue) printf("%strue%s", prefix, postfix);
        else printf("%sfalse%s", prefix, postfix);
        break;
      case String:
        printf("%s\"%s\"%s", prefix, value->sValue, postfix);
        break;
      case List:
        printf("%s[", prefix);
        Value *inx = NULL; list_for_each_entry(inx, &value->lValue, lValue) {
            _value_print(inx, " ", ",");
        }
        printf("]%s", postfix);
        break;
      case Object:
        printf("%s{", prefix);
        size_t iter = 0;
        void *item;
        while (hashmap_iter(value->oValue, &iter, &item)) {
            _named_value_print((NamedValue *)item, "; ");
        }
        printf("}%s", postfix);
        break;
      default:
        printf("%s unknown %s", prefix, postfix);
    }
  }
}

void value_print(Value *value) {
  _value_print(value, defaultPrefix, "\n");
}
