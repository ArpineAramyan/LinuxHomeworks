#define THREAD 32
#define QUEUE 256

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "thread_pool.h"

//int tasks = 0;
pthread_mutex_t lock;

int RandFactorial()
{
	pthread_mutex_lock(&lock);

	int m = rand();
	m %= 20;
	int fact = 1;

	for(int i = m; i > 1; --i)
		fact *= i;

	pthread_mutex_unlock(&lock);

	return fact;
}

int RandAnd()
{
	pthread_mutex_lock(&lock);

	int a = rand();
	a = a & 2021;

	pthread_mutex_unlock(&lock);

	return a;
}

int RandOr()
{
	pthread_mutex_lock(&lock);

	int b = rand();
	b = b | 2022;

	pthread_mutex_unlock(&lock);
	
	return b;

}
int main()
{
	//threadpool_t *pool;

	//pthread_mutex_init(&lock, NULL);

}