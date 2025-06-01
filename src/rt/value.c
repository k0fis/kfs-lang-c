#include "value.h"

#define defaultPrefix  "  "

Value *value_new(ValueType type) {
  KFS_MALLOC(Value, value);
  value->type = type;
  KFS_LST_INIT(value->listValue);
  KFS_LST_INIT(value->handle);
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
  value->oValue = dict_new((element_free)value_delete);
  return value;
}

Value *value_copy(Value *value) {
  if (value == NULL) {
    KFS_ERROR("Cannot create copy of (%p)", value);
    return NULL;
  }
  Value *result, *inx;
  size_t iter; void *item;
  switch (value->type) {
    case Bool:
    case Int:
      return value_new_int(value->iValue);
    case Double:
      return value_new_double(value->dValue);
    case String:
      return value_new_string(value->sValue);
    case List:
      result = value_new_list();
      inx = NULL; list_for_each_entry(inx, &value->listValue, handle) {
        value_list_add(result, value_copy(inx));
      }
      return result;
    case Object:
      result = value_new_object();
      DictItem *iny; list_for_each_entry(iny, &value->oValue->lst, lst) {
        value_object_add(result, iny->name, value_copy((Value*)iny->data));
      }
      return result;
    case FC_Break:
    case FC_Conti:
    case FC_Return:
    case Empty:
      return value_new(value->type);
  }
  KFS_ERROR("Error in copying values, type: %i", value->type);
  return NULL;
}

int value_object_add(Value *obj, char *name, Value *val) {
  if (obj->type != Object) {
    return -2;
  }
  dict_set(obj->oValue, strdup(name), val, KFS_DICT_SET_NORMAL);
  return 0;
}

Value *value_object_get(Value *obj, char *name) {
  if (obj->type == Object) {
    return dict_get(obj->oValue, name);
  }
  return NULL;
}

int value_list_add(Value *list, Value *value) {
  if (list->type != List) {
    KFS_ERROR("Try add item into array, but expression is not array, but type = %i", list->type);
    return -1;
  }
  if (value != NULL)
    list_add_tail(&value->handle, &list->listValue);
  return 0;
}

Value *value_list_get(Value *list, int index) {
  if (index < 0) {
    KFS_ERROR("Try to access out of bounds, index cannot be negative (%i)", index);
    return NULL;
  }
  if (list->type != List) {
    KFS_ERROR("Try add item into array, but expression is not array, type = %i", list->type);
    return NULL;
  }
  int iny = 0; Value *inx = NULL; list_for_each_entry(inx, &list->listValue, handle) {
     if (iny == index) {
        return inx;
     }
     iny++;
  }
  KFS_ERROR("Try to access out of bounds(%i -> %i)", iny, index);
  return NULL;
}

void value_delete(Value *value) {
  if (value != NULL) {
    if (value->sValue != NULL) {
      free(value->sValue);
    }
    if (value->type == List) {
      Value *inx, *tmp; list_for_each_entry_safe(inx, tmp, &value->listValue, handle) {
        list_del(&inx->handle);
        value_delete(inx);
      }
    }
    if (value->type == Object) {
      dict_delete(value->oValue);
    }
    free(value);
  }
}


Value *value_delist(Value *item) {
  if (item == NULL) return NULL;
  if (item->type != List) return item;
  int cnt= 0; Value *inx, *tmp; list_for_each_entry(inx, &item->listValue, handle) {
    cnt++;
  }
  if (cnt != 1) return item;
  list_for_each_entry_safe(inx, tmp, &item->listValue, handle) {
    list_del(&inx->handle);
    value_delete(item);
    return inx;
  }
  KFS_ERROR("Bad state!", NULL);
  return NULL;
}

char *trim_str_buffer(char *buffer) {
  return realloc(buffer, strlen(buffer)+1);
}

char *value_to_string(Value *value, int mode) {
  if (value == NULL) {
    KFS_MALLOC_CHAR(tmp, 256);
    snprintf(tmp, 255, "NULL");
    return trim_str_buffer(tmp);
  } else {
    switch (value->type) {
      case FC_Break:
        return strdup("FC_Break");
      case FC_Conti:
        return strdup("FC_Conti");
      case FC_Return:
        return strdup("FC_Return");
      case Empty:
        return strdup("");
      case Int: {
        KFS_MALLOC_CHAR(tmp, 256);
        snprintf(tmp, 255, "%i", value->iValue);
        return trim_str_buffer(tmp);
      }
      case Double: {
        KFS_MALLOC_CHAR(tmp, 256);
        snprintf(tmp, 255, "%lf", value->dValue);
        return trim_str_buffer(tmp);
      }
      case Bool: {
        KFS_MALLOC_CHAR(tmp, 256);
        snprintf(tmp, 255, "%s", value->iValue?"true":"false");
        return trim_str_buffer(tmp);
      }
      case String: {
        if (mode & VALUE_TO_STRING_STR_WITH_APOSTROPHE) {
          KFS_MALLOC_CHAR(tmp, strlen(value->sValue)+3);
          snprintf(tmp, strlen(value->sValue)+3, "'%s'", value->sValue);
          return tmp;
        } else {
          return strdup(value->sValue);
        }
      }
      case List: {
        KFS_MALLOC_CHAR(ret, 3);
        strcat(ret, "[ ");
        Value *inx = NULL; list_for_each_entry(inx, &value->listValue, handle) {
          char *val = value_to_string(inx, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
          ret = realloc(ret, strlen(ret) + strlen(val)+3);
          strcat(ret, val);
          strcat(ret, ", ");
          free(val);
        }
        ret[strlen(ret)-1] = ']';
        if (strlen(ret) > 2) {
          ret[strlen(ret)-2] = ' ';
        }
        return ret;
      }
      case Object: {
        KFS_MALLOC_CHAR(ret,3);
        strcat(ret, "{ ");
        DictItem *inx; list_for_each_entry(inx, &value->oValue->lst, lst) {
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
        return trim_str_buffer(ret);
      }
    }
  }
}

int value_read_file(char *filename, Value **result, Options *options) {
  int ret;
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    KFS_ERROR("Cannot open file %s", filename);
    return RET_VALUE_CANNOT_OPEN_FILE;
  }
  long size = -1;
  if ((ret=fseek(file, 0, SEEK_END))) {
    KFS_ERROR("Cannot seek to end of file %s - %s", filename, strerror(ret));
  } else {
    size = ftell(file);
    if ((ret = fseek(file, 0, SEEK_SET))) {
      KFS_ERROR("Cannot seek to begin of file %s - %s", filename, strerror(ret));
    }
  }
  if (size < 0) {
    KFS_ERROR("Cannot read file %s - %s", filename, strerror(size));
    fclose(file);
    return RET_VALUE_CANNOT_OPEN_FILE;
  }
  if (size >= options->maxReadFileLength) {
    KFS_ERROR("file %s has bigger size %ld than difined max: %ld", filename, size, options->maxReadFileLength);
    fclose(file);
    return RET_VALUE_FILE_OVERSIZED;
  }
  KFS_MALLOC_CHAR(str, size+1);
  if (str == NULL) {
    KFS_ERROR("Cannot allocate memory for file %s", filename);
    fclose(file);
    return RET_VALUE_FILE_NON_ALLOC;
  }
  size_t readed = fread (str, 1, size, file);
  if (readed != size) {
    KFS_ERROR("Cannot read full size fole: %s defined len: %ld, readed: %ld", filename, size, readed);
  }
  *result = value_new_string(str);
  free(str);
  if ((ret=fclose(file))) {
    KFS_ERROR("Cannot close file %s - %s", filename, strerror(ret));
    return RET_VALUE_CANNOT_CLOSE_FILE;
  }
  return RET_OK;
}
