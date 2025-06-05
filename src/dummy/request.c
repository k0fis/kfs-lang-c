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

#define NI KFS_ERROR("Not implemented", NULL); return RET_NOT_IMPLEMENTED;
int request_new(Request **r, Options *o)   { NI }
int request_delete(Request *req) { NI }
int request_post(Request *req)   { NI }
int request_get(Request *req)    { NI }