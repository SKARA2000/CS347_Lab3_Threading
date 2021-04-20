#include "barrier.h"

void *myfunc(void *args){
	my_arg *m_args = (my_arg*)(args);
	int tid = m_args->tid;
	int *myfunc_args = (int *)(m_args->args);

	printf("Hello...you entered thread %d with argument %d\n", 
	tid, *myfunc_args);
	printf("Now we enter a barrier\n");

	barrier_synch(m_args->sync_t);

	printf("Barrier crossed for thread %d\n", tid);

	return NULL;
}

int main(){
	int n_threads = 8;
	thread_sync threads;
	my_arg my_args[n_threads];	

	int arg_vec[8];

	for(int i=0; i<n_threads; i++){
		arg_vec[i] = i*i;
		my_args[i].args = &arg_vec[i];
		my_args[i].routine = myfunc;
	}

	mythread_create(&threads, my_args, n_threads);
	mythread_join(&threads, n_threads);

	return 0;
}