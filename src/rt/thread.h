#ifndef _THREAD_H_
#define _THREAD_H_

#include "options.h"

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus


typedef struct tag_thread Thread;
typedef struct tag_mutex Mutex;
typedef struct tag_timer Timer;
typedef struct tag_multi_threads_env  MultiThreadEnvs;

int thread_env_create(MultiThreadEnvs **mtEnv, Options *options);
int thread_env_delete(MultiThreadEnvs *mtEnv);

int thread_thread_create(MultiThreadEnvs *mtEnv, Thread **threadOut, void * (* threadStart)(void *), void *arg, void (* threadData_free)(void *), int endingOptions);
int thread_thread_delete(Thread *thread);
int thread_thread_join(Thread *thread);
int thread_thread_cancel(Thread *thread);

int thread_mutex_create(MultiThreadEnvs *mtEnv, Mutex **mutex);
int thread_mutex_delete(Mutex *mutex);

int timer_create(Timer **outTimer, unsigned int seconds, unsigned int uSeconds, \
    void (*call_fn)(void *data), int (*call_stop)(void *data), void *data);
int timer_delete(Timer *timer);

#define THREAD_ENDING_JOIN    0x001
#define THREAD_ENDING_CANCEL  0x002

#if defined(__cplusplus)
}
#endif  // __cplusplus
#endif