#include "countdown.h"

void *myfunc1(void *args){
	my_arg *m_args = (my_arg*)(args);
	int tid = m_args->tid;
	int *myfunc_args = (int *)(m_args->args);
	printf("Hello...you entered thread %d with argument %d\n", tid, *myfunc_args);
	printf("Now we enter a barrier for thread %d\n", tid);
	selective_barrier_wait(m_args->sync_t);
	printf("Barrier crossed for thread %d\n", tid);
	return NULL;
}

void *myfunc2(void *args){
	my_arg *m_args = (my_arg*)(args);
	int tid = m_args->tid;
	int *myfunc_args = (int *)(m_args->args);
	printf("Hello...you entered thread %d with argument %d\n", tid, *myfunc_args);
	printf("Now we enter operation zone in thread %d\n", tid);
	for(int i=0; i<20000; ){
		i++;
	}
	printf("Operation complete for thread %d\n", tid);
	selective_barrier_signal(m_args->sync_t);
	return NULL;
}

int main(){
	int n_threads = 8;
	int n_threads2 = 0;
	thread_sync threads;
	my_arg my_args[n_threads];
	int arg_vec[n_threads];
	for(int i=0; i<n_threads; i++){
		arg_vec[i] = i*i;
		my_args[i].args = &arg_vec[i];
		if(i%2){
			my_args[i].routine = myfunc1;	
		}else{
			my_args[i].routine = myfunc2;
			n_threads2++;
		}
	}
	th_create(&threads, my_args, n_threads, n_threads2);
	th_join(&threads, n_threads);

	return 0;
}