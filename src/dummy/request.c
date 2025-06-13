//
// Created by Pavel Dřímalka on 03.06.2025.
//

#include "request.h"

#include "utils.h"

typedef struct tag_request {
} Request;
int request_init() {
    return RET_OK;
}
int request_cleanup() {
    return RET_OK;
}

int request_new(Request **r, char *url, const Options *o) {
    NI("request_new");
}
int request_delete(Request *req) {
    NI("request_delete");
}
int request_post(Request *req) {
    NI("request_post");
}
int request_get(Request *req) {
    NI("request_get");
}
int request_to_string(const Request *req, char **output) {
    NI("request_to_string");
}
int request_to_value(const Request *req, Value **output) {
    NI("request_to_value");
}