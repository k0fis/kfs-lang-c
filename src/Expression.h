#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include <stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

typedef enum tagOperationType
{
    eIntVALUE,
    eDoubleVALUE,
    eMULTIPLY,
    eADD,
    eMINUS,
    eDIVIDE,
    eMODULO,
    ePOWER
} OperationType;

typedef struct tagExpression
{
    OperationType type;

    int lValue;
    double dValue;

    struct tagExpression *left;
    struct tagExpression *right;
} Expression;

Expression *expression_create_integer(int value);
Expression *expression_create_double(double value);

Expression *expression_create_binary_operation(OperationType type, Expression *left, Expression *right);

void expression_delete(Expression *b);

#if defined(__cplusplus)
}
#endif  // __cplusplus

#endif