#include <pthread.h>
#include <stdlib.h>
#include "thread_pool.h"
#include <unistd.h>

typedef struct
{
	void(*function)(void *);
	void *argument;
} threadpool_task_t;

// definition
struct threadpool_t
{
	pthread_mutex_t lock;
	pthread_cond_t notify;
	pthread_t *threads;
	threadpool_task_t *queue;
	int thread_count;
	int queue_size;
	int head;
	int tail;
	int count;
	int shutdown;
	int started;
};

static void *threadpool_tread(void *threadpool);
int threadpool_free(threadpool_t *pool);

threadpool_t *threadpool_create(int thread_count, int queue_size, int flags)
{
	if(thread_count <= 0 || thread_count > MAX_THREADS || queue_size <= 0 || queue_size > MAX_QUEUE)
		return NULL;

	threadpool_t *pool;

	if((pool = (threadpool_t*)malloc(sizeof(threadpool_t))) == NULL)
	{
		if(pool)
			threadpool_free(pool);
		return NULL;
	}

	pool->thread_count = 0;
	pool->queue_size = queue_size;
	pool->head = pool->tail = pool->count = 0;
	pool->shutdown = pool->started = 0;
	pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
	pool->queue = (threadpool_task_t *)malloc(sizeof(threadpool_task_t) * queue_size);

	//mutex, cond
	if((pthread_mutex_init(&(pool->lock), NULL) != 0) || (pthread_cond_init(&(pool->notify), NULL) != 0) || (pool->threads == NULL) || (pool->queue == NULL))
	{
		if(pool)
			threadpool_free(pool);
		return NULL;
	}

	for(unsigned int i = 0; i < thread_count; ++i)
	{
		if(pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void*)pool) != 0)
		{
			threadpool_destroy(pool, 0);
			return NULL;
		}

		++pool->thread_count;
		++pool->started;
	}

	return pool;
}

int threadpool_add(threadpool_t *pool, void(*function)(void *), void *argument, int flags)
{
	int next;
	int err = 0;

	if(pool == NULL || function == NULL)
		return -1;

	if(pthread_mutex_lock(&(pool->lock)) != 0)
		return -2;

	next = pool->tail + 1;
	next = (next == pool->queue_size) ? 0 : next;

	do
	{
		if(pool->count == pool->queue_size)
		{
			err = -3;
			break;
		}

		if(pool->shutdown)
		{
			err = -4;
			break;
		}

		pool->queue[pool->tail].function = function;
		pool->queue[pool->tail].argument = argument;

		pool->tail = next;
		pool->count += 1;

		if(pthread_cond_signal(&(pool->notify)) != 0)
		{
			err = -2;
			break;
		}

	} while(0);

	if(pthread_mutex_unlock(&(pool->lock)) != 0)
		return -2;

	return err;
}

int threadpool_destroy(threadpool_t *pool, int flags)
{
	int err = 0;

	if(pool == NULL)
		return -1;

	if(pthread_mutex_lock(&(pool->lock)) != 0)
		return -2;

	do
	{
		if(pool->shutdown)
		{
			err = -4;
			break;
		}

		pool->shutdown = 1;

		if((pthread_cond_broadcast(&(pool->notify)) != 0) || (pthread_mutex_unlock(&(pool->lock)) != 0))
		{
			err = -2;
			break;
		}

		for(unsigned int i = 0; i < pool->thread_count; ++i)
			if(pthread_join(pool->threads[i], NULL) != 0)
				err = -5;	
	} while(0);

	if(!err)
		threadpool_free(pool);

	return err;
}

int threadpool_free(threadpool_t *pool)
{
	if(pool == NULL || pool->started > 0)
		return -1;

	if(pool->threads)
	{
		free(pool->thrads);
		free(pool->queue);

		pthread_mutex_lock(&(pool->lock));
		pthread_mutex_destroy(&(pool->lock));
		pthread_mutex_destroy(&(pool->notify));
	}

	free(pool);
	return 0;
}

static void *threadpool_tread(void *threadpool)
{
	threadpool_t *pool = (threadpool_t *)threadpool;
	threadpool_task_t task;

	for(;;)
	{
		pthread_mutex_lock(&(pool->lock));

		while((pool->count == 0) && (!pool->shutdown))
			pthread_cond_wait(&(pool->notify), &(pool->lock));

		task.function = pool->queue[pool->head].function;
		task.argument = pool->queue[pool->head].argument;

		pool->head += 1;
		pool->head = (pool->head == pool->queue_size) ? 0 : pool->head;
		pool->count -= 1;

		pthread_mutex_unlock(&(pool->lock));
	}

	--pool->started;

	pthread_mutex_unlock(&(pool->lock));
	pthread_exit(NULL);

	return NULL;
}