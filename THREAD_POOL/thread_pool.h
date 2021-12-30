#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#define MAX_THREADS 64
#define MAX_QUEUE 65536

typedef struct threadpool_t threadpool_t;

// create a thread pool, third parametr is not used
threadpool_t *threadpool_create(int thread_count, int queue_size, int flags);

//add tasks
int threadpool_add(threadpool_t *pool, void(*routine)(void *), void *arg, int flags);

//destroy trad pools
int threadpool_destroy(threadpool_t *pool, int flags);

#endif