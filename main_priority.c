#include "priority.h"

void *myfunc(void *args){
	t_arg *t_args = (t_arg*)(args);
	int *twarg = (int *)(t_args->t_arg_2);
	printf("Hello...you are in thread %d with argument %d with priority %d\n", 
	t_args->t_arg_1, *twarg, t_args->t_arg_3);
	return NULL;
}

int main(){
	int n_threads = 8;
	thread_sync threads;
	my_arg my_args[n_threads];

	int arg_vec[n_threads];
	int priority_vec[] = {1, 1, 2, 2, 1, 3, 2, 3};

	for(int i=0; i<n_threads; i++){
		arg_vec[i] = i*i;
		my_args[i].routine = myfunc;
		my_args[i].args = &arg_vec[i];
	}

	mythread_create(&threads, my_args, priority_vec, n_threads);
	mythread_join(&threads, n_threads);

	return 0;
}