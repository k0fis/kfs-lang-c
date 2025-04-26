#include "Expression.h"

Expression *expression_new() {
    Expression *b = (Expression *)malloc(sizeof(Expression));
    if (b == NULL) return NULL;
    b->type = eIntVALUE;
    b->lValue = 0;
    b->left = NULL;
    b->right = NULL;
    return b;
}

Expression *expression_create_integer(int value) {
    Expression *b = expression_new();
    if (b == NULL) return NULL;
    b->type = eIntVALUE;
    b->lValue = value;
    return b;
}

Expression *expression_create_binary_operation(OperationType type, Expression *left, Expression *right) {
    Expression *b = expression_new();
    if (b == NULL) return NULL;
    b->type = type;
    b->left = left;
    b->right = right;
    return b;
}

void expression_delete(Expression *b) {
    if (b == NULL) return;
    expression_delete(b->left);
    expression_delete(b->right);
    free(b);
}