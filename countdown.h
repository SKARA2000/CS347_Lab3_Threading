#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

typedef struct __t_arg_t {
	int t_arg_1;
	void *t_arg_2;
} t_arg;

typedef struct  __sync{
	int flag;
	int count;
	int n_threads2;
	pthread_cond_t cond;
	pthread_mutex_t lock;
} synch;

typedef struct __mythread{
	pthread_t *thread;
	synch sync_t;
} thread_sync;

typedef struct __myarg_t {
	int tid;
	synch* sync_t;
	void *args;
	void *(*routine)(void *);
} my_arg;

void selective_barrier_wait(synch *sync_t){
	pthread_mutex_lock(&(sync_t->lock));
	while(sync_t->flag == 0){
		// printf("%d %d\n", sync_t->count, sync_t->n_threads);
		pthread_cond_signal(&(sync_t->cond));
		pthread_cond_wait(&(sync_t->cond), &(sync_t->lock));
	}
	pthread_cond_signal(&(sync_t->cond));
	pthread_mutex_unlock(&(sync_t->lock));
}

void selective_barrier_signal(synch *sync_t){
	pthread_mutex_lock(&(sync_t->lock));
	(sync_t->count)++;
	if(sync_t->count == sync_t->n_threads2){
		sync_t->flag = 1;	
	}
	pthread_cond_signal(&(sync_t->cond));
	pthread_mutex_unlock(&(sync_t->lock));
}

void initialize_synch(synch* sync_t, int n_threads2){
	sync_t->flag = 0;
	sync_t->count = 0;
	sync_t->n_threads2 = n_threads2;
	int lc = pthread_mutex_init(&(sync_t->lock), NULL);
	assert(lc == 0);
	lc = pthread_cond_init(&(sync_t->cond), NULL);
	assert(lc==0);
}

void th_create(thread_sync *threads, my_arg my_args[], int n_threads, int n_threads2){
	initialize_synch(&(threads->sync_t), n_threads2);
	threads->thread = (pthread_t *)malloc(n_threads*sizeof(pthread_t));
	for(int i=0; i<n_threads; i++){
		my_args[i].sync_t = &(threads->sync_t);
		my_args[i].tid = i;
 		pthread_create((threads->thread + i), NULL, my_args[i].routine, &my_args[i]);
	}
}

void th_join(thread_sync *threads, int n_threads){
	for(int i=0; i<n_threads; i++){
 		pthread_join(*(threads->thread + i), NULL);
 		printf("Joined thread %d\n", i);
	}	
	free(threads->thread);
}