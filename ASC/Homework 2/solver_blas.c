/*
 * Tema 2 ASC
 * 2022 Spring
 */
#include "utils.h"
#include "cblas.h"
#include <string.h>
/* 
 * Add your BLAS implementation here
 */
double* my_solver(int N, double *A, double *B) {
	printf("BLAS SOLVER\n");
	double *BtB = (double *)malloc(N * N * sizeof(double));
  	double *AAt = (double *)malloc(N * N * sizeof(double));
  	double *BAAt = (double *)malloc(N * N * sizeof(double));

	// calculate B transpose * B using cblas_dgemm
	cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, N, N, N, 1.0, B, N, B, N, 0.0, BtB, N);
	// calculate A * A transpose using cblas_dgemm
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, N, N, N, 1.0, A, N, A, N, 0.0, AAt, N);
	// calculate B * A * A transpose using cblas_dgemm
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, B, N, AAt, N, 0.0, BAAt, N);

	// calculate B * A * A transpose + B transpose * B using cblas_daxpy
	cblas_daxpy(N * N, 1.0, BAAt, 1, BtB, 1);

	free(BAAt);
	free(AAt);
	return BtB;
}
