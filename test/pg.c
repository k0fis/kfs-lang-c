#include <stdio.h>
#include <unistd.h>

#include "thread.h"

#include "options.h"
#include "utils.h"

int thread_check(void *arg) {
    return  *((int *)arg) > 15;
}
void thread_start(void *arg) {
    int *i = (int *)arg;
    printf("value: %i\n", *i);
    (*i) = *i +1;
}

int main(int argc, char *argv[]) {
    int res = RET_OK;
    Options *options;

    options_create(&options);
    options_fulfill(options, argc, argv);

    Timer *timer;
    int inx = 0;
    timer_create(&timer, 0, 500000, thread_start, thread_check, &inx);

    sleep(5);
    timer_delete(timer);
    sleep(2);
    options_delete(options);
    return res;
}
