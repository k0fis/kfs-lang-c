#ifndef _json_h_
#define _json_h_

#include "utils.h"
#include "options.h"
#include "value.h"
#include "json_parser.h"
#include "json_lexer.h"


#if defined(__cplusplus)
extern "C" {
#endif

int json_read_string(char *jsonStr, Value **output, Options *options);

#if defined(__cplusplus)
}
#endif


#endif //_json_h_