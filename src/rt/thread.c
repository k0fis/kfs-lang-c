#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "thread.h"
#include "utils.h"
#include "ll/ll.h"

typedef struct tag_multi_threads_env {
    Options *options;
    ll_t threadList;
    ll_t mutexList;

    pthread_mutex_t threadMutex;
} MultiThreadEnvs;

typedef struct tag_thread {
    pthread_t thread;
    int endingOptions;
    int joined;
    void *resultData;
    void (* threadData_free)(void *);
    MultiThreadEnvs *env;

    ll_t handle;
} Thread;

typedef struct tag_mutex {
    pthread_mutex_t mutex;
    MultiThreadEnvs *env;
    ll_t handle;
} Mutex;

typedef struct tag_timer {
    void (*call_fn)(void *data);
    int (*call_stop)(void *data);
    void *data;

    unsigned int seconds;
    unsigned int uSeconds;
    int sysEnd;
    pthread_t tid;
} Timer;

int thread_env_create(MultiThreadEnvs **mtEnv, Options *options) {
    KFS_MALLOC2(MultiThreadEnvs, env);
    env->options = options;
    KFS_LST_INIT(env->threadList);
    KFS_LST_INIT(env->mutexList);
    int res; check_error(res, pthread_mutex_init(&env->threadMutex, NULL), "pthread_mutex_init", RET_CANNOT_INIT_MUTEX);
    *mtEnv = env;
    return RET_OK;
}

int thread_env_delete(MultiThreadEnvs *mtEnv) {
    {
        Thread *inx, *tmp; list_for_each_entry_safe(inx, tmp, &mtEnv->threadList, handle) {
            thread_thread_delete(inx);
        }
    }
    {
        Mutex *inx, *tmp; list_for_each_entry_safe(inx, tmp, &mtEnv->mutexList, handle) {
            thread_mutex_delete(inx);
        }
    }
    int res; check_error(res, pthread_mutex_destroy(&mtEnv->threadMutex), "pthread_mutex_destroy", RET_CANNOT_DESTROY_MUTEX);
    free(mtEnv);
    return RET_OK;
}

int thread_thread_create_int(MultiThreadEnvs *mtEnv, Thread **threadOut, void * (* threadStart)(void *), void *arg, void (* threadData_free)(void *), int endingOptions) {
    KFS_MALLOC2(Thread, thread);
    thread->endingOptions = endingOptions;
    thread->joined = 0;
    thread->resultData = NULL;
    thread->threadData_free = threadData_free;
    thread->env = mtEnv;
    int res;
    pthread_attr_t attr;
    check_error(res, pthread_attr_init(&attr), "pthread_attr_init", RET_THREAD_INIT_ATTR)

    if (mtEnv->options->threadStackSize != -1) {
        check_error(res, pthread_attr_setstacksize(&attr, mtEnv->options->threadStackSize), "pthread_attr_setstacksize", RET_THREAD_SETUP_STACK_ATTR)
    }

    check_error(res,pthread_create(&thread->thread, &attr, threadStart, arg), "pthread_create", RET_THREAD_CREATE_PTHREAD)

    check_error(res, pthread_attr_destroy(&attr), "pthread_attr_destroy", RET_THREAD_DESTROY_ATTR)

    list_add(&thread->handle, &mtEnv->threadList);
    if (threadOut != NULL) {
        *threadOut = thread;
    }
    return RET_OK;
}

int thread_mutex_lock(pthread_mutex_t *mutex) {
    int res; check_error(res, pthread_mutex_lock(mutex), "pthread_mutex_lock", RET_CANNOT_LOCK_MUTEX);
    return RET_OK;
}

int thread_mutex_unlock(pthread_mutex_t *mutex) {
    int res; check_error(res, pthread_mutex_unlock(mutex), "pthread_mutex_unlock", RET_CANNOT_UNLOCK_MUTEX);
    return RET_OK;
}

int thread_thread_create(MultiThreadEnvs *mtEnv, Thread **threadOut, void * (* threadStart)(void *), void *arg, void (* threadData_free)(void *), int endingOptions) {
    thread_mutex_lock(&mtEnv->threadMutex);
    int res = thread_thread_create_int(mtEnv, threadOut, threadStart, arg, threadData_free, endingOptions);
    thread_mutex_unlock(&mtEnv->threadMutex);
    return res;
}

int thread_thread_delete(Thread *thread) {
    thread_mutex_lock(&thread->env->threadMutex);
    list_del(&thread->handle);
    if (thread->endingOptions & THREAD_ENDING_JOIN) {
        thread_thread_join(thread);
    }
    if (thread->endingOptions & THREAD_ENDING_CANCEL) {
        thread_thread_cancel(thread);
    }
    if (thread->resultData != NULL) {
        if (thread->threadData_free != NULL) {
            thread->threadData_free(thread->resultData);
        } else {
            free(thread->resultData);
        }
    }
    thread_mutex_unlock(&thread->env->threadMutex);
    free(thread);
    return RET_OK;
}

int thread_thread_cancel(Thread *thread) {
    int res;
    check_error(res, pthread_cancel(thread->thread), "pthread_cancel", RET_CANNOT_CANCEL_THREAD);
    return RET_OK;
}

int thread_thread_join(Thread *thread) {
    int res;
    if (thread->joined > 0) {
        check_error(res, pthread_join(thread->thread, &thread->resultData), "pthread_join", RET_THREAD_JOIN);
        thread->joined++;
    }
    return RET_OK;
}

int thread_mutex_create(MultiThreadEnvs *mtEnv, Mutex **mutexOut) {
    thread_mutex_lock(&mtEnv->threadMutex);
    KFS_MALLOC2(Mutex, mutex);
    int res; check_error2(res, pthread_mutex_init(&mutex->mutex, NULL), "pthread_mutex_init");
    if (res != RET_OK) {
        thread_mutex_unlock(&mtEnv->threadMutex);
        return RET_CANNOT_INIT_MUTEX;
    }
    mutex->env = mtEnv;
    list_add(&mutex->handle, &mtEnv->mutexList);
    *mutexOut = mutex;
    thread_mutex_unlock(&mtEnv->threadMutex);
    return RET_OK;
}
int thread_mutex_delete(Mutex *mutex) {
    thread_mutex_lock(&mutex->env->threadMutex);
    int res2 = RET_OK;
    check_error2(res2, pthread_mutex_destroy(&mutex->mutex), "pthread_mutex_destroy");
    list_del(&mutex->handle);
    thread_mutex_unlock(&mutex->env->threadMutex);
    free(mutex);
    return res2;
}



void *timer_run(void *t);

int timer_create(Timer **outTimer, unsigned int seconds, unsigned int uSeconds, \
    void (*call_fn)(void *data), int (*call_stop)(void *data), void *data)
{
    int res;
    assert(call_fn!=NULL);
    KFS_MALLOC2(Timer, timer);
    timer->call_fn = call_fn;
    timer->call_stop = call_stop;
    timer->data = data;
    timer->seconds = seconds;
    timer->uSeconds = uSeconds;
    timer->sysEnd = FALSE;
    check_error2(res, pthread_create(&timer->tid, NULL, timer_run, timer), "pthread_create");
    *outTimer = timer;
    return RET_OK;
}

int timer_delete(Timer *timer) {
    int res;
    timer->sysEnd = TRUE;
    check_error2(res, pthread_join(timer->tid, NULL), "pthread_join");
    free(timer);
    return RET_OK;
}

void * timer_run(void *t) {
    int res;
    Timer *timer = (Timer *) t;
    while (TRUE) {
        KFS_TRACE("timer_run - 1", NULL);
        if (timer->seconds > 0) {
            sleep(timer->seconds);
        }
        if (timer->uSeconds > 0) {
            usleep(timer->uSeconds);
        }
        KFS_TRACE("timer_run - 2", NULL);
        timer->call_fn(timer->data);
        KFS_TRACE("timer_run - 3", NULL);
        if (timer->sysEnd) {
            break;
        }
        if (timer->call_stop != NULL) {
            res = timer->call_stop(timer->data);
            if (res) {
                break;
            }
        }
    }
    return NULL;
}