#include "env.h"
#include "rt/utils.h"
#include "value.h"
#include "json.h"
#include "rt/env.h"

#include "rt/options.h"

int test_json(char *json, Options *opts) {

    Value *output;
    int result = json_read_string(json, &output, opts);
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

int main(int argc, char *argv[]) {
    int res = RET_OK;
    Options *options;
    options_create(&options);
    options_fulfill(options, argc, argv);
    res += test_json("{}", options);
    res += test_json("{ \"a\" : 1  }", options);
    res += test_json("{ \"b\" : 1.9  }", options);
    res += test_json("{ \"c\" : \"aaa\"  }", options);
    res += test_json("{ \"d\" : \"aaa\", \"e\":12, \"f\":3.41  }", options);
    res += test_json("{ \"d\" : \"aaa\", \"e\":12, \"f\":3.41, \"g\": [[[1, 2]], 1]  }", options);
    options_delete(options);
    return res;
}
