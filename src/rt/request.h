#ifndef _request_h_
#define _request_h_

#include "options.h"

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

typedef struct tag_request Request;

int request_init();
int request_cleanup();

int request_new(Request **req, Options *);
int request_delete(Request *req);

int request_post(Request *req);
int request_get(Request *req);

#if defined(__cplusplus)
}
#endif  // __cplusplus

#endif
