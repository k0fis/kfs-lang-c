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

    request_cleanup();


    options_delete(options);
    return res;
}
