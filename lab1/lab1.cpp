#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>


double run(int n, int threads, bool output)
{
	double start_time = omp_get_wtime();
	
	int i, j, thread_num;

	omp_set_num_threads(threads);

	// init matrix
	double a[n][n];
	#pragma omp parallel shared(n, threads), private(i, j, thread_num)
	{
		thread_num = omp_get_thread_num();
		i = 0;

		while (i < n){
			j = thread_num;
			while (j < n){
				a[i][j] = sin(i) + cos(j);
				j += threads;
			}
			i++;
		}
	}

	if (output){
		printf("Matrix A\n\n");
		for(i = 0; i < n; i++){
			for(j = 0; j < n; j++){
				printf("%f  ", a[i][j]);
			}
			printf("\n");
		}
		printf("\n\n");
	}

	double b[n];
	#pragma omp parallel shared(n, threads), private(i, thread_num)
	{
		thread_num = omp_get_thread_num();
		i = thread_num;

		while (i < n){
			b[i] = sin(i) * cos(i);
			i += threads;
		}
	}

	if (output){
		printf("Vector B\n\n");
		for(i = 0; i < n; i++){
			printf("%f  ", b[i]);
		}
		printf("\n\n");
	}

	double c[n];
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

	if (output){
		printf("Result C\n\n");
		for(i = 0; i < n; i++){
			printf("%f  ", c[i]);
		}
		printf("\n\n");
	}

	return omp_get_wtime() - start_time;;
}


int main(int argc, char *argv[])
{
	int max_threads = omp_get_max_threads();
	for(int i = 1; i <= 1000; i++){
		printf("%d ", i);
		for(int j = 1; j <= max_threads; j++){
			printf("%f ", run(i, j, false));
		}
		printf("\n");

	}
}