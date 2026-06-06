#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/Network/tpool.h"

static void *worker_thread(void *arg)
{
    threadpool_t *pool = arg;

    while (1)
    {
        pthread_mutex_lock(&pool->lock);

        while (pool->queue_size == 0 && pool->shutdown == 0)
        {
            pthread_cond_wait(&pool->not_empty, &pool->lock);
        }

        if (pool->shutdown && pool->queue_size == 0)
        {
            pthread_mutex_unlock(&pool->lock);
            pthread_exit(NULL);
        }

        task_t task = pool->queue[pool->queue_front];
        pool->queue_front = (pool->queue_front + 1) % MAX_QUEUE;
        pool->queue_size--;

        pthread_cond_signal(&pool->not_full);
        pthread_mutex_unlock(&pool->lock);

        task.function(task.arg);
    }

    return NULL;
}

threadpool_t *threadpool_create(int num_threads)
{
    threadpool_t *pool = malloc(sizeof(threadpool_t));
    pool->num_threads = num_threads;
    pool->queue_size = 0;
    pool->queue_front = 0;
    pool->queue_rear = 0;
    pool->shutdown = 0;

    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->not_empty, NULL);
    pthread_cond_init(&pool->not_full, NULL);

    pool->workers = malloc(num_threads * sizeof(pthread_t));
    for (int i = 0; i < num_threads; i++)
        pthread_create(&pool->workers[i], NULL, worker_thread, pool);

    return pool;
}

int threadpool_submit(threadpool_t *pool, void (*func)(void *), void *arg)
{
    pthread_mutex_lock(&pool->lock);

    // Wait if queue is full
    while (pool->queue_size == MAX_QUEUE && !pool->shutdown)
        pthread_cond_wait(&pool->not_full, &pool->lock);

    if (pool->shutdown)
    {
        pthread_mutex_unlock(&pool->lock);
        return -1;
    }

    // Enqueue task
    pool->queue[pool->queue_rear].function = func;
    pool->queue[pool->queue_rear].arg = arg;
    pool->queue_rear = (pool->queue_rear + 1) % MAX_QUEUE;
    pool->queue_size++;

    pthread_cond_signal(&pool->not_empty);
    pthread_mutex_unlock(&pool->lock);
    return 0;
}

void threadpool_destroy(threadpool_t *pool)
{
    pthread_mutex_lock(&pool->lock);
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->not_empty);
    pthread_mutex_unlock(&pool->lock);

    for (int i = 0; i < pool->num_threads; i++)
        pthread_join(pool->workers[i], NULL);

    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->not_empty);
    pthread_cond_destroy(&pool->not_full);
    free(pool->workers);
    free(pool);
}
