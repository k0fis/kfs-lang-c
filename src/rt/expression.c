#include "expression.h"

Expression *expression_new(OperationType type) {
  KFS_MALLOC(Expression, expression);
  expression->type = type;
  expression->lValue = 0;
  expression->dValue = 0;
  expression->lst.next = &expression->lst;
  expression->lst.prev = &expression->lst;
  expression->left = NULL;
  expression->right = NULL;
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
  if (expression == NULL) return NULL;
  return expression;
}

uint64_t named_expr_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    NamedExpression *nv = (NamedExpression *)item;
    return hashmap_sip(nv->name, strlen(nv->name), seed0, seed1);
}

int named_expr_compare(const void *a, const void *b, void *udata) {
    NamedExpression *nva = (NamedExpression *)a;
    NamedExpression *nvb = (NamedExpression *)b;
    return strcmp(nva->name, nvb->name);
}

void named_expr_delete_from_hash(void *nvv) {
    NamedExpression *nv = (NamedExpression*)nvv;
    if (nv->name != NULL) {
      free(nv->name);
    }
    if (nv->expression != NULL) {
      expression_delete(nv->expression);
    }
}

Expression *expression_create_object() {
  Expression *expression = expression_new(eObjectVALUE);
  if (expression == NULL) return NULL;
  expression->object = hashmap_new(sizeof(NamedExpression), 12, 5, 11, named_expr_hash,
        named_expr_compare, named_expr_delete_from_hash, NULL);
  return expression;
}

Expression *expression_create_binary_operation(OperationType type, Expression *left, Expression *right) {
  Expression *expression = expression_new(type);
  if (expression == NULL) return NULL;
  expression->left = left;
  expression->right = right;
  return expression;
}

void expression_delete(Expression *expression) {
  if (expression == NULL) return;
  expression_delete(expression->left);
  expression_delete(expression->right);
  if (expression->str != NULL) {
    free(expression->str);
  }
  if (expression->type == eListVALUE) {
    Expression *inx, *tmp; list_for_each_entry_safe(inx, tmp, &expression->lst, lst) {
      list_del(&inx->lst);
      expression_delete(inx);
    }
  } else if (expression->type == eObjectVALUE) {
    hashmap_free(expression->object);
  }
  free(expression);
}

Expression *expression_add_list_item(Expression *list, Expression *value) {
  if (list->type != eListVALUE) {
    KFS_ERROR("Cannot add item into non list expression");
  } else {
    list_add_tail(&value->lst, &list->lst);
  }
  return list;
}

Expression *expression_add_object_item(Expression *object, char *name, Expression *value) {
  if (object->type != eObjectVALUE) {
    KFS_ERROR("Cannot add item into non object");
    return object;
  }
  NamedExpression *found = (NamedExpression*)hashmap_get(object->object, &(NamedExpression){.name=name});
  if (found == NULL) {
    KFS_MALLOC(NamedExpression, ne);
    ne->name = strdup(name);
    ne->expression = value;
    hashmap_set(object->object, ne);
  } else {
    expression_delete(found->expression);
    found->expression = value;
  }
  return object;
}

Expression *expression_get_object_item(Expression *object, char *name, Expression *value) {
  if (object->type != eObjectVALUE) {
    KFS_ERROR("Cannot get items from non object expressions");
  } else {
    NamedExpression *ne = (NamedExpression*)hashmap_get(object->object, &(NamedExpression){.name=name});
    if (ne != NULL) {
      return ne->expression;
    }
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

/*
Expression *expression_create_array_access_operation(Expression *left, int inx) {
   if (left->type != eListVALUE) {
      KFS_ERROR("Cannot array access into non array expression");
    } else {
      Expression *expression = expression_new(eARRAY_ACCESS);
      if (expression == NULL) return NULL;
      expression->left = left;
      expression->lValue = inx;
      return expression;
    }
    return NULL;
}
*/