#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "../include/Network/tpool.hpp"

void *threadpool_t::worker_thread(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;

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

threadpool_t::threadpool_t(int num_threads)
{
    // threadpool_t pool() = new threadpool_t(num_threads);
    this->num_threads = num_threads;
    this->queue_size = 0;
    this->queue_front = 0;
    this->queue_rear = 0;
    this->shutdown = 0;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);

    workers = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    for (int i = 0; i < num_threads; i++)
        pthread_create(&workers[i], NULL, worker_thread, this);
}

int threadpool_t::submit(void (*func)(void *), void *arg)
{
    pthread_mutex_lock(&lock);

    // Wait if queue is full
    while (queue_size == MAX_QUEUE && !shutdown)
        pthread_cond_wait(&not_full, &lock);

    if (shutdown)
    {
        pthread_mutex_unlock(&lock);
        return -1;
    }

    // Enqueue task
    queue[queue_rear].function = func;
    queue[queue_rear].arg = arg;
    queue_rear = (queue_rear + 1) % MAX_QUEUE;
    queue_size++;

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&lock);

    return 0;
}

threadpool_t::~threadpool_t()
{
    pthread_mutex_lock(&lock);
    shutdown = 1;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&lock);

    for (int i = 0; i < num_threads; i++)
        pthread_join(workers[i], NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);
    free(workers);
}
