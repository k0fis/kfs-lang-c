#include "rt/utils.h"
#include "value.h"
#include "json_parser.h"
#include "json_lexer.h"

int test_json(char *json) {
    zzscan_t scanner;
    YY_BUFFER_STATE state;

    if (zzlex_init(&scanner)) {
        KFS_ERROR("Cannot init zzlex", NULL);
        return -1;
    }

    state = zz_scan_string(json, scanner);
    Value *output;
    if (zzparse(&output, scanner)) {
        KFS_ERROR("Cannot parse code %s", json);
        return -2;
    }

    zz_delete_buffer(state, scanner);
    zzlex_destroy(scanner);

    if (output == NULL) {
        KFS_ERROR("Cannot parse code %s, result is NULL", json);
        return -3;
    }
    char *info = value_to_string(output, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
    KFS_INFO2("json: %s", info);
    free(info);
    value_delete(output);
    return 0;
}

int main() {
    test_json("{}");
    test_json("{ \"a\" : 1  }");
    test_json("{ \"b\" : 1.9  }");
    test_json("{ \"c\" : \"aaa\"  }");
    test_json("{ \"d\" : \"aaa\", \"e\":12, \"f\":3.41  }");
}
