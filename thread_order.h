#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

typedef struct __t_arg_t {
	int t_arg_1;
	void *t_arg_2;
} t_arg;

typedef struct  __sync{
	int counter;
	int *id_vec;
	int start;
	pthread_cond_t cond;
	pthread_mutex_t lock;
} synch;

typedef struct __mythread{
	pthread_t *thread;
	synch sync_t;
} thread_sync;

typedef struct __myarg_t {
	void *(*routine)(void *);
	synch* sync_t;
	void *args;
	int tid;
} my_arg;

void *wrapper_func(void *args){
	my_arg *my_args = (my_arg*)(args);
	synch *sync_t = my_args->sync_t;

	pthread_mutex_lock(&(sync_t->lock));
	while((*(sync_t->id_vec + sync_t->counter) != my_args->tid) || (sync_t->start == 0)){
		// printf("%d %d %d\n",sync_t->id_vec[sync_t->counter], my_args -> tid, sync_t->start);
		pthread_cond_signal(&(sync_t->cond));
		pthread_cond_wait(&(sync_t->cond), &(sync_t->lock));
	}

	void *myfunc_arg = my_args-> args;
	void *(*routine)(void *) = my_args->routine;
	t_arg myfunc_targ;
	myfunc_targ.t_arg_1 = my_args->tid;
	myfunc_targ.t_arg_2 = myfunc_arg;
	routine(&myfunc_targ);

	(sync_t->counter)++;
	pthread_cond_signal(&(sync_t->cond));
	pthread_mutex_unlock(&(sync_t->lock));

	return NULL;
}

void initialize_synch(synch* sync_t, int n_threads, int order_vec[]){
	sync_t->counter = 0;
	sync_t->start = 0;
	int lc = pthread_mutex_init(&(sync_t->lock), NULL);
	assert(lc == 0);
	lc = pthread_cond_init(&(sync_t->cond), NULL);
	sync_t->id_vec = order_vec;
}

void exec_my_threads(synch* sync_t){
	pthread_mutex_lock(&(sync_t->lock));
	sync_t->start = 1;
	pthread_cond_signal(&(sync_t->cond));
	printf("Started Execution: %d\n", *(sync_t->id_vec + sync_t->counter));
	pthread_mutex_unlock(&(sync_t->lock));
}

void th_create(thread_sync *threads, my_arg my_args[], int order_vec[], int n_threads){
	initialize_synch(&(threads->sync_t), n_threads, order_vec);
	threads->thread = (pthread_t *)malloc(n_threads*sizeof(pthread_t));
	for(int i=0; i<n_threads; i++){
		my_args[i].sync_t = &(threads->sync_t);
		my_args[i].tid = i;
 		pthread_create((threads->thread + i), NULL, wrapper_func, &my_args[i]);
	}
	exec_my_threads(&(threads->sync_t));
}

void th_join(thread_sync *threads, int order_vec[], int n_threads){
	pthread_join(*(threads->thread + order_vec[n_threads-1]), NULL);
	free(threads->thread);
}
