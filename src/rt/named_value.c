#include "named_value.h"

uint64_t named_value_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    NamedValue *nv = (NamedValue *)item;
    return hashmap_sip(nv->name, strlen(nv->name), seed0, seed1);
}

int named_value_compare(const void *a, const void *b, void *udata) {
    NamedValue *nva = (NamedValue *)a;
    NamedValue *nvb = (NamedValue *)b;
    return strcmp(nva->name, nvb->name);
}

void named_value_delete_from_hash(void *nvv) {
    NamedValue *nv = (NamedValue*)nvv;
    if (nv->name != NULL) {
      free(nv->name);
    }
    if (nv->value != NULL) {
      value_delete(nv->value);
    }
}

struct hashmap *named_value_create_hashmap() {
  return hashmap_new(sizeof(NamedValue), 12, 3, 7, named_value_hash,
               named_value_compare, named_value_delete_from_hash, NULL);
}

Value *named_value_get(struct hashmap *map, char *name) {
  NamedValue * nv = ((NamedValue *)hashmap_get(map, &(NamedValue){.name = name}));
  if (nv == NULL) {
    return NULL;
  }
  return nv->value;
}

Value *named_value_set(struct hashmap *map, char *name, Value *newVal) {
  NamedValue * nv = ((NamedValue *)hashmap_get(map, &(NamedValue){.name = name}));
    if (nv == NULL) {
      hashmap_set(map, named_value_new(name, newVal));
    } else {
      value_delete(nv->value);
      nv->value = newVal;
    }
    return newVal;
}

NamedValue *named_value_new(char *name, Value *value) {
  KFS_MALLOC(NamedValue, nValue);
  nValue->name = strdup(name);
  nValue->value = value;
  return nValue;
}

void named_value_delete(NamedValue *nv) {
    named_value_delete_from_hash(nv);
    free(nv);
}

void _named_value_print(NamedValue *value, char *postfix) {
  KFS_MALLOC_CHAR(name, 3+strlen(value->name) );
  strcat(name, value->name);
  strcat(name, ": ");
  _value_print(value->value, name, postfix);
  free(name);
}
void named_value_print(NamedValue *value) {
  _named_value_print(value, "\n");
}
