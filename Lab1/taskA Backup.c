#include "time_header.h"

void busy_wait(struct timespec t){
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);
    
    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

int main(){
	struct timespec wait_time;
	wait_time.tv_nsec = 0;
	wait_time.tv_sec = 1;

	busy_wait(wait_time);
	return 0;
}
