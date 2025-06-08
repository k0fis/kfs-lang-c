#ifndef _request_h_
#define _request_h_

#include "options.h"
#include "value.h"

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

typedef struct tag_request Request;

int request_init();
int request_cleanup();

int request_new(Request **request, char *url, Options *);
int request_delete(Request *req);
int request_set_header(Request *req, int count, char **headers);

int request_post(Request *req);
int request_get(Request *req);

int request_to_string(Request *req, char **output);
int request_to_value(Request *req, Value **output);

#if defined(__cplusplus)
}
#endif  // __cplusplus

#endif
