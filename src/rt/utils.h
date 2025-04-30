#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ll/ll.h"
#include "hashmap/hashmap.h"
#include "expression.h"
#include "value.h"


#define FALSE (0)
#define TRUE (!FALSE)

#define KFS_INFO(str) fprintf(stdout, "INFO %s \x1b[0;35m in %s:%i\x1b[0m.\n", str, __FILE__, __LINE__);
#define KFS_ERROR(str) fprintf(stderr, "%s in %s:%i.\n" , str, __FILE__, __LINE__);

#define KFS_MALLOC(type, ptr) \
  type * ptr; \
  if (NULL == (ptr = malloc(sizeof(type)))) { \
    KFS_ERROR("Failed to allocate memory"); \
    return NULL; \
  }\

#define KFS_MALLOC_CHAR(ptr, len) \
  char * ptr; \
  if (NULL == (ptr = malloc(sizeof(char)*len))) { \
    KFS_ERROR("Failed to allocate memory for char"); \
  } else { \
    ptr[0] = '\0';\
  } \

#define KFS_LST_INIT(name) \
  name.next = &name; \
  name.prev = &name

#endif