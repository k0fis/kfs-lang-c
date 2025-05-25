
#include "kfs_lang_env.h"
#include "utils.h"
#include "parser.h"
#include "lexer.h"
#include "env.h"
#include "version.h"
#include "rt/options.h"

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

    // cycle scripts

    if (yyparse(kfsLangEnv, scanner)) {
        KFS_ERROR("Cannot parse code", NULL);
        res = RET_CANNOT_PARSE_CODE_ERROR;
    } else {
        Value *result = kfs_lang_eval_value(kfsLangEnv, kfsLangEnv->expression, KLE_EVAL_NORMAL);
        if (result->type == FC_Return) {
            result->type = List; // remove hack
        }
        result = value_delist(result);

        char *valStr = value_to_string(result, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
        printf("%s\n", valStr);
        free(valStr);

        value_delete(result);
    }
    yylex_destroy(scanner);
    kfs_lang_env_delete(kfsLangEnv);
    options_delete(options);
    return res;
}