
#include "kfs_lang_env.h"
#include "utils.h"
#include "options.h"
#include "parser.h"
#include "lexer.h"
#include "env.h"
#include "version.h"


int eval_result(KfsLangEnv *env) {
    Value *result = kfs_lang_eval_value(env, env->expression, KLE_EVAL_NORMAL);
    if (result->type == FC_Return) {
        result->type = List; // remove hack
    }
    result = value_delist(result);

    char *valStr = value_to_string(result, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
    printf("%s\n", valStr);
    free(valStr);

    value_delete(result);
    return RET_OK;
}

int main(int argc, char *argv[]) {
    yyscan_t scanner;
    int res = RET_OK;
    if (yylex_init(&scanner)) {
        KFS_ERROR("Cannot init yylex", NULL);
        return RET_CANNOT_INIT_LEX_ERROR;
    }

    Options *options; options_create(&options);
    res = options_fulfill(options, argc, argv);
    if (res != RET_OK) {
        yylex_destroy(scanner);
        options_delete(options);
        return res;
    }

    if (options->printVersion) {
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

    if (options->scripts.next != &options->scripts) {
        list_for_each_entry_safe(inx, tmp, &options->scripts, handle) {
            if (options->verbose) {
                printf("run file: %s\n", inx->str);
            }
            FILE *file = fopen(inx->str, "r");
            if (file == NULL) {
                KFS_ERROR("Cannot open file %s\n", inx->str);
                continue;
            }
            yyset_in(file, scanner);
            if (yyparse(kfsLangEnv, scanner, options)) {
                KFS_ERROR("Cannot parse ENV %s", inx->str);
            } else {
                eval_result(kfsLangEnv);
            }
            fclose(file);
        }
    } else {
        if (yyparse(kfsLangEnv, scanner, options)) {
            KFS_ERROR("Cannot parse code", NULL);
            res = RET_CANNOT_PARSE_CODE_ERROR;
        } else {
            eval_result(kfsLangEnv);
        }
    }
    yylex_destroy(scanner);
    kfs_lang_env_delete(kfsLangEnv);
    options_delete(options);
    return res;
}