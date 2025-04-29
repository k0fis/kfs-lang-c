#ifndef _kfs_eval_h_
#define _kfs_eval_h_

#include "utils.h"


#if defined(__cplusplus)
extern "C" {
#endif

Expression *eval_kfs_lang(char *code);
Value *eval_value(Expression *expression);

#if defined(__cplusplus)
}
#endif
#endif