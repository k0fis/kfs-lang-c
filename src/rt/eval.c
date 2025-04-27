#include "eval.h"

Value *eval_value(Expression *e) {
  Value *lv, *rv, *result;

  switch (e->type) {
    case eIntVALUE:
      return value_new_int(e->lValue);
    case eDoubleVALUE:
      return value_new_double(e->dValue);
    case eBoolVALUE:
      return value_new_bool(e->lValue);
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
    case eLT:
      result = value_lt((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eLE:
      result = value_le((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eEQ:
      result = value_eq((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eNE:
      result = value_ne((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eGT:
      result = value_gt((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eGE:
      result = value_ge((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eAND:
      result = value_and((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eOR:
      result = value_or((lv = eval_value(e->left)), (rv = eval_value(e->right)));
      value_delete(lv); value_delete(rv);
      return result;
    case eNOT:
      result = value_not((lv = eval_value(e->left)));
      value_delete(lv);
      return result;

  }
  return NULL;
}