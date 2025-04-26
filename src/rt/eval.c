#include "eval.h"

Value *eval_value(Expression *e) {
  Value *lv, *rv, *result;
  switch (e->type) {
    case eIntVALUE:
      return value_new_int(e->lValue);
    case eDoubleVALUE:
      return value_new_double(e->dValue);
    case eMULTIPLY:
      result = value_mul((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eADD:
      result = value_plus((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eMINUS:
      result = value_minus((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eDIVIDE:
      result = value_divide((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eMODULO:
      result = value_mod((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case ePOWER:
      result = value_power((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
  }
  return NULL;
}