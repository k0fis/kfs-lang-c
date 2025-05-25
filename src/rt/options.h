#ifndef OPTIONS_H
#define OPTIONS_H

#include "ll/ll.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct tag_StrList {
    char *str;
    ll_t handle;
} StrList;

typedef struct tag_Options {
    int printVersion;
    int verbose;

    ll_t scripts;
    ll_t envs;
} Options;

int str_list_create(const char *str, StrList **lst);
int str_list_delete(StrList *lst);


int options_create(Options **options);
int options_delete(Options *options);
int options_envs_sdd(Options *options, const char *str);
int options_scripts_sdd(Options *options, const char *str);

int options_fulfill(Options *options, int argv, char **argc);



#if defined(__cplusplus)
}
#endif

#endif
