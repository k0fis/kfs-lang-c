#include "eval.h"

#include "parser.h"
#include "lexer.h"
#include "expression.h"

int yyparse(Expression **expression, yyscan_t scanner);


Expression *eval_kfs_lang(char *code) {
  Expression *expression;
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if (yylex_init(&scanner)) {
    KFS_ERROR("Cannot init yylex");
    return NULL;
  }
  state = yy_scan_string(code, scanner);
  if (yyparse(&expression, scanner)) {
    KFS_ERROR("Cannot parse code");
    return NULL;
  }
  yy_delete_buffer(state, scanner);
  yylex_destroy(scanner);
  return expression;
}

Value *eval_value(Expression *e) {
  if (e == NULL) {
    return NULL;
  }
  Value *lv, *rv, *result;
  NamedExpression *ne;
  Expression *expr;
  int iny;

  switch (e->type) {
    case eIntVALUE:
      return value_new_int(e->lValue);
    case eDoubleVALUE:
      return value_new_double(e->dValue);
    case eBoolVALUE:
      return value_new_bool(e->lValue);
    case eListVALUE:
      lv = value_new_list();
      if (lv == NULL) {
        KFS_ERROR("Cannot obtain new LIST value");
        return NULL;
      }
      Expression *inx; list_for_each_entry(inx, &e->lst, lst) {
        value_list_add(lv, eval_value(inx));
      }
      return lv;
    case eObjectVALUE:
      lv = value_new_object();
      size_t iter = 0;
      void *item;
      while (hashmap_iter(e->object, &iter, &item)) {
        NamedExpression *ne = (NamedExpression*)item;
        value_object_add(lv, ne->name, eval_value(ne->expression));
      }
      return lv;
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
    case eUNARY_MINUS:
      result = eval_value(e->left);
      if (result->type == Int) {
        result->iValue = -1*result->iValue;
      } else if (result->type == Double) {
        result->dValue = -1*result->dValue;
      }
      return result;
    case eDOT:
      lv = eval_value(e->left);
      if (lv->type != Object) {
        KFS_ERROR("Object access to non-object value");
        value_delete(lv);
        return NULL;
      }
      result = value_object_map_get(lv->oValue, e->str);
      if (result == NULL) {
        KFS_ERROR("empty result");
      } else {
        result = value_copy(result);
        if (result == NULL) {
          KFS_ERROR("Cannot create value copy");
        }
      }
      value_delete(lv);
      return result;
    case eARRAY_ACCESS:
      rv = eval_value(e->right);
      if (rv->type != Int) {
        KFS_ERROR("Array access - index type must be INT");
        value_delete(rv);
        return NULL;
      }
      int position = rv->iValue;
      value_delete(rv);
      lv = eval_value(e->left);
      if (lv->type != List) {
        KFS_ERROR("Array access to non-array value");
        value_delete(lv);
        return NULL;
      }
      result = value_list_get(lv, position);
      if (result == NULL) {
        KFS_ERROR("empty value");
      } else {
        result = value_copy(result);
        if (result == NULL) {
          KFS_ERROR("Cannot create value copy");
        }
      }
      value_delete(lv);
      return result;
  }
  return NULL;
}