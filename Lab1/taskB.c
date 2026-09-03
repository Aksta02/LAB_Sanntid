#include <stdint.h>
#include <string.h>
#include <stdio.h>

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

    asm volatile(
        "mrs %0, cntfrq_el0"
        : "=r"(val)
    );

    return val;
}

int main(){

	int ns_max = 100;

    int histogram[100];
    memset(histogram, 0, sizeof(histogram));

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
	
	return 0;
}