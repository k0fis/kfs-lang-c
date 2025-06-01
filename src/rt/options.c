#include "options.h"
#include "utils.h"
#include <libgen.h>
#include <getopt.h>

#define MAX_FILE_LENGTH__DEFAULT 10240
#define OPTIONS_STRING_MAX_LEN    8192

#define USAGE "[-i] [-v] [-r] [-h] [-d] [-l max_file_read_length] [-e env_file] [-f script_file] [-s script]\n"\
    "\t\t-r,--version\tprint version info\n" \
    "\t\t-v,--verbose\tset verbose flag\n" \
    "\t\t-i,--stdin\t\tread script code form standard input\n" \
    "\t\t-h,--help\tprint usage\n" \
    "\t\t-l,--read-max-file-length"\
    "\t\t-d,--dump\tdump environment\n" \
    "\t\t-e,--env\tread and set env variables from file\n" \
    "\t\t-f,--file\tread code from file\n" \
    "\t\t-s,--script\tscript code\n"

static struct option long_options[] =
  {
    {"verbose", no_argument,       0, 'v'},
    {"version", no_argument,       0, 'r'},
    {"help",    no_argument,       0, 'h'},
    {"dump",    no_argument,       0, 'd'},
    {"script",  required_argument, 0, 's'},
    {"file",    required_argument, 0, 'f'},
    {"env",     required_argument, 0, 'e'},
    {"stdin",   no_argument,       0, 'i'},
    {"read-max-file-length", required_argument, 0, 'l'},
    {0, 0, 0, 0}
  };

#define SHORT_OPTS "vrhds:f:e:i"

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
    options->printVersion = FALSE;
    options->verbose = FALSE;
    options->dumpEnv = FALSE;
    options->maxReadFileLength = MAX_FILE_LENGTH__DEFAULT;
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

int options_fulfill(Options *options, const int argv, char **argc) {
    while (TRUE) {
      int option_index = 0;
      int c = getopt_long (argv, argc, SHORT_OPTS, long_options, &option_index);
      /* Detect the end of the options. */
      if (c == -1) break;
      switch (c) {
        case 0:
          if (long_options[option_index].flag != 0) break;
          KFS_INFO2("option %s", long_options[option_index].name);
          if (optarg) KFS_INFO2(" with arg %s", optarg);
          break;
        case 'v': options->verbose = TRUE; break;
        case 'r': options->printVersion = TRUE; break;
        case 'i': options_scripts_add(options, NULL, STR_LIST_MODE_STDIN);; break;
        case 'd': options->dumpEnv = TRUE; break;
        case 'e': options_envs_add(options, optarg); break;
        case 'f': options_scripts_add(options, optarg, STR_LIST_MODE_FILE); break;
        case 's': options_scripts_add(options, optarg, STR_LIST_MODE_SCRIPT); break;
        case 'l': options->maxReadFileLength = strtol(optarg, NULL, 10); break;
        case '?':
        case 'h': fprintf(stderr, "Usage: %s %s\n", basename(argc[0]), USAGE); break;
        default: KFS_ERROR("Unknown option '%c'", c); break;
      }
    }

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
    if (options->printVersion) {
        strncat(ret, " -r", len);
    }
    if (options->dumpEnv) {
        strncat(ret, " -d", len);
    }
    if (options->verbose) {
        strncat(ret, " -v", len);
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