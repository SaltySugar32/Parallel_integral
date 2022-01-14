#include <omp.h>
#include <stdlib.h> 
#include <stdio.h>
#include <iostream>

#define n 10000000u

#define CACHE_LINE 64u 

typedef struct element_t_
{
	alignas(CACHE_LINE) double value;
} element_t;

typedef float (*f_t) (float);

float g(float x)
{
	return x * x;
}

typedef struct experiment_result_t_
{
	float result;
	double time;
} experiment_result_t;

typedef float (*integrate_t)(float a, float b, f_t f);

experiment_result_t run_experiment(integrate_t integrate)
{
	experiment_result_t result;
	double t0 = omp_get_wtime();
	result.result = integrate(-1, 1, g);
	result.time = omp_get_wtime() - t0;
	return result;
}
void run_experiments(experiment_result_t* results, float(*I)(float a, float b, f_t f)) {
	for (unsigned T = 1; T <= std::thread::hardware_concurrency(); ++T) {
		omp_set_num_threads(T);
		results[T - 1] = run_experiment(I);
	}
}


void print_experiment_result(const char* name, const experiment_result_t* r) {
	printf("\n###### Function %s ######\n", name);
	printf("Thread\tTime\n");
	for (unsigned t = 1; t <= std::thread::hardware_concurrency(); ++t)
		printf("%u\t%g\n", t, r[t - 1].time, r[0].time / r[t - 1].time);
}

