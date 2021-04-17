#include "thread_order.h"

void *myfunc(void *args){
	t_arg *t_args = (t_arg*)(args);
	int *twarg = (int *)(t_args->t_arg_2);
	printf("Hello...you are in thread %d with argument %d\n", t_args->t_arg_1, *twarg);
	return NULL;
}

int main(){
	int n_threads = 8;
	thread_sync threads;
	my_arg my_args[n_threads];

	int arg_vec[n_threads];
	int order_vec[] = {7, 2, 1, 4, 3, 0, 6, 5};

	for(int i=0; i<n_threads; i++){
		arg_vec[i] = i*i;
		my_args[i].routine = myfunc;
		my_args[i].args = &arg_vec[i];
	}

	th_create(&threads, my_args, order_vec, n_threads);
	th_join(&threads, order_vec, n_threads);

	return 0;
}