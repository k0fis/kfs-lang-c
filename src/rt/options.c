#include "options.h"
#include "utils.h"
#include <libgen.h>
#include <getopt.h>

#define MAX_FILE_LENGTH__DEFAULT 10240
#define OPTIONS_STRING_MAX_LEN    8192

int str_list_create(const char *str, StrList **lst, int mode) {
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
    strList->mode = mode;
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
    KFS_LST_INIT(options->envs);
    options->version = FALSE;
    options->verbose = FALSE;
    options->dump = FALSE;
    options->maxReadFileLength = MAX_FILE_LENGTH__DEFAULT;
    options->sslSkipPeerVerification = TRUE;
    options->sslSkipHostnameVerification = TRUE;
    *opts = options;
    return RET_OK;
}

void options_list_delete(ll_t *lst) {
    StrList *inx, *tmp; list_for_each_entry_safe(inx, tmp, lst, handle) {
        list_del(&inx->handle);
        str_list_delete(inx);
    }
}

int options_delete(Options *opts) {
    options_list_delete(&opts->scripts);
    options_list_delete(&opts->envs);
    free(opts);
    return RET_OK;
}

int options_lst_add(ll_t *list, const char *str, int mode) {
    StrList *lst;
    if (str_list_create(str, &lst, mode) != RET_OK) {
        KFS_ERROR("Cannot add string into list", NULL);
        return RET_ALLOC_ERROR;
    }
    list_add_tail(&lst->handle,list);
    return RET_OK;
}

int options_scripts_add(Options *options, const char *str, int mode) {
    return options_lst_add(&options->scripts, str, mode);
}

int options_envs_add(Options *options, const char *str) {
    return options_lst_add(&options->envs, str, STR_LIST_MODE_NORMAL);
}

char *options_help_lines(struct option *long_options, char *descs[]) {
    KFS_MALLOC_CHAR(ret, 1);

    for (int inx = 0; long_options[inx].name; inx++) {
        unsigned long len = strlen(ret);
        ret = realloc(ret, 1+len+11+strlen(long_options[inx].name) + strlen(descs[inx]));
        strcat(ret, "\t\t");
        len = strlen(ret);
        if (long_options[inx].flag == NULL) {
            ret[len++] = '-';
            ret[len++] = (char)long_options[inx].val;
            ret[len++] = ',';
            ret[len++] = '\0';
        } else {
            strcat(ret, "   ");
        }
        strcat(ret,"--");
        strcat(ret, long_options[inx].name);
        len = strlen(ret);
        ret[len++] = '\t';
        ret[len++] = '\t';
        ret[len++] = '\0';
        strcat(ret, descs[inx]);
        len = strlen(ret);
        ret[len++] = '\n';
        ret[len++] = '\0';
    }

    return ret;
}

char *options_help_line(struct option *long_options) {
    KFS_MALLOC_CHAR(ret, 1);
    unsigned long llen = 0;
    for (int inx = 0; long_options[inx].name; inx++) {
        unsigned long len = strlen(ret);
        if (long_options[inx].flag == NULL) {
            if (long_options[inx].has_arg == no_argument) {
                llen += 1+len+5;
                ret = realloc(ret, 1+len+5);
                ret[len++] = '[';
                ret[len++] = '-';
                ret[len++] = (char)long_options[inx].val;
                ret[len++] = ']';
                ret[len++] = ' ';
                ret[len++] = '\0';
            } else {
                llen += 1+len+6+strlen(long_options[inx].name);
                ret = realloc(ret, 1+len+6+strlen(long_options[inx].name));
                ret[len++] = '[';
                ret[len++] = '-';
                ret[len++] = (char)long_options[inx].val;
                ret[len++] = ' ';
                strcat(ret, long_options[inx].name);
                len = strlen(ret);
                ret[len++] = ']';
                ret[len++] = ' ';
                ret[len++] = '\0';
            }
        } else {
            llen += 1+len+6+strlen(long_options[inx].name);
            ret = realloc(ret, 1+len+6+strlen(long_options[inx].name));
            strcat(ret, "[--");
            len = strlen(ret);
            strcat(ret, long_options[inx].name);
            len = strlen(ret);
            ret[len++] = ']';
            ret[len++] = ' ';
            ret[len++] = '\0';
        }
        if (llen > 390) {
            llen = 0;
            ret = realloc(ret, strlen(ret)+5);
            strcat(ret, "\\\n\t\t");
        }
    }
    return ret;
}
char *options_short_codes(struct option *long_options) {
    int inx = 0; int retPos = 0;
    KFS_MALLOC_CHAR(ret, 256);
    while (long_options[inx].name) {
        if (long_options[inx].flag == 0) {
            ret[retPos] = (char)long_options[inx].val; retPos++;
            if (long_options[inx].has_arg == required_argument) {
                ret[retPos] = ':'; retPos++;
            }
        }
        inx++;
    }
    return realloc(ret, retPos+1);
}

static char *optionsDef[] = {
    "set verbose option",
    "print version",
    "\tdump environment",
    "\tprint usage",
    "\tscript code",
    "\tread code from file",
    "\tread and set env variables from file",
    "\tread script code form standard input",
    "\t\tmax file length for reading",
    "\tskip peer verification",
    "\t\tdo ssl peer verification",
    "skip hostname verification",
    "\tdo hostname verification",
};

int options_fulfill(Options *options, const int argv, char **argc) {
    struct option long_options[] = {
        {"verbose", no_argument,       &options->verbose,      TRUE},
        {"version", no_argument,       &options->version, TRUE},
        {"dump",    no_argument,       &options->dump,      TRUE},
        {"help",    no_argument,       0, 'h'},
        {"script",  required_argument, 0, 's'},
        {"file",    required_argument, 0, 'f'},
        {"env",     required_argument, 0, 'e'},
        {"stdin",   no_argument,       0, 'i'},
        {"read_max_file_length", required_argument, 0, 'l'},
        {"ssl_skip_peer_verification",     no_argument, &options->sslSkipPeerVerification, TRUE},
        {"ssl_peer_verification",          no_argument, &options->sslSkipPeerVerification, FALSE},
        {"ssl_skip_hostname_verification", no_argument, &options->sslSkipHostnameVerification, TRUE},
        {"ssl_hostname_verification",      no_argument, &options->sslSkipHostnameVerification, FALSE},
        {0, 0, 0, 0}
    };

    char *stLine; char *lines;
    char *shorts = options_short_codes(long_options);
    while (TRUE) {
      int option_index = 0;
      int c = getopt_long (argv, argc, shorts, long_options, &option_index);
      if (c == -1) break;
      switch (c) {
        case 0:
          if (long_options[option_index].flag != 0) break;
          KFS_INFO2("option %s", long_options[option_index].name);
          if (optarg) KFS_INFO2(" with arg %s", optarg);
          break;
        case 'i': options_scripts_add(options, NULL, STR_LIST_MODE_STDIN);; break;
        case 'e': options_envs_add(options, optarg); break;
        case 'f': options_scripts_add(options, optarg, STR_LIST_MODE_FILE); break;
        case 's': options_scripts_add(options, optarg, STR_LIST_MODE_SCRIPT); break;
        case 'l': options->maxReadFileLength = strtol(optarg, NULL, 10); break;
        case '?':
        case 'h':
              stLine = options_help_line(long_options);
              lines = options_help_lines(long_options, optionsDef);
              fprintf(stderr, "Usage: %s %s\n\n%s\n", basename(argc[0]), stLine, lines);
              free(stLine);
              free(lines);
              break;
        default: KFS_ERROR("Unknown option '%c'", c); break;
      }
    }
    free(shorts);
    if (optind < argv) {
      printf ("non-option ARGV-elements: ");
      while (optind < argv)
        printf ("%s ", argc[optind++]);
      putchar ('\n');
    }
    return RET_OK;
}

int options_to_string(Options *options, char **result) {
    size_t len = OPTIONS_STRING_MAX_LEN;
    KFS_MALLOC_CHAR(ret, len);
    *result = ret;
    if (options->version) {
        strncat(ret, " --verbose", len);
    }
    if (options->dump) {
        strncat(ret, " --dump", len);
    }
    if (options->verbose) {
        strncat(ret, " --version", len);
    }
    if (options->maxReadFileLength != MAX_FILE_LENGTH__DEFAULT) {
        size_t len0 = strlen(ret);
        len -= len0;
        ret += len0;
        snprintf(ret, len, " -l %ld", options->maxReadFileLength);
    }

    StrList *inx; list_for_each_entry(inx, &options->envs, handle) {
        size_t len0 = strlen(ret);
        len -= len0;
        ret += len0;
        snprintf(ret, len, " -e %s", inx->str);
    }

    list_for_each_entry(inx, &options->scripts, handle) {
        size_t len0 = strlen(ret);
        len -= len0;
        ret += len0;
        switch (inx->mode) {
            case STR_LIST_MODE_NORMAL:
                KFS_ERROR("Unknown script mode %s", inx->str);
                break;
            case STR_LIST_MODE_STDIN:
                strncat(ret, " -i", len);
                break;
            case STR_LIST_MODE_FILE:
                snprintf(ret, len, " -f %s", inx->str);
                break;
            case STR_LIST_MODE_SCRIPT:
                snprintf(ret, len, " -s '%s'", inx->str);
                break;
        }
    }

    *result = realloc(*result, strlen(*result)+1);
    return RET_OK;
}