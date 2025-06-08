//
// Created by Pavel Dřímalka on 03.06.2025.
//

#include <curl/curl.h>
#include <curl/header.h>
#include <stddef.h>
#include <stdio.h>
#include "version.h"
#include "request.h"

#include "options.h"
#include "utils.h"

#define CURL_CALL(var, res, msg) if ((var = res) != CURLE_OK) { KFS_ERROR("CURL error on %s: %s", msg, curl_easy_strerror(var));}
#define CURL_CALL2(var, res, msg, msg2) if ((var = res) != CURLE_OK) { KFS_ERROR("CURL error on %s - %s: %s", msg, msg2, curl_easy_strerror(var));}

int request_init() {
    CURLcode curlCode;
    CURL_CALL(curlCode, curl_global_init(CURL_GLOBAL_ALL), "curl_global_init");
    if (curlCode != CURLE_OK) {
        return RET_REQUEST_CANNOT_INIT;
    }
    return RET_OK;
}
int request_cleanup() {
    curl_global_cleanup();
    return RET_OK;
}

typedef struct tag_request {
    CURL *curl;
    char *url;
    int secure;
    struct curl_slist *headers;

    long response_code;
    double elapsed;
    char *response;
    size_t response_size;
    char *content_type;
    long header_size;
    long header_len;
    char **header_names;
    char **header_values;
} Request;

static size_t request_mem_write(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    Request *mem = (Request *)userp;

    char *ptr = realloc(mem->response, mem->response_size + realsize + 1);
    if(ptr == NULL) {
        KFS_ERROR("not enough memory (realloc returned NULL)", NULL);
        return 0;
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->response_size]), contents, realsize);
    mem->response_size += realsize;
    mem->response[mem->response_size] = 0;

    // return the size of content that is copied.
    return realsize;
}

int request_new(Request **request, char *url, const Options *options) {
    KFS_MALLOC2(Request, req);
    req->curl = curl_easy_init();
    req->url = NULL;
    req->response = NULL;
    req->content_type = NULL;
    req->headers = NULL;
    req->response_size = 0;
    req->response_code = 0;
    req->header_size = 0;
    req->header_len = 0;
    req->elapsed = -1;
    req->header_names = NULL;
    req->header_values = NULL;
    req->secure = strncmp("https", url, 5) == 0;
    CURLcode curlCode;
    if (req->curl == NULL) {
        KFS_ERROR("Cannot init CURL!", NULL);
    }
    if (options->requestVerbose) {
        CURL_CALL(curlCode, curl_easy_setopt(req->curl, CURLOPT_VERBOSE, 1L), "curl_easy_setopt(CURLOPT_VERBOSE)")
    }
    CURL_CALL(curlCode, curl_easy_setopt(req->curl, CURLOPT_USERAGENT, "kfs-lang/" VERSION), "curl_easy_setopt(CURLOPT_USERAGENT)");
    if (req->secure) {
        if (options->sslSkipPeerVerification) {
            CURL_CALL(curlCode, curl_easy_setopt(req->curl, CURLOPT_SSL_VERIFYPEER, 0L), "curl_easy_setopt(CURLOPT_SSL_VERIFYPEER)")
        }
        if (options->sslSkipHostnameVerification) {
            CURL_CALL(curlCode, curl_easy_setopt(req->curl, CURLOPT_SSL_VERIFYHOST, 0L), "curl_easy_setopt(CURLOPT_SSL_VERIFYHOST)")
        }

        /* cache the CA cert bundle in memory for a week */
        CURL_CALL(curlCode, curl_easy_setopt(req->curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L), "curl_easy_setopt(CURLOPT_CA_CACHE_TIMEOUT)")
    }
    CURL_CALL(curlCode,curl_easy_setopt(req->curl, CURLOPT_URL, url), "curl_easy_setopt(curl_easy_setopt)")
    CURL_CALL(curlCode,curl_easy_setopt(req->curl, CURLOPT_WRITEFUNCTION, request_mem_write), "curl_easy_setopt(curl_easy_setopt)")
    CURL_CALL(curlCode,curl_easy_setopt(req->curl, CURLOPT_WRITEDATA, (void *)req), "curl_easy_setopt(curl_easy_setopt)")

    *request = req;
    return RET_OK;
}

int request_delete(Request *req) {
    if (req->header_names != NULL) {
        free(req->header_names);
    }
    if (req->header_values != NULL) {
        free(req->header_values);
    }
    curl_easy_cleanup(req->curl);
    if (req->headers != NULL) {
        curl_slist_free_all(req->headers);
    }
    free(req);
    return RET_OK;
}

int request_post_perform(Request *req) {
    CURLcode curlCode;
    CURL_CALL(curlCode,curl_easy_getinfo(req->curl, CURLINFO_RESPONSE_CODE, &req->response_code), "curl_easy_getinfo(CURLINFO_RESPONSE_CODE)")
    CURL_CALL(curlCode,curl_easy_getinfo(req->curl, CURLINFO_TOTAL_TIME, &req->elapsed), "curl_easy_getinfo(CURLINFO_TOTAL_TIME)");
    CURL_CALL(curlCode,curl_easy_getinfo(req->curl, CURLINFO_EFFECTIVE_URL, &req->url), "curl_easy_getinfo(CURLINFO_EFFECTIVE_URL)");
    CURL_CALL(curlCode,curl_easy_getinfo(req->curl, CURLINFO_CONTENT_TYPE, &req->content_type), "curl_easy_getinfo(CURLINFO_CONTENT_TYPE)");
    CURL_CALL(curlCode,curl_easy_getinfo(req->curl, CURLINFO_HEADER_SIZE, &req->header_len), "curl_easy_getinfo(CURLINFO_HEADER_SIZE)");

    // read headers
    struct curl_header *prev = NULL; struct curl_header *h = NULL;
    for (req->header_size = 0; (h = curl_easy_nextheader(req->curl, CURLH_HEADER, 0, prev));req->header_size++, prev = h) {
        if (req->header_names == NULL) {
            req->header_names = malloc(sizeof(char *));
            req->header_values = malloc(sizeof(char *));
        } else {
            req->header_names = realloc(req->header_names, sizeof(char *) * (1+req->header_size));
            req->header_values = realloc(req->header_values, sizeof(char *)* (1+req->header_size));
        }
        req->header_names[req->header_size] = h->name;
        req->header_values[req->header_size] = h->value;
    }
    return RET_OK;
}

int request_internal_do_request(Request *req, long curl_request_type, char *type_name ) {
    CURLcode curlCode;
    if (req->headers != NULL) {
        CURL_CALL(curlCode, curl_easy_setopt(req->curl, CURLOPT_HTTPHEADER, req->headers), "curl_easy_setopt(CURLOPT_HTTPHEADER)")
    }
    CURL_CALL2(curlCode, curl_easy_setopt(req->curl, curl_request_type, 1L), "curl_easy_setopt",  type_name)
    CURL_CALL(curlCode, curl_easy_perform(req->curl), "curl_easy_perform");
    if (curlCode != CURLE_OK) {
        KFS_ERROR("curl_easy_perform %s failed", type_name);
        return RET_REQUEST_GET_FAILED;
    }
    request_post_perform(req);
    return RET_OK;
}
int request_get(Request *req) {
    return request_internal_do_request(req, CURLOPT_HTTPGET, "CURLOPT_HTTPGET");
}

int request_post(Request *req) {
    return request_internal_do_request(req, CURLOPT_HTTPPOST, "CURLOPT_HTTPPOST");
}

int request_to_string(const Request *req, char **output) {
    char numberBuff[32], *ret2;
    KFS_MALLOC_CHAR(ret, 1l
        +  5 +strlen(req->url) + 1
        +  8+2
        + 15 /*+responseCode*/ + 32
        + 15 /*+headerLen*/ + 32
        + 15 /*+headerSize*/ + 32
        +  9 /*headers:\n*/
        + 15 /*Content Type*/
        +  9 /*+elapsed*/ + 32
        + 15 /*+responseSize*/ + 32
        );
    if (ret == NULL) {
        KFS_ERROR("Out of memory", NULL);
        return RET_ALLOC_ERROR;
    }
    strcat(ret, "url: ");
    strcat(ret, req->url);
    strcat(ret, "\n");

    strcat(ret, "secure: ");
    if (req->secure) {
       strcat(ret, "1\n");
    } else {
        strcat(ret, "0\n");
    }

    strcat(ret, "response code: ");
    sprintf(numberBuff, "%ld\n", req->response_code);
    strcat(ret, numberBuff);

    strcat(ret, "header length: ");
    sprintf(numberBuff, "%ld\n", req->header_len);
    strcat(ret, numberBuff);

    strcat(ret, "header count: ");
    sprintf(numberBuff, "%ld\n", req->header_size);
    strcat(ret, numberBuff);

    strcat(ret, "response size: ");
    sprintf(numberBuff, "%ld\n", req->response_size);
    strcat(ret, numberBuff);


    strcat(ret, "elapsed: ");
    sprintf(numberBuff, "%lf\n", req->elapsed);
    strcat(ret, numberBuff);

    strcat(ret, "Content Type: ");
    strcat(ret, req->content_type);
    ret2 = realloc(ret, 1+ strlen(ret) + strlen(req->content_type)+1);
    if (ret2 == NULL) {
        KFS_ERROR("Out of memory", NULL);
        free(ret);
        return RET_ALLOC_ERROR;
    } else { ret = ret2;}
    strcat(ret, "\n");

    strcat(ret, "headers:\n");
    for (int inx = 0; inx < req->header_size; inx++) {
        unsigned long sl_n = strlen(req->header_names[inx]);
        ret2 = realloc(ret, 1+strlen(ret) + sl_n + 10 + strlen(req->header_values[inx]));
        if (ret2 == NULL) {
            KFS_ERROR("Out of memory", NULL);
            free(ret);
            return RET_ALLOC_ERROR;
        }
        ret = ret2;
        strcat(ret, "\t");
        strcat(ret, req->header_names[inx]);
        if (sl_n < 8) {
            strcat(ret, "\t\t");
        }
        strcat(ret, "\t:\t");
        strcat(ret, req->header_values[inx]);
        strcat(ret, "\n");
    }

    ret2 = realloc(ret, 1+ 10+ strlen(ret)  + strlen(req->response));
    if (ret2== NULL) {
        KFS_ERROR("Out of memory", NULL);
        free(ret);
        return RET_ALLOC_ERROR;
    } else {
        ret = ret2;
    }
    strcat(ret, "response:\n");
    strcat(ret, req->response);

    *output = ret;
    return RET_OK;
}

int request_set_header(Request *req, int count, char **headers) {
    for (int inx = 0; inx < count; inx++) {
        req->headers = curl_slist_append(req->headers, headers[inx]);
    }
    return RET_OK;
}

int request_to_value(const Request *req, Value **output) {
    Value *value = value_new_object();
    value_object_add(value, "secure", value_new_bool(req->secure==TRUE));
    if (req->headers != NULL) {
        Value *headers = value_new_list();
        for (struct curl_slist *slist = req->headers; slist != NULL; slist = slist->next ) {
            value_list_add(headers, value_new_string(slist->data));
        }
        value_object_add(value, "request_headers", headers);
    }
    value_object_add(value, "response_code", value_new_int((int)req->response_code));
    value_object_add(value, "elapsed", value_new_double(req->elapsed));
    value_object_add(value, "response", value_new_string(req->response));
    value_object_add(value, "response_size", value_new_int((int)req->response_size));
    value_object_add(value, "content_type", value_new_string(req->content_type));
    value_object_add(value, "header_size", value_new_int((int)req->header_size));
    value_object_add(value, "header_len", value_new_int((int)req->header_len));

    if (req->header_size > 0) {
        Value *headers = value_new_object();
        for (int inx = 0; inx < req->header_size; inx++) {
            value_object_add(headers, req->header_names[inx], value_new_string(req->header_values[inx]));
        }
        value_object_add(value, "response_headers", headers);
    }
    *output = value;
    return RET_OK;
}