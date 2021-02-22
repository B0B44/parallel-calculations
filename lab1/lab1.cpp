#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

int SAMPLE_COUNT = 10000;
int SAMPLE_COUNT_FOR_AVG = 5;
int RUNS_STEP = 100;
int THREADS_ARRAY[5] = {1,2,4,6};
int THREADS_ARRAY_LENGTH = 4;

double a[10000][10000];
double b[10000];
double c[10000];

double run(int n, int threads, bool output)
{
	double start_time = omp_get_wtime();

	int i, j, thread_num;

	omp_set_num_threads(threads);

	// init matrix & vector
	#pragma omp parallel shared(n, threads), private(i, j, thread_num)
	{
		thread_num = omp_get_thread_num();
		i = 0;

		while (i < n){
			j = thread_num;
			double sin_i = sin(i);
			while (j < n){
				a[i][j] = sin_i + cos(j);
				j += threads;
			}

			if ((thread_num != 0 && i % thread_num == 0) || (i == 0 && thread_num == 0)){
				b[i] = sin_i * cos(i);
			}
			i++;
		}
	}

	double result;
	#pragma omp parallel shared(n, threads), private(i, j, thread_num, result)
	{
		thread_num = omp_get_thread_num();
		i = thread_num;

		while (i < n){
			result = 0;
			for(j = 0; j < n; j++){
				result += a[i][j]*b[j];
			}
			c[i] = result;


			i += threads;
		}
	}

	return omp_get_wtime() - start_time;;
}

double avrg(int runs, int i, int j){
	double results = 0;
	for(int x = 0; x < 10; x++){
		results += run(i, j, false);
	}
	return results / runs;
}

int main(int argc, char *argv[])
{
	int max_threads = omp_get_max_threads();
	
	for(int i = 0; i <= SAMPLE_COUNT; i+=RUNS_STEP){
		printf("%d ", i);

		for(int j = 0; j < THREADS_ARRAY_LENGTH; j++){
			printf("%f ", avrg(SAMPLE_COUNT_FOR_AVG, i, THREADS_ARRAY[j]));
			
		}
		
		printf("\n");

	}
}
