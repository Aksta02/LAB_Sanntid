#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define N 5

pthread_mutex_t forks[N];
pthread_barrier_t barrier;

void *philosopher(void *args)
{
    long id = (long)args;
    int left = id;
    int right = (id + 1) % N;

    pthread_barrier_wait(&barrier);   // Alle starter samtidig

    pthread_mutex_lock(&forks[left]);
    printf("Philosopher %ld picked up fork %d\n", id, left);
    usleep(100 * 1000);               // Alle rekker å ta venstre gaffel
    pthread_mutex_lock(&forks[right]);
    printf("Philosopher %ld picked up fork %d and eats\n", id, right);

    pthread_mutex_unlock(&forks[right]);
    pthread_mutex_unlock(&forks[left]);
    return NULL;
}

int main()
{
    pthread_t threads[N];
    pthread_barrier_init(&barrier, NULL, N);

    for (int i = 0; i < N; i++)
        pthread_mutex_init(&forks[i], NULL);
    for (long i = 0; i < N; i++)
        pthread_create(&threads[i], NULL, philosopher, (void *)i);
    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);

    printf("All philosophers are done\n");
    return 0;
}