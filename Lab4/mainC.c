// clang mainC.c -o mainC -std=gnu11 -g -lpthread
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>


#define NUM_ITERATIONS 1000000
#define NUM_THREADS 2

long g_counter = 0;            // Delt mellom trådene
pthread_barrier_t g_barrier;   // Sørger for at trådene starter samtidig
sem_t g_sem;


void *fn(void *args)
{
    long id = (long)args;
    long local = 0;            // Hver tråd har sin egen kopi på stacken

    // Vent til begge trådene er klare, slik at de starter samtidig
    pthread_barrier_wait(&g_barrier);

    for (long i = 0; i < NUM_ITERATIONS; i++)
    {
        sem_wait(&g_sem);      // Setter verdien fra 1 til 0, eller venter hvis den er 0
        g_counter++;
        sem_post(&g_sem);      // Setter verdien fra 0 til 1 og vekker en ventende tråd
        local++;
    }

    // Les den delte variabelen mens vi holder låsen
    sem_wait(&g_sem);
    long global = g_counter;
    sem_post(&g_sem);

    printf("Thread %ld: local = %ld, global = %ld\n", id, local, global);
    return NULL;
}

int main()
{
    pthread_t threadHandles[NUM_THREADS];

    pthread_barrier_init(&g_barrier, NULL, NUM_THREADS);
	sem_init(&g_sem, 0, 1); // 0 vil si at den deles mellom to tråder hvor 1 er startverdien 

    for (long i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threadHandles[i], NULL, fn, (void *)i);
    }

    for (long i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threadHandles[i], NULL);
    }

    pthread_barrier_destroy(&g_barrier);
	sem_destroy(&g_sem);

    printf("Final global = %ld (expected %d)\n", g_counter, NUM_THREADS * NUM_ITERATIONS);
    return 0;
}