//
// Created by Pavel Dřímalka on 25.05.2025.
//

#ifndef ENV_H
#define ENV_H

#include "options.h"

#ifdef cplusplus
extern "C" {
#endif

int env_load_file(char *filename, Options *options);
int env_set_env(char *key, char *value, Options *options);

#ifdef cplusplus
}
#endif

#endif //ENV_H
