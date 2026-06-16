#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

#define MAX_QUEUE 100

// Task
class task_t
{
public:
    void (*function)(void *);
    void *arg;
};

// ThreadPool
class threadpool_t
{
public:
    pthread_t *workers;

    int num_threads;

    task_t queue[MAX_QUEUE];
    int queue_size;
    int queue_front;
    int queue_rear;

    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;

    int shutdown;

    static void *worker_thread(void *arg);

    threadpool_t(int num_threads);
    ~threadpool_t();

    int submit(void (*function)(void *), void *arg);
};

// threadpool_t *threadpool_create(int num_threads);

// int threadpool_submit(
//     threadpool_t *pool,
//     void (*function)(void *),
//     void *arg);

// void threadpool_destroy(
//     threadpool_t *pool);

#endif