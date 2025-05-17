
#include "kfs_lang_env.h"
#include "utils.h"
#include "parser.h"
#include "lexer.h"

int main(int argc, char *argv[]) {
    yyscan_t scanner;
    int res = 0;
    if (yylex_init(&scanner)) {
        KFS_ERROR("Cannot init yylex", NULL);
        return -101;
    }
    KfsLangEnv *kfsLangEnv = kfs_lang_env_new();

    if (yyparse(kfsLangEnv, scanner)) {
        KFS_ERROR("Cannot parse code", NULL);
        res = -102;
    } else {
        Value *result = kfs_lang_eval_value(kfsLangEnv, kfsLangEnv->expression, KLE_EVAL_NORMAL);
        if (result->type == FC_Return) {
            result->type = List; // remove hack
        }
        result = value_delist(result);

        char *valStr = value_to_string(result, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
        KFS_INFO(valStr);
        free(valStr);

        value_delete(result);
    }
    yylex_destroy(scanner);

    kfs_lang_env_delete(kfsLangEnv);
    return res;
}