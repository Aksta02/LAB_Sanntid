#include <stdio.h>
#include "sort.h"

int main(int argc, char *argv[])
{
    int i;
    
    printf("Arguments before sort: ");

    for (i=1;i<argc;i++)
        printf("%s ",argv[i]);
    printf("\n");
    
    sort(argc,argv);

    printf("Arguments after sort:  ");

    for (i=1;i<argc;i++)
        printf("%s ",argv[i]);
    printf("\n");

    return 0;
}
struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}

int timespec_cmp(struct timespec lhs, struct timespec rhs){
    if (lhs.tv_sec < rhs.tv_sec)
        return -1;
    if (lhs.tv_sec > rhs.tv_sec)
        return 1;
    return lhs.tv_nsec - rhs.tv_nsec;
}

void busy_wait(struct timespec t){
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);
    
    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void times(long sekund){
    struct tms buf;
    long hz = sysconf(_SC_CLK_TCK);
    clock_t start = times(&buf);
	clock_t now =start; 
    
    while(now - start < sekund *hz){
        for(volatile int i = 0; i < 10000; i++){}
        now = times(&buf);
    }
}