#include "json.h"
#include "utils.h"
#include "json_parser.h"
#include "json_lexer.h"

int json_read_string(char *jsonStr, Value **output, Options *options) {
    zzscan_t scanner;
    YY_BUFFER_STATE state;

    if (zzlex_init(&scanner)) {
        KFS_ERROR("Cannot init zzlex", NULL);
        return -1;
    }

    state = zz_scan_string(jsonStr, scanner);
    if (zzparse(output, scanner, options)) {
        KFS_ERROR("Cannot parse json %s", jsonStr);
        return -2;
    }

    zz_delete_buffer(state, scanner);
    zzlex_destroy(scanner);

    if (output == NULL) {
        KFS_ERROR("Cannot parse json %s, result is NULL", jsonStr);
        return -3;
    }
    return 0;
}