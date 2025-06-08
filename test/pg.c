#include "env.h"
#include "utils.h"
#include "json.h"
#include "request.h"

#include "rt/options.h"


int main(int argc, char *argv[]) {
    int res = RET_OK;
    Options *options;
    options_create(&options);
    options_fulfill(options, argc, argv);
    if ((res = request_init()) != RET_OK) {
        KFS_ERROR("Cannot initialize request", NULL);
        return res;
    }

    char * url = "https://kofis.eu";
    Request *req;
    request_new(&req, url, options);

    if ((res = request_get(req)) != RET_OK) {
        KFS_ERROR("Cannot get request %s", url);
        return res;
    }

    char *result;
    request_to_string(req, &result);
    KFS_INFO2("Request url: %s\n%s\n", url, result);
    free(result);

    Value *val;
    request_to_value(req, &val);
    request_cleanup();

    result = value_to_string(val, VALUE_TO_STRING_STR_WITH_APOSTROPHE);
    KFS_INFO2("Request url: %s\n%s\n", url, result);
    free(result);

    value_delete(val);

    options_delete(options);
    return res;
}
