#include "rt/utils.h"
#include "value.h"
#include "json.h"

#include "rt/options.h"

int test_json(char *json) {

    Value *output;
    int result = json_read_string(json, &output);
    if (result != 0) {
      return result;
    }
    if (output == NULL) {
        KFS_ERROR("Cannot parse code %s, result is NULL", json);
        return -5;
    }

    char *info = value_to_string(output, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
    KFS_INFO2("json: %s", info);
    free(info);

    value_delete(output);
    return 0;
}

void main_test_json() {
    test_json("{}");
    test_json("{ \"a\" : 1  }");
    test_json("{ \"b\" : 1.9  }");
    test_json("{ \"c\" : \"aaa\"  }");
    test_json("{ \"d\" : \"aaa\", \"e\":12, \"f\":3.41  }");
    test_json("{ \"d\" : \"aaa\", \"e\":12, \"f\":3.41, \"g\": [[[1, 2]], 1]  }");
}

void main(int argc, char *argv[]) {
    Options *options;
    options_create(&options);

    options_fulfill(options, argc, argv);

    options_delete(options);
}