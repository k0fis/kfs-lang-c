#include "options.h"
#include "rt/utils.h"
#include <libgen.h>
#include <getopt.h>

int str_list_create(const char *str, StrList **lst) {
    KFS_MALLOC2(StrList, strList);
    if (strList == NULL) {
        return RET_ALLOC_ERROR;
    }
    KFS_LST_INIT(strList->handle);
    if (str != NULL) {
        strList->str = strdup(str);
    } else {
        strList->str = NULL;
    }
    *lst = strList;
    return RET_OK;
}

int str_list_delete(StrList *lst) {
    if (lst->str != NULL) {
        free(lst->str);
    }
    free(lst);
    return RET_OK;
}

int options_create(Options **opts) {
    KFS_MALLOC2(Options, options);
    if (options == NULL) {
        return RET_ALLOC_ERROR;
    }
    KFS_LST_INIT(options->scripts);
    options->printVersion = FALSE;
    *opts = options;
    return RET_OK;
}

int options_delete(Options *opts) {
    StrList *inx, *tmp; list_for_each_entry_safe(inx, tmp, &opts->scripts, handle) {
        list_del(&inx->handle);
        str_list_delete(inx);
    }
    free(opts);
    return RET_OK;
}

int options_scripts_sdd(Options *options, const char *str) {
    StrList *lst;
    if (str_list_create(str, &lst) != RET_OK) {
        KFS_ERROR("Cannot add string into list", NULL);
        return RET_ALLOC_ERROR;
    }
    list_add_tail(&lst->handle,&options->scripts);
    return RET_OK;
}

int options_fulfill(Options *options, const int argv, char **argc) {
    int opt;
    while ((opt = getopt(argv, argc, "vs:")) != EOF) {
        switch(opt) {
            case 'v':
                KFS_INFO("Options version");
                options->printVersion = TRUE;
                break;
            case 's':
                KFS_INFO2("Options script %s", optarg);
                options_scripts_sdd(options, optarg);
                break;
            default: ;
                KFS_ERROR("Usage: %s [-v] [-s script]", basename(argc[0]));
                return RET_BAD_PARAMETERS_ERROR;
        }
    }
    return RET_OK;
}