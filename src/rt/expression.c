#include "expression.h"

Expression *expression_new(OperationType type) {
  KFS_MALLOC(Expression, expression);
  expression->type = type;
  expression->lValue = 0;
  expression->dValue = 0;
  KFS_LST_INIT(expression->list);
  KFS_LST_INIT(expression->upHandle);
  expression->left = NULL;
  expression->right = NULL;
  expression->next = NULL;
  expression->object = NULL;
  expression->str = NULL;
  return expression;
}

Expression *expression_create_integer(int value) {
  Expression *expression = expression_new(eIntVALUE);
  if (expression == NULL) return NULL;
  expression->lValue = value;
  return expression;
}

Expression *expression_create_double(double value) {
  Expression *expression = expression_new(eDoubleVALUE);
  if (expression == NULL) return NULL;
  expression->dValue = value;
  return expression;
}

Expression *expression_create_bool(int value) {
  Expression *expression = expression_new(eBoolVALUE);
  if (expression == NULL) return NULL;
  expression->lValue = value;
  return expression;
}

Expression *expression_create_string(char *value) {
  Expression *expression = expression_new(eStringVALUE);
  if (expression == NULL) return NULL;
  expression->str = strdup(value);
  return expression;
}

Expression *expression_create_list() {
  Expression *expression = expression_new(eListVALUE);
  return expression;
}

Expression *expression_create_object() {
  Expression *expression = expression_new(eObjectVALUE);
  if (expression == NULL) return NULL;
  expression->object = dict_new((element_free)expression_delete);
  return expression;
}

Expression *expression_create_binary_operation(OperationType type, Expression *left, Expression *right) {
  Expression *expression = expression_new(type);
  if (expression == NULL) return NULL;
  expression->left = left;
  expression->right = right;
  return expression;
}

Expression *expression_create_variable_assign(char *name, Expression *value) {
  Expression *expression = expression_new(eASSIGN_VAR);
  if (expression == NULL) return NULL;
  expression->left = value;
  expression->str = strdup(name);
  return expression;
}

Expression *expression_create_variable(char *name) {
  Expression *expression = expression_new(eVAR);
  if (expression == NULL) return NULL;
  expression->str = strdup(name);
  return expression;
}

Expression *expression_create_new_block(Expression *block) {
  Expression *expression = expression_new(eBLOCK);
  if (expression == NULL) return NULL;
  expression->left = block;
  return expression;
}

Expression *expression_create_if(Expression *query, Expression *blockTrue, Expression *blockFalse) {
  Expression *expression = expression_new(eIF);
  if (expression == NULL) return NULL;
  expression->next = query;
  expression->right = blockTrue;
  expression->left = blockFalse;
  return expression;
}

Expression *expression_create_while(Expression *query, Expression *block) {
  Expression *expression = expression_new(eWHILE);
  if (expression == NULL) return NULL;
  expression->next = query;
  expression->right = block;
  return expression;
}

Expression *expression_create_break() {
  Expression *expression = expression_new(eBREAK);
  return expression;
}

Expression *expression_create_continue() {
  Expression *expression = expression_new(eCONTI);
  return expression;
}


Expression *expression_delist(Expression *item) {
  if (item == NULL) return NULL;
  if (item->type != eListVALUE) return item;
  int cnt= 0; Expression *inx, *tmp; list_for_each_entry(inx, &item->list, upHandle) {
    cnt++;
  }
  if (cnt != 1) return item;
  list_for_each_entry_safe(inx, tmp, &item->list, upHandle) {
    list_del(&inx->upHandle);
    expression_delete(item);
    return inx;
  }
  KFS_ERROR("Bad state!", NULL);
  return NULL;
}

void expression_delete(Expression *expression) {
  if (expression == NULL) return;
  expression_delete(expression->left);
  expression_delete(expression->right);
  expression_delete(expression->next);
  if (expression->str != NULL) {
    free(expression->str);
  }
  if (expression->type == eListVALUE) {
    Expression *inx, *tmp; list_for_each_entry_safe(inx, tmp, &expression->list, upHandle) {
      list_del(&inx->upHandle);
      expression_delete(inx);
    }
  } else if (expression->type == eObjectVALUE) {
    dict_delete(expression->object);
  }
  free(expression);
}

Expression *expression_add_list_item(Expression *list, Expression *value) {
  if (list->type != eListVALUE) {
    KFS_ERROR("Cannot add item into non list expression, type = %i", list->type);
  } else {
    list_add_tail(&value->upHandle, &list->list);
  }
  return list;
}

Expression *expression_add_object_item(Expression *object, char *name, Expression *value) {
  if (object->type != eObjectVALUE) {
    KFS_ERROR("Cannot add item into non object(%s)", name);
    return object;
  }
  int ret = dict_set(object->object, strdup(name), value, KFS_DICT_SET_NORMAL);
  if (ret != KFS_DICT_SET_RET_OK){
    KFS_ERROR("Bad SET object : %i", ret);
  }
  return object;
}

Expression *expression_get_object_item(Expression *object, char *name, Expression *value) {
  if (object->type != eObjectVALUE) {
    KFS_ERROR("Cannot get items from non object expressions(%s)", name);
  } else {
    return (Expression *)dict_get(object->object, name);
  }
  return NULL;
}

Expression *expression_create_dot_operation(Expression *left, char *name) {
    Expression *expression = expression_new(eDOT);
    if (expression == NULL) return NULL;
    expression->left = left;
    expression->str = strdup(name);
    return expression;
}
