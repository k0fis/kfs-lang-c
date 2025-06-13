#ifndef OPTIONS_H
#define OPTIONS_H

#include "ll/ll.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define STR_LIST_MODE_NORMAL 0
#define STR_LIST_MODE_STDIN  1
#define STR_LIST_MODE_FILE   2
#define STR_LIST_MODE_SCRIPT 3

typedef struct tag_StrList {
    char *str;
    int mode;
    ll_t handle;
} StrList;

typedef struct tag_Options {
    int version;
    int verbose;
    int dump;
    int sslSkipPeerVerification;
    int sslSkipHostnameVerification;
    int requestVerbose;

    long maxReadFileLength;
    long threadStackSize;

    ll_t scripts;
    ll_t envs;
} Options;

int str_list_create(const char *str, StrList **lst, int mode);
int str_list_delete(StrList *lst);


int options_create(Options **options);
int options_delete(Options *options);
int options_envs_add(Options *options, const char *str);
int options_scripts_add(Options *options, const char *str, int mode);
int options_code_add(Options *options, const char *str);

int options_fulfill(Options *options, int argv, char **argc);

int options_to_string(Options *options, char **result);


#if defined(__cplusplus)
}
#endif

#endif
