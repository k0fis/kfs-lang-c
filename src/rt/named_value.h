#ifndef _named_vlue_h_
#define _named_vlue_h_

#include "value.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct tagNamedValue {
    char *name;
    Value *value;
} NamedValue;

NamedValue *named_value_new(char *name, Value *value);
void named_value_delete(NamedValue *nv);

void _named_value_print(NamedValue *value, char *postfix);
void named_value_print(NamedValue *value);

struct hashmap *named_value_create_hashmap();
Value *named_value_get(struct hashmap *map, char *name);
Value *named_value_set(struct hashmap *map, char *name, Value *newVal);

#if defined(__cplusplus)
}
#endif

#endif