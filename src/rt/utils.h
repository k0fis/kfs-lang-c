#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ll/ll.h"


#define FALSE (0)
#define TRUE (!FALSE)

#define KFS_INFO(str) fprintf(stdout, "\x1b[30;106mINFO\x1b[0m %s \x1b[0;35m in %s:%i\x1b[0m.\n", str, __FILE__, __LINE__);
#define KFS_INFO2(fmt,...) { fprintf(stdout, "\x1b[30;106mINFO\x1b[0m "); fprintf(stdout, fmt, __VA_ARGS__); fprintf(stdout, " \x1b[0;35min %s:%i\x1b[0m\n", __FILE__, __LINE__); } ;
#define KFS_ERROR(fmt, ...) { fprintf(stdout, "\x1b[41;30mERROR:\x1b[0m");  fprintf(stdout, fmt, __VA_ARGS__); fprintf(stdout, " \x1b[0;35min %s:%i\x1b[0m\n", __FILE__, __LINE__); } ;
#define KFS_ERROR_2(fmt, ...) { fprintf(stderr, fmt, __VA_ARGS__); fprintf(stderr, "in %s:%i\n", __FILE__, __LINE__); };

#define KFS_MALLOC(type, ptr) \
  type * ptr; \
  if (NULL == (ptr = malloc(sizeof(type)))) { \
    KFS_ERROR("Failed to allocate memory (%i)", (int)sizeof(ptr)); \
    return NULL; \
  }\

#define KFS_MALLOC_CHAR(ptr, len) \
  char * ptr; \
  if (NULL == (ptr = malloc(sizeof(char)*(int)len))) { \
    KFS_ERROR("Failed to allocate memory for char(%i)", (int)len); \
  } else { \
    ptr[0] = '\0';\
  } \

#define KFS_LST_INIT(name) \
  name.next = &name; \
  name.prev = &name

#ifdef DEBUG
  #define KFS_DEBUG(fmt, ...)  {fprintf(stdout, "\x1b[43;30mDEBUG:\x1b[0m"); fprintf(stdout, fmt, __VA_ARGS__); fprintf(stdout, " \x1b[0;35min %s:%i\x1b[0m\n", __FILE__, __LINE__); }
  #define KFS_DEBUG_ML(fmt, ...)  {fprintf(stdout, "\x1b[43;30mDEBUG:\x1b[0m\n"); fprintf(stdout, fmt, __VA_ARGS__); fprintf(stdout, "\n\x1b[43;30mDEBUG info done:\x1b[0m  \x1b[0;35min %s:%i\x1b[0m\n", __FILE__, __LINE__); }
#else
  #define KFS_DEBUG(fmt, ...)
  #define KFS_DEBUG_ML(fmt, ...)
#endif

#ifdef TRACE
  #define KFS_TRACE(fmt, ...)  {fprintf(stdout, "\x1b[45;30mTRACE:\x1b[0m"); fprintf(stdout, fmt, __VA_ARGS__); fprintf(stdout, " \x1b[0;35min %s:%i\x1b[0m\n", __FILE__, __LINE__); }
#else
  #define KFS_TRACE(fmt, ...)
#endif


#endif