// clang taskH_fixed.c -o taskH_fixed -std=gnu11 -g -fsanitize=thread -lpthread
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define N 5

pthread_mutex_t forks[N];
pthread_barrier_t barrier;

void *philosopher(void *args)
{
    long id = (long)args;
    int first = id;
    int second = (id + 1) % N;


    if (id == N - 1)
    {
        first = 0;
        second = N - 1;
    }

    pthread_barrier_wait(&barrier);  

    pthread_mutex_lock(&forks[first]);
    printf("Philosopher %ld picked up fork %d\n", id, first);
    usleep(100 * 1000);             
    pthread_mutex_lock(&forks[second]);
    printf("Philosopher %ld picked up fork %d and eats\n", id, second);

    pthread_mutex_unlock(&forks[second]);
    pthread_mutex_unlock(&forks[first]);
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