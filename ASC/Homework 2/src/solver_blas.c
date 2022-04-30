/*
 * Tema 2 ASC
 * 2022 Spring
 */
#include "utils.h"
#include "cblas.h"
/* 
 * Add your BLAS implementation here
 */
double * my_solver(int N, double * A, double * B) {
    printf("BLAS SOLVER\n");
    // we are calculating C = B * (A * A transpose) + (B transpose * B)
    double * BtB = (double * ) malloc(N * N * sizeof(double));
    double * AAt = (double * ) malloc(N * N * sizeof(double));
    double * C = (double * ) malloc(N * N * sizeof(double));
    int i, j;

    // BtB = B transpose * B
    cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, N, N, N, 1.0, B, N, B, N, 0.0, BtB, N);

    // AAt = A * A transpose
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, N, N, N, 1.0, A, N, A, N, 0.0, AAt, N);

    // B * AAt
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, B, N, AAt, N, 0.0, C, N);

    // C = (B * AAt) + BtB
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            C[i * N + j] += BtB[i * N + j];
        }
    }

    free(BtB);
    free(AAt);
    return C;

}