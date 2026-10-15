#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <wiringPi.h>

#define PERIOD_NS        2000000L 

#define TEST_A      8
#define RESPONSE_A  9
#define TEST_B      7
#define RESPONSE_B  15
#define TEST_C      16
#define RESPONSE_C  1

struct responseTaskArgs {
    struct {
        uint32_t test;
        uint32_t response;
    } pin;
    int cpu;
};

void io_init(void) {
    wiringPiSetup();

    pinMode(TEST_A, INPUT); pinMode(RESPONSE_A, OUTPUT);
    pinMode(TEST_B, INPUT); pinMode(RESPONSE_B, OUTPUT);
    pinMode(TEST_C, INPUT); pinMode(RESPONSE_C, OUTPUT);

    digitalWrite(RESPONSE_A, HIGH);
    digitalWrite(RESPONSE_B, HIGH);
    digitalWrite(RESPONSE_C, HIGH);
}

int set_cpu(int cpu_id)
{
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu_id < 0 || cpu_id >= num_cores)
        return EINVAL;

    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_id, &cpu);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}


/* static void *responseTask(void *args) {
    struct responseTaskArgs a = *(struct responseTaskArgs *)args;
	set_cpu(a.cpu);

    while (1) {
        if (digitalRead(a.pin.test) == LOW) {
            digitalWrite(a.pin.response, LOW);            // svar
            while (digitalRead(a.pin.test) == LOW) {}     // vent til BRTT resetter
            digitalWrite(a.pin.response, HIGH);           // klar for neste
        }
    }
    return NULL;
} */

static void timespec_add_ns(struct timespec *t, long ns) {
    t->tv_nsec += ns;
    while (t->tv_nsec >= 1000000000L) {
        t->tv_nsec -= 1000000000L;
        t->tv_sec++;
    }
}

static void *periodicResponseTask(void *args) {
    struct responseTaskArgs a = *(struct responseTaskArgs *)args;
    set_cpu(a.cpu);

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    while (1) {
        // Speil testpinnen til svarpinnen:
        // test LOW -> svar LOW, test HIGH -> svar HIGH
        digitalWrite(a.pin.response, digitalRead(a.pin.test));

        timespec_add_ns(&next, PERIOD_NS);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return NULL;
}


static void *disturbanceTask(void *args) {
    (void)args;
    set_cpu(0);   // samme kjerne som svartrådene

    while (1) {
        asm volatile("" ::: "memory");
    }
    return NULL;
}

int main() {
    io_init();

    pthread_t threadA, threadB, threadC;
/* 	pthread_t disturbance[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&disturbance[i], NULL, disturbanceTask, NULL);
    } */

	//for busy_polling
/*     pthread_create(&threadA, NULL, responseTask,
        &(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 0});
    pthread_create(&threadB, NULL, responseTask,
        &(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 0});
    pthread_create(&threadC, NULL, responseTask,
        &(struct responseTaskArgs){{TEST_C, RESPONSE_C}, 0}); */

	//For periodic
    pthread_create(&threadA, NULL, periodicResponseTask,
        &(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 0});
    pthread_create(&threadB, NULL, periodicResponseTask,
        &(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 0});
    pthread_create(&threadC, NULL, periodicResponseTask,
        &(struct responseTaskArgs){{TEST_C, RESPONSE_C}, 0});

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);
    return 0;
}