#ifndef _kfs_eval_h_
#define _kfs_eval_h_

#include "value.h"
#include "expression.h"

#if defined(__cplusplus)
extern "C" {
#endif


Value *eval_value(Expression *expression);

#if defined(__cplusplus)
}
#endif
#endif