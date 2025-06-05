//
// Created by Pavel Dřímalka on 03.06.2025.
//

#include <curl/curl.h>
#include <stddef.h>
#include <stdio.h>
#include "request.h"

#include "options.h"
#include "utils.h"

int request_init() {
    curl_global_init(CURL_GLOBAL_ALL);
    return RET_OK;
}
int request_cleanup() {
    return RET_OK;
}

typedef struct tag_request {
   CURL *curl;
} Request;

#define CURL_CALL(var, res, msg) if ((var = res) != CURLE_OK) { KFS_ERROR("CURL error %s: %s", msg, curl_easy_strerror(var));}

int request_new(Request **req, Options *options) {
    KFS_MALLOC2(Request, r);
    r->curl = curl_easy_init();
    CURLcode curlCode;
    if (r->curl == NULL) {
        KFS_ERROR("Cannot init CURL!", NULL);
    }
    if (options->sslSkipPeerVerification) {
        CURL_CALL(curlCode, curl_easy_setopt(r->curl, CURLOPT_SSL_VERIFYPEER, 0L), "curl_easy_setopt(CURLOPT_SSL_VERIFYPEER)")
    }
    if (options->sslSkipHostnameVerification) {
        CURL_CALL(curlCode, curl_easy_setopt(r->curl, CURLOPT_SSL_VERIFYHOST, 0L), "curl_easy_setopt(CURLOPT_SSL_VERIFYHOST)")
    }

    *req = r;
    return RET_OK;
}

int request_delete(Request *req) {
    curl_easy_cleanup(req->curl);
    free(req);
    return RET_OK;
}

int request_post(Request *req) {
    return RET_NOT_IMPLEMENTED;
}

int request_get(Request *req) {
    return RET_NOT_IMPLEMENTED;
}