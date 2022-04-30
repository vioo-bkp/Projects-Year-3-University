/*
 * Tema 2 ASC
 * 2021 Spring
 */
#include "utils.h"

/* Unoptimized implementation */
double * my_solver(int N, double * A, double * B) {
    printf("NEOPT SOLVER\n");
    // we are calculating C = B * (A * A transpose) + (B transpose * B)
    // A - superior triangular matrix
    double * BtB = (double * ) malloc(N * N * sizeof(double));
    double * AAt = (double * ) malloc(N * N * sizeof(double));
    double * BAAt = (double * ) malloc(N * N * sizeof(double));
    double * C = (double * ) malloc(N * N * sizeof(double));

    int i, j, k;

    // BtB = B transpose * B
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            BtB[i * N + j] = 0;
            for (k = 0; k < N; k++) {
                BtB[i * N + j] += B[k * N + i] * B[k * N + j];
            }
        }
    }

    // AAt = A * A transpose
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            AAt[i * N + j] = 0;
            for (k = 0; k < N; k++) {
                AAt[i * N + j] += A[i * N + k] * A[j * N + k];
            }
        }
    }

    // BAAt = B * AAt
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            BAAt[i * N + j] = 0;
            for (k = 0; k < N; k++) {
                BAAt[i * N + j] += B[i * N + k] * AAt[k * N + j];
            }
        }
    }

    // C = BAAt + BtB
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            C[i * N + j] = BAAt[i * N + j] + BtB[i * N + j];
        }
    }

    free(BtB);
    free(AAt);
    free(BAAt);
    return C;

}