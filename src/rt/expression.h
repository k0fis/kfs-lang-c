#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "utils.h"

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

typedef enum tagOperationType
{
    eIntVALUE, eDoubleVALUE, eBoolVALUE, eListVALUE, eObjectVALUE, eStringVALUE,
    eMULTIPLY, eADD, eMINUS, eDIVIDE, eMODULO, ePOWER, // 11
    eLT, eLE, eEQ, eNE, eGT, eGE, eAND, eOR, eNOT, eUNARY_MINUS, // 21
    eDOT, eARRAY_ACCESS, eINT
} OperationType;

typedef struct tagExpression {
  OperationType type;

  int lValue;
  double dValue;
  ll_t lst;
  struct hashmap *object;
  char *str;

  struct tagExpression *left;
  struct tagExpression *right;
} Expression;

typedef struct tagNamedExpression {
  char *name;
  Expression *expression;
} NamedExpression;

Expression *expression_create_integer(int value);
Expression *expression_create_double(double value);
Expression *expression_create_bool(int value);
Expression *expression_create_string(char *value);
Expression *expression_create_list();
Expression *expression_create_object();

Expression *expression_create_binary_operation(OperationType type, Expression *left, Expression *right);
Expression *expression_create_dot_operation(Expression *left, char *name);
//Expression *expression_create_array_access_operation(Expression *left, int inx);

void expression_delete(Expression *b);

Expression *expression_add_list_item(Expression *lst, Expression *value);
Expression *expression_add_object_item(Expression *object, char *name, Expression *value);
Expression *expression_get_object_item(Expression *object, char *name, Expression *value);

#if defined(__cplusplus)
}
#endif  // __cplusplus

#endif