#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <string.h>

int main() {

    int ns_max = 10000;
    int histogram[10000];

    memset(histogram, 0, sizeof(histogram));

    for (int i = 0; i < 1000000; i++) {

        struct timespec t1, t2;

        clock_gettime(CLOCK_MONOTONIC, &t1);

        sched_yield();

        clock_gettime(CLOCK_MONOTONIC, &t2);

        long ns = (t2.tv_sec - t1.tv_sec) * 1000000000L
                + (t2.tv_nsec - t1.tv_nsec);

        if (ns >= 0 && ns < ns_max) {
            histogram[ns]++;
        }
    }

    for (int i = 0; i < ns_max; i++) {
        printf("%d\n", histogram[i]);
    }

    return 0;
}