#ifndef _KFS_DICT_H
#define _KFS_DICT_H

#include "ll/ll.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef void (*element_free)(void *element);

typedef struct tagDistionary {
  element_free elfree;
  ll_t lst;
} Dictionary;


typedef struct tagDictItem {
  char *name;
  void *data;

  ll_t lst;
} DictItem;

#define KFS_DICT_SET_NORMAL              0x0
#define KFS_DICT_SET_DO_NOT_CREATE_NEW   0x1

#define KFS_DICT_SET_RET_OK      0
#define KFS_DICT_SET_RET_NOT_SET 1
#define KFS_DICT_SET_RET_ERROR  -1

Dictionary *dict_new(element_free elfree);
void dict_delete(Dictionary *);

int dict_set(Dictionary *, char *name, void *data, int mode);
void *dict_get(Dictionary *, char *name);

int dict_count(Dictionary *dict);

#if defined(__cplusplus)
}
#endif

#endif //_KFS_DICT_H