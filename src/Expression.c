#include "Expression.h"

Expression *expression_new() {
  Expression *expression = (Expression *)malloc(sizeof(Expression));
  if (expression == NULL) return NULL;
  expression->type = eIntVALUE;
  expression->lValue = 0;
  expression->left = NULL;
  expression->right = NULL;
  return expression;
}

Expression *expression_create_integer(int value) {
  Expression *expression = expression_new();
  if (expression == NULL) return NULL;
  expression->type = eIntVALUE;
  expression->lValue = value;
  return expression;
}

Expression *expression_create_double(double value) {
  Expression *expression = expression_new();
  if (expression == NULL) return NULL;
  expression->type = eDoubleVALUE;
  expression->dValue = value;
  return expression;
}

Expression *expression_create_binary_operation(OperationType type, Expression *left, Expression *right) {
  Expression *expression = expression_new();
  if (expression == NULL) return NULL;
  expression->type = type;
  expression->left = left;
  expression->right = right;
  return expression;
}

void expression_delete(Expression *expression) {
    if (expression == NULL) return;
    expression_delete(expression->left);
    expression_delete(expression->right);
    free(expression);
}