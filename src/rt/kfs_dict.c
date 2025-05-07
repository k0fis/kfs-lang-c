#include "kfs_dict.h"
#include "utils.h"


DictItem *dict_item_new(char *name, void *data) {
  KFS_MALLOC(DictItem, item);
  item->name = name;
  item->data = data;
  KFS_LST_INIT(item->lst);
  return item;
}

Dictionary *dict_new(element_free elfree) {
  KFS_MALLOC(Dictionary, dict);
  dict->elfree = elfree;
  KFS_LST_INIT(dict->lst);
  return dict;
}

void dict_delete(Dictionary *dict) {
  if (dict != NULL) {
    DictItem *inx, *tmp; list_for_each_entry_safe(inx, tmp, &dict->lst, lst) {
      list_del(&inx->lst);
      free(inx->name);
      dict->elfree(inx->data);
      free(inx);
    }
    free(dict);
  }
}

int dict_set(Dictionary *dict, char *name, void *data, int mode) {
  DictItem *inx; list_for_each_entry(inx, &dict->lst, lst) {
    if (!strcmp(inx->name, name)) {
      dict->elfree(inx->data);
      inx->data = data;
      free(name);
      return KFS_DICT_SET_RET_OK;
    }
  }
  if (mode & KFS_DICT_SET_DO_NOT_CREATE_NEW) {
    return KFS_DICT_SET_RET_NOT_SET;
  }
  list_add_tail(&dict_item_new(name, data)->lst, &dict->lst);
  return KFS_DICT_SET_RET_OK;
}

void *dict_get(Dictionary *dict, char *name) {
  DictItem *inx; list_for_each_entry(inx, &dict->lst, lst) {
    if (!strcmp(inx->name, name)) {
      return inx->data;
    }
  }
  return NULL;
}

int dict_count(Dictionary *dict) {
  int count = 0;
  DictItem *inx; list_for_each_entry(inx, &dict->lst, lst) count++;
  return count;
}
