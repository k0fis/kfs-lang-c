#include "thread.h"
#include "utils.h"

typedef struct tag_thread {} Thread;
typedef struct tag_mutes {} Mutex;

int thread_env_create(MultiThreadEnvs **mtEnv, Options *options) {
    NI("thread_create_env");
}

int thread_delete_env(MultiThreadEnvs *mtEnv) {
    NI("thread_delete_env");
}

int thread_thread_create(MultiThreadEnvs *mtEnv, Thread **threadOut, void * (* threadStart)(void *), void *arg) {
    NI("thread_create_thread");
}

int thread_delete_thread(Thread *thread) {
    NI("thread_delete_thread");
}

int thread_create_mutex(MultiThreadEnvs *mtEnv, Mutex **mutex) {
    NI("thread_create_mutex");
}

int thread_delete_mutex(Mutex *mutex) {
    NI("thread_delete_mutex");
}
