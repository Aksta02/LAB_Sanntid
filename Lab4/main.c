// clang main.c -o main -std=gnu11 -g -lpthread
#include <stdio.h>
#include <pthread.h>


#define NUM_ITERATIONS 1000000
#define NUM_THREADS 2

long g_counter = 0;            // Delt mellom trådene
pthread_barrier_t g_barrier;   // Sørger for at trådene starter samtidig

void *fn(void *args)
{
    long id = (long)args;
    long local = 0;            // Hver tråd har sin egen kopi på stacken

    // Vent til begge trådene er klare, slik at de starter samtidig
    pthread_barrier_wait(&g_barrier);

    for (long i = 0; i < NUM_ITERATIONS; i++)
    {
        g_counter++;
        local++;
    }

    printf("Thread %ld: local = %ld, global = %ld\n", id, local, g_counter);
    return NULL;
}

int main()
{
    pthread_t threadHandles[NUM_THREADS];

    pthread_barrier_init(&g_barrier, NULL, NUM_THREADS);

    for (long i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threadHandles[i], NULL, fn, (void *)i);
    }

    for (long i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threadHandles[i], NULL);
    }

    pthread_barrier_destroy(&g_barrier);

    printf("Final global = %ld (expected %d)\n", g_counter, NUM_THREADS * NUM_ITERATIONS);
    return 0;
}