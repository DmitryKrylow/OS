/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"
#include "list.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool

// the work queue
task worktodo;

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem;
struct node *queue = NULL;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    task *newTask = malloc(sizeof(task));
    if(!newTask){
        return 1;
    }
    newTask->function = t.function;
    newTask->data = t.data;

    pthread_mutex_lock(&mutex);
    insert(&queue, newTask);
    pthread_mutex_unlock(&mutex);

    return 0;
}

// remove a task from the queue
task dequeue() 
{
    pthread_mutex_lock(&mutex);
    delete(&queue, queue->task);
    pthread_mutex_unlock(&mutex);

    return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    while (queue != NULL) {
        // execute the task
        sem_wait(&sem);
        execute(queue->task->function, queue->task->data);
        dequeue();
    }

    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{

    worktodo.function = somefunction;
    worktodo.data = p;

    int res = enqueue(worktodo);
    if(!res){
        sem_post(&sem);
    }

    return res;

}

// initialize the thread pool
void pool_init(void)
{
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem,0,0);
    for(int i =0; i < NUMBER_OF_THREADS; i++) {
        pthread_create(&bee[i], NULL, worker, NULL);
    }
}

// shutdown the thread pool
void pool_shutdown(void)
{
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);
    for(int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_join(bee[i], NULL);
    }
}
