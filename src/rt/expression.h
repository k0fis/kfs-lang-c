#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "kfs_dict.h"
#include "utils.h"

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

typedef enum tagOperationType
{
    eIntVALUE, eDoubleVALUE, eBoolVALUE, eListVALUE, eObjectVALUE, eStringVALUE, // 0..5
    eMULTIPLY, eADD, eMINUS, eDIVIDE, eMODULO, ePOWER, // 6..11
    eLT, eLE, eEQ, eNE, eGT, eGE, eAND, eOR, eNOT, eUNARY_MINUS, // 12..21
    eDOT, eARRAY_ACCESS, eINT, eASSIGN_VAR, eVAR, eBLOCK, eIF, // 22.28
    eWHILE, eBREAK, eCONTI, eRETURN,  // 29..32
    eFUNCTION_DEF, eFUNCTION_CALL, eIS_EMPTY, eSET_EMPTY, ePRINT // 33..37
} OperationType;

typedef struct tagExpression {
  OperationType type;

  int lValue;
  double dValue;
  ll_t list;
  Dictionary *object;
  char *str;

  struct tagExpression *left;
  struct tagExpression *right;
  struct tagExpression *next;

  ll_t upHandle;
  ll_t fceListHandle;
} Expression;

Expression *expression_create_integer(int value);
Expression *expression_create_double(double value);
Expression *expression_create_bool(int value);
Expression *expression_create_string(char *value);
Expression *expression_create_list();
Expression *expression_create_object();

Expression *expression_create_binary_operation(OperationType type, Expression *left, Expression *right);
Expression *expression_create_dot_operation(Expression *left, char *name);
Expression *expression_create_variable_assign(char *name, Expression *value);
Expression *expression_create_variable(char *name);
Expression *expression_create_new_block(Expression *block);
Expression *expression_create_if(Expression *query, Expression *blockTrue, Expression *blockFalse);
Expression *expression_create_while(Expression *query, Expression *block);
Expression *expression_create_break();
Expression *expression_create_continue();
Expression *expression_create_return(Expression *value);
Expression *expression_create_print(Expression *expression);
Expression *expression_create_function(char * name, Expression *value);
Expression *expression_create_function_call(char * name, Expression *value);
Expression *expression_create_variable_empty(char * name);
Expression *expression_create_is_empty(char * name);

Expression *expression_delist(Expression *item);

void expression_delete(Expression *b);

Expression *expression_add_list_item(Expression *lst, Expression *value);
Expression *expression_add_object_item(Expression *object, char *name, Expression *value);
Expression *expression_get_object_item(Expression *object, char *name, Expression *value);

#if defined(__cplusplus)
}
#endif  // __cplusplus

#endif