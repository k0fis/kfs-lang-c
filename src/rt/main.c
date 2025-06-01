
#include "kfs_lang_env.h"
#include "utils.h"
#include "options.h"
#include "parser.h"
#include "lexer.h"
#include "env.h"
#include "version.h"


int eval_result(KfsLangEnv *env, Options *options) {
    Value *result = kfs_lang_eval_value(env, env->expression, KLE_EVAL_NORMAL, options);
    if (result != NULL) {
        int isReturn = FALSE;
        if (result->type == FC_Return) {
            result->type = List; // remove hack
            isReturn = TRUE;
        }
        result = value_delist(result);

        if (options->verbose || isReturn) {
            char *valStr = value_to_string(result, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
            printf("%s\n", valStr);
            free(valStr);
        }

        value_delete(result);
    }
    return RET_OK;
}

int main(int argc, char *argv[]) {
    int res = RET_OK;

    Options *options; options_create(&options);
    res = options_fulfill(options, argc, argv);
    if (res != RET_OK) {
        options_delete(options);
        return res;
    }
    if (options->verbose) {
        char *optString;
        if ((res = options_to_string(options, &optString)) != RET_OK) {
            KFS_ERROR("Cannot convert options into string!", NULL);
        } else {
            printf("arguments: %s\n", optString);
        }
        if (optString != NULL) {
            free(optString);
        }
    }
    if (options->printVersion || options->verbose) {
        printf("kfs-lang version %s\n", VERSION);
    }
    StrList *inx, *tmp; list_for_each_entry_safe(inx, tmp, &options->envs, handle) {
        if (options->verbose) {
            printf("load env file: %s\n", inx->str);
        }
        env_load_file(inx->str, options);
        list_del(&inx->handle);
        str_list_delete(inx);
    }
    KfsLangEnv *kfsLangEnv = kfs_lang_env_new();

    list_for_each_entry_safe(inx, tmp, &options->scripts, handle) {
        yyscan_t scanner;
        if (yylex_init(&scanner)) {
            KFS_ERROR("Cannot init yylex", NULL);
            return RET_CANNOT_INIT_LEX_ERROR;
        }
        if (inx->mode == STR_LIST_MODE_SCRIPT) {
            if (options->verbose) {
                printf("run code: %s\n", inx->str);
            }
            YY_BUFFER_STATE state = yy_scan_string(inx->str, scanner);
            if (yyparse(kfsLangEnv, scanner, options)) {
                KFS_ERROR("Cannot parse script: %s", inx->str);
            } else {
                res = eval_result(kfsLangEnv, options);
            }
            yy_delete_buffer(state, scanner);
        } else if (inx->mode == STR_LIST_MODE_FILE) {
            if (options->verbose) {
                printf("run file: %s\n", inx->str);
            }
            FILE *file = fopen(inx->str, "r");
            if (file == NULL) {
                KFS_ERROR("Cannot open file %s\n", inx->str);
            } else {
                yyset_in(file, scanner);
                if (yyparse(kfsLangEnv, scanner, options)) {
                    KFS_ERROR("Cannot parse script file %s", inx->str);
                } else {
                    res = eval_result(kfsLangEnv, options);
                }
                fclose(file);
            }
        } if (inx->mode == STR_LIST_MODE_STDIN) {
            if (options->verbose) {
                KFS_INFO("read code from stdin");
            }
            yyset_in(stdin, scanner);
            if (yyparse(kfsLangEnv, scanner, options)) {
                KFS_ERROR("Cannot parse code from stdin", NULL);
            } else {
                res = eval_result(kfsLangEnv, options);
            }
        }
        yylex_destroy(scanner);
        list_del(&inx->handle);
        str_list_delete(inx);
    }
    if (options->dumpEnv) {
        char * dump = kfs_lang_vars_to_string(kfsLangEnv, KLVTS_ALL_SPACES);
        printf("dump: %s\n\n", dump);
        free(dump);
    }
    kfs_lang_env_delete(kfsLangEnv);
    options_delete(options);
    return res;
}