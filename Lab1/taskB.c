#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>


// Leser CPU-telleren
uint64_t rdtsc(void)
{
    uint64_t val;

    asm volatile(
        "isb; mrs %0, cntvct_el0; isb;"
        : "=r"(val)
        :
        : "memory"
    );

    return val;
}


// Leser frekvensen til CPU-telleren
uint64_t rdtsc_freq(void)
{
    uint64_t val;

    asm volatile(
        "mrs %0, cntfrq_el0"
        : "=r"(val)
    );

    return val;
}


int main()
{
    int ns_max = 200;

    int histogram_rdtsc[200];
    int histogram_clock[200];

    memset(histogram_rdtsc, 0, sizeof(histogram_rdtsc));
    memset(histogram_clock, 0, sizeof(histogram_clock));


    // -------------------------
    // RDTSC
    // -------------------------

    uint64_t freq = rdtsc_freq();

    for (int i = 0; i < 10 * 1000 * 1000; i++)
    {
        uint64_t t1 = rdtsc();
        uint64_t t2 = rdtsc();

        uint64_t ticks = t2 - t1;

        int ns = (int)(ticks * 1000000000ULL / freq);

        if (ns >= 0 && ns < ns_max)
        {
            histogram_rdtsc[ns]++;
        }
    }


    // -------------------------
    // CLOCK_GETTIME
    // -------------------------

    for (int i = 0; i < 10 * 1000 * 1000; i++)
    {
        struct timespec t1, t2;

        clock_gettime(CLOCK_MONOTONIC, &t1);
        clock_gettime(CLOCK_MONOTONIC, &t2);

        long ns =
            (t2.tv_sec - t1.tv_sec) * 1000000000L
            + (t2.tv_nsec - t1.tv_nsec);

        if (ns >= 0 && ns < ns_max)
        {
            histogram_clock[ns]++;
        }
    }


    // -------------------------
    // TIMES
    // -------------------------

    long ticks_per_second = sysconf(_SC_CLK_TCK);

    double times_resolution_ns =
        1000000000.0 / ticks_per_second;

    fprintf(stderr,
            "times() CLK_TCK = %ld\n",
            ticks_per_second);

    fprintf(stderr,
            "times() resolution = %.0f ns\n",
            times_resolution_ns);


    // SKRIV HISTOGRAM

    for (int i = 0; i < ns_max; i++)
    {
        printf("%d %d %d\n",
               i,
               histogram_rdtsc[i],
               histogram_clock[i]);
    }

    return 0;
}