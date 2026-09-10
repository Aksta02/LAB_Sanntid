#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>

uint64_t rdtsc(void){
    uint64_t val;
    asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val) :: "memory"); 
    // You can check the current CPU frequency with $sudo dmesg | grep MHz
    return val;
}
// for(int i = 0; i < 10*1000*1000; i++){
//     	// read timer
// 	}

uint64_t rdtsc_freq(void)
{
    uint64_t val;
//RDTSC
    uint64_t freq = rdtsc_freq();

    for (int i = 0; i < 10 * 1000 * 1000; i++) {

        uint64_t t1 = rdtsc();
        uint64_t t2 = rdtsc();

        uint64_t ticks = t2 - t1;

        int ns = (int)(ticks * 1000000000ULL / freq);

        if (ns >= 0 && ns < ns_max) {
            histogram[ns]++;
        }
    }

    for (int i = 0; i < ns_max; i++) {
        printf("%d\n", histogram[i]);
	}

    asm volatile(
        "mrs %0, cntfrq_el0"
        : "=r"(val)
    );

    return val;
}

int main(){

	int ns_max = 100;

    int histogram_rdtsc[100];
    int histogram_clock[100];
    int histogram_times[100];

    memset(histogram_rdtsc, 0, sizeof(histogram_rdtsc));
    memset(histogram_clock, 0, sizeof(histogram_clock));
    memset(histogram_times, 0, sizeof(histogram_times));

	// RDTSC
    uint64_t freq = rdtsc_freq();

    for (int i = 0; i < 10 * 1000 * 1000; i++) {

        uint64_t t1 = rdtsc();
        uint64_t t2 = rdtsc();

        uint64_t ticks = t2 - t1;

        int ns = (int)(ticks * 1000000000ULL / freq);

        if (ns >= 0 && ns < ns_max) {
            histogram_rdtsc[ns]++;
        }
    }

	// CLOCK_GETTIME
    for (int i = 0; i < 10 * 1000 * 1000; i++) {

        struct timespec ts1, ts2;

        clock_gettime(CLOCK_MONOTONIC, &ts1);
        clock_gettime(CLOCK_MONOTONIC, &ts2);

        int ns_cg =
            (int)((ts2.tv_sec - ts1.tv_sec) * 1000000000LL
            + (ts2.tv_nsec - ts1.tv_nsec));

        if (ns_cg >= 0 && ns_cg < ns_max) {
            histogram_clock[ns_cg]++;
        }
    }

	/// TIMES
    long ticks_per_sec = sysconf(_SC_CLK_TCK);

    for (int i = 0; i < 10 * 1000 * 1000; i++) {

        struct tms tm1, tm2;

        clock_t t1 = times(&tm1);
        clock_t t2 = times(&tm2);

        int ns_t =
            (int)((t2 - t1) * 1000000000LL / ticks_per_sec);

        if (ns_t >= 0 && ns_t < ns_max) {
            histogram_times[ns_t]++;
        }
    }

	// PRINT ALL HISTOGRAMS
    for (int i = 0; i < ns_max; i++) {

        printf("%d %d %d %d\n",
               i,
               histogram_rdtsc[i],
               histogram_clock[i],
               histogram_times[i]);
    }

	return 0;
}