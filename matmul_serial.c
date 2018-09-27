/*
 *
 * This is a tiled matrix multiply program which will compute the product
 *
 *                C  = A * B
 *
 * A ,B and C are both square matrix. They are statically allocated and
 * initialized with constant number.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <unistd.h>

#define ORDER 1024   // the order of the matrix: you can assume power of two
#define AVAL  3.0    // initial value of A
#define BVAL  5.0    // initial value of B
#define TOL   0.001  // tolerance used to check the result
#define MAXBLOCK 128 //maximum blocking factor to be checked
#define MIN(a,b) (((a)<(b))?(a):(b)) //function to calculate min value

#define N ORDER
#define P ORDER
#define M ORDER

int BFAC;

double A[N][P];
double B[P][M];
double C[N][M];

// Initialize the matrices (uniform values to make an easier check)
void matrix_init(void) {
	int i, j;

	// A[N][P] -- Matrix A
	for (i=0; i<N; i++) {
		for (j=0; j<P; j++) {
			A[i][j] = AVAL;
		}
	}

	// B[P][M] -- Matrix B
	for (i=0; i<P; i++) {
		for (j=0; j<M; j++) {
			B[i][j] = BVAL;
		}
	}

	// C[N][M] -- result matrix for AB
	for (i=0; i<N; i++) {
		for (j=0; j<M; j++) {
			C[i][j] = 0.0;
		}
	}
}

// The actual mulitplication function, totally naive
double matrix_multiply(void) {
	int i, j, k;
	double start, end;

	// timer for the start of the computation
	// If you do any dynamic reorganization, 
	// do it before you start the timer
	// the timer value is captured.
	start = omp_get_wtime(); 
	for(i = 0; i<N; i++){
		for(j = 0; j<M; j++){
			for(k=0; k<P;k++){
				C[i][j] +=A[i][k]* B[k][j];
			}
		}
	}
	// timer for the end of the computation
	end = omp_get_wtime();
	// return the amount of high resolution time spent
	return end - start;
}

//Need to investigate further optimizations of this method
double tiled_matrix_multiply(void) {
	int i, j, k, kk, jj, r;
	double start, end;
	
	// timer for the start of the computation
	// If you do any dynamic reorganization, 
	// do it before you start the timer
	// the timer value is captured.

	
	start = omp_get_wtime(); 
	
	
	for(kk = 0; kk < N; kk+=BFAC){
		for(jj = 0; jj < N; jj+=BFAC){	
			for (i = 0; i < N; i++){
				for (j = kk; j < MIN(kk+BFAC,N); j++){
					for(k = jj; k < MIN(jj+BFAC,N); k++){
						C[i][k] += A[i][k] * B[k][j];
				        }
				}
			}
		}
	}
	// timer for the end of the computation
	end = omp_get_wtime();
	// return the amount of high resolution time spent
	return end - start;
}


int variable_block(int j){
	if(j%4 == 0 && j<=MAXBLOCK) {
		BFAC = j;
		return 0;
	}
	return -1;
}	

// Function to check the result, relies on all values in each initial
// matrix being the same
int check_result(void) {
	int i, j;

	double e  = 0.0;
	double ee = 0.0;
	double v  = AVAL * BVAL * ORDER;

	for (i=0; i<N; i++) {
		for (j=0; j<M; j++) {
			e = C[i][j] - v;
			
			ee += e * e;
		}
	}	
	if (ee > TOL) {
		return 0;
	} else {
		return 1;
	}
}

void printResult(double arr[][N]){
	size_t i, j;
	for(i = 0; i < N; ++i){
		for(j = 0; j < N ; ++j){
			printf("%f ", arr[i][j]);
		}
		puts("");
	}
}

// main function
int main(int argc, char *argv[]) {
	BFAC = atoi(argv[1]);
	int index = 4;
	char input[2];		
	int correct;
	double run_time;
	double mflops;
	// initialize the matrices
	matrix_init();
	run_time = tiled_matrix_multiply();
	// multiply and capture the runtime	
	// verify that the result is sensible
	correct  = check_result();		
	// Compute the number of mega flops
	mflops = (2.0 * N * P * M) / (1000000.0 * run_time);
	printf("Order %d multiplication in %f seconds \n", ORDER, run_time);
	printf("Order %d multiplication at %f mflops\nThe blocking size is %d \n", ORDER, mflops, BFAC);
	// Display check results
	if (correct) {
		printf("\n Hey, it worked");
	} else {
		printf("\n Errors in multiplication");
	}
	printf("\n all done \n");
	return 0;
}
