#ifndef __VALUE_H__
#define __VALUE_H__

#include "utils.h"

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

typedef enum tagValueType {
    Int,
    Double,
    Bool,
    String,
    List,
    Object
} ValueType;

typedef struct tagValue {
    ValueType type;

    int iValue; // Int && Bool -> 0 false, !0 true
    double dValue;
    char *sValue;
    ll_t lValue;
    struct hashmap *oValue;
} Value;

typedef struct tagNamedValue {
    char *name;
    Value *value;
} NamedValue;

Value *value_new_int(int iValue);
Value *value_new_double(double dValue);
Value *value_new_bool(int bValue);
Value *value_new_string(char *sValue);
Value *value_new_list();
Value *value_new_object();
void value_delete(Value *value);
Value *value_copy(Value *value);

NamedValue *named_value_new(char *name, Value *value);
void named_value_delete(NamedValue *nv);

int value_list_add(Value *list, Value *value);
Value *value_list_get(Value *list, int inx);

int value_object_add(Value *obj, char *name, Value *val);
Value *value_object_get(Value *obj, char *name);
Value *value_object_map_get(struct hashmap *map, char *name);

Value *value_plus(Value *left, Value *right);
Value *value_minus(Value *left, Value *right);
Value *value_mul(Value *left, Value *right);
Value *value_divide(Value *left, Value *right);
Value *value_mod(Value *left, Value *right);
Value *value_power(Value *left, Value *right);

Value *value_lt(Value *left, Value *right);
Value *value_le(Value *left, Value *right);
Value *value_eq(Value *left, Value *right);
Value *value_ne(Value *left, Value *right);
Value *value_gt(Value *left, Value *right);
Value *value_ge(Value *left, Value *right);
Value *value_and(Value *left, Value *right);
Value *value_or(Value *left, Value *right);
Value *value_not(Value *left);

void value_print(Value *value);
void named_value_print(NamedValue *value);
void _value_print(Value *value, char *prefix, char *postfix);

#if defined(__cplusplus)
}
#endif  // __cplusplus


#endif