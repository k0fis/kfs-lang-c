//
// Created by Pavel Dřímalka on 25.05.2025.
//

#include "env.h"
#include "options.h"
#include "env_parser.h"
#include "env_lexer.h"
#include "utils.h"
#include <stdlib.h>
#include <sys/errno.h>
#include <ctype.h>

int env_load_file(char *filename, Options *options) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        KFS_ERROR("Cannot open file %s\n", filename);
        return RET_ENV_CANNOT_OPEN_FILE;
    }
    envscan_t scanner;
    if (envlex_init(&scanner)) {
        KFS_ERROR("Cannot init envlex for %s", filename);
        fclose(file);
        return RET_ENV_CANNOT_INIT_SCANNER;
    }
    envset_in(file, scanner);
    if (envparse(scanner, options)) {
        KFS_ERROR("Cannot parse ENV %s", filename);
        fclose(file);
        return RET_ENV_CANNOT_PARSE_ENV;
    }
    fclose(file);
    envlex_destroy(scanner);
    return RET_OK;
}

int env_set_env(char *key, char *value, Options *options) {
    char * p = value;
    unsigned long l = strlen(p);
    if (l > 0) {
        while(isspace(p[l - 1])) p[--l] = 0;
        while(* p && isspace(* p)) ++p, --l;
        memmove(value, p, l + 1);
    }
    if (l > 0) {
        if (options->verbose) {
            printf("set env: '%s' to '%s'\n", key, value);
        }
        int ret = setenv(key, value, 1);
        if (ret) {
            switch (errno) {
                case EINVAL:
                    KFS_ERROR("key %s is not set, its NULL", key);
                    return RET_ENV_NULL_KEY;
                case ENOENT:
                    KFS_ERROR("key %s is not set, Insufficient memory to add a new variable to the environment", key);
                    return RET_ENV_MEMORY_ERROR;
            }
            KFS_ERROR("Cannot set %s, unknown error %i/%i", key, ret, errno);
            return RET_ENV_UNKNOWN_ERROR;
        }
    } else {
        if (options->verbose) {
            printf("unset env: '%s'\n", key);
        }
        int ret = unsetenv(key);
        if (ret) {
            switch (errno) {
                case EINVAL:
                    KFS_ERROR("key %s is not unset, its NULL", key);
                    return RET_ENV_NULL_KEY;
            }
            KFS_ERROR("Cannot unset %s, unknown error %i/%i", key, ret, errno);
            return RET_ENV_UNKNOWN_ERROR;
        }
    }
    return RET_OK;
}
