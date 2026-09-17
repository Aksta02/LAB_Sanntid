#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "scheduling.h"
#include "schedulers.h"

void set_task_state(struct Task *task, enum taskState taskNewState)
{
    pthread_mutex_lock(&taskStateMutex);
    task->state = taskNewState;
    pthread_mutex_unlock(&taskStateMutex);
}

void wait_for_rescheduling(int quantum, struct Task *task)
{
    int startTime;
    int waitTime;

    pthread_mutex_lock(&timeMutex);
    startTime = globalTime;
    pthread_mutex_unlock(&timeMutex);

    do
    {
        pthread_mutex_lock(&timeMutex);
        pthread_cond_wait(&timeCond, &timeMutex);
        waitTime = globalTime - startTime;
        pthread_mutex_unlock(&timeMutex);
    } while (task->state != finished && waitTime < quantum);

    usleep(timeUnitUs / 100);
}

void round_robin(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    int taskIndex = 0;

    do
    {
        // Skip finished tasks or those that have not arrived yet
        if (tasks[taskIndex]->state == finished || tasks[taskIndex]->arrivalTime > globalTime)
        {
            taskIndex = (taskIndex + 1) % taskCount;
            continue;
        }

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Wait for the quantum interval
        wait_for_rescheduling(quantum, tasks[taskIndex]);

        //  Check if the task is finished
        if (tasks[taskIndex]->state == finished)
        {
        }
        else
        {
            set_task_state(tasks[taskIndex], preempted);
        }

        // Find the next task to run
        taskIndex = (taskIndex + 1) % taskCount;

    } while (globalTime < timeout);
}
// Implement your schedulers here!
void first_come_first_served(struct Task **tasks, int taskCount, int timeout)
{
    int taskIndex = 0;

    do
    {
        //Skipper ferdige tasker eller om de ikke har ankommet enda
        if (tasks[taskIndex]->state == finished || tasks[taskIndex]->arrivalTime > globalTime)
        {
            taskIndex = (taskIndex + 1) % taskCount;
            continue;
        }

        //Setter task til staten "running"
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        //Venter til oppgaven er helt ferdig
        pthread_mutex_lock(&timeMutex);
        while (tasks[taskIndex]->state != finished)
        {
            pthread_cond_wait(&timeCond, &timeMutex);
        }
        pthread_mutex_unlock(&timeMutex);

        //Gå til neste oppgave
        taskIndex = (taskIndex + 1) % taskCount;

    } while (globalTime < timeout);
}
void shortest_process_next(struct Task **tasks, int taskCount, int timeout)
{
int taskIndex = -1;

do
{
    taskIndex = -1;   //resett hver runde

    for (int i = 0; i < taskCount; i++)
    {
        // Skip finished tasks or those that have not arrived yet
        if (tasks[i]->state == finished || tasks[i]->arrivalTime > globalTime)
        {
            continue;
        }
        if (taskIndex == -1 || tasks[i]->totalRuntime < tasks[taskIndex]->totalRuntime)
        {
            taskIndex = i;
        }
    }

    if (taskIndex == -1)
    {
        // Ingen oppgaver klare akkurat nå - vent til neste tidsenhet
        pthread_mutex_lock(&timeMutex);
        pthread_cond_wait(&timeCond, &timeMutex);
        pthread_mutex_unlock(&timeMutex);
        continue;
    }

    // Setter task til staten "running"
    if (tasks[taskIndex]->startTime == -1)
        tasks[taskIndex]->startTime = globalTime;
    set_task_state(tasks[taskIndex], running);

    // Venter til oppgaven er helt ferdig
    pthread_mutex_lock(&timeMutex);
    while (tasks[taskIndex]->state != finished)
    {
        pthread_cond_wait(&timeCond, &timeMutex);
    }
    pthread_mutex_unlock(&timeMutex);

} while (globalTime < timeout);
}
void highest_response_ratio_next(struct Task **tasks, int taskCount, int timeout)
{
    int taskIndex = -1;

    do
    {
        taskIndex = -1;
        double bestRatio = -1.0;

        for (int i = 0; i < taskCount; i++)
        {
            if (tasks[i]->state == finished || tasks[i]->arrivalTime > globalTime)
            {
                continue;
            }

            double waitTime = globalTime - tasks[i]->arrivalTime;
            double ratio = (waitTime + tasks[i]->totalRuntime) / (double)tasks[i]->totalRuntime;

            if (taskIndex == -1 || ratio > bestRatio)
            {
                bestRatio = ratio;
                taskIndex = i;
            }
        }

        if (taskIndex == -1)
        {
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }

        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        pthread_mutex_lock(&timeMutex);
        while (tasks[taskIndex]->state != finished)
        {
            pthread_cond_wait(&timeCond, &timeMutex);
        }
        pthread_mutex_unlock(&timeMutex);

    } while (globalTime < timeout);
}
void shortest_remaining_time(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    int taskIndex = -1;

    do
    {
        taskIndex = -1;
        int bestRemaining = -1;

        for (int i = 0; i < taskCount; i++)
        {
            // Skip finished tasks or those that have not arrived yet
            if (tasks[i]->state == finished || tasks[i]->arrivalTime > globalTime)
            {
                continue;
            }

            int remaining = tasks[i]->totalRuntime - tasks[i]->currentRuntime;

            if (taskIndex == -1 || remaining < bestRemaining)
            {
                bestRemaining = remaining;
                taskIndex = i;
            }
        }

        if (taskIndex == -1)
        {
            // Ingen oppgaver klare akkurat nå - vent til neste tidsenhet
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }

        // Sett valgt oppgave til running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Kjør et quantum, sjekk deretter om noe kortere har dukket opp
        wait_for_rescheduling(quantum, tasks[taskIndex]);

        // Hvis oppgaven ikke ble finished i løpet av quantumet, preempt den
        if (tasks[taskIndex]->state != finished)
        {
            set_task_state(tasks[taskIndex], preempted);
        }

    } while (globalTime < timeout);
}
void feedback(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    int *level = malloc(taskCount * sizeof(int));
    for (int i = 0; i < taskCount; i++)
        level[i] = 0;  // alle starter på høyeste prioritet

    do
    {
        int taskIndex = -1;
        int bestLevel = -1;

        for (int i = 0; i < taskCount; i++)
        {
            // Skip finished tasks or those that have not arrived yet
            if (tasks[i]->state == finished || tasks[i]->arrivalTime > globalTime)
            {
                continue;
            }

            if (taskIndex == -1 || level[i] < bestLevel)
            {
                bestLevel = level[i];
                taskIndex = i;
            }
        }

        if (taskIndex == -1)
        {
            // Ingen oppgaver klare akkurat nå - vent til neste tidsenhet
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }

        // Sett valgt oppgave til running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Quantum dobles for hvert nivå ned
        int taskQuantum = quantum * (1 << level[taskIndex]);  // quantum * 2^level

        wait_for_rescheduling(taskQuantum, tasks[taskIndex]);

        if (tasks[taskIndex]->state != finished)
        {
            set_task_state(tasks[taskIndex], preempted);
            level[taskIndex]++;  // degrader til lavere prioritet
        }

    } while (globalTime < timeout);

    free(level);
}
