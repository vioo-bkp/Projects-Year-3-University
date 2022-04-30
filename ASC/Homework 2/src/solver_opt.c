/*
 * Tema 2 ASC
 * 2022 Spring
 */
#include "utils.h"

/*
 * Add your optimized implementation here
 */
// C = B * A * A transpose + B transpose * B
double * my_solver(register int N, double * A, double * B) {
    printf("OPT SOLVER\n");

    // we are calculating C = B * (A * A transpose) + (B transpose * B)
    // A - superior triangular matrix
    double * BtB = (double * ) malloc(N * N * sizeof(double));
    double * AAt = (double * ) malloc(N * N * sizeof(double));
    double * BAAt = (double * ) malloc(N * N * sizeof(double));
    double * C = (double * ) malloc(N * N * sizeof(double));

    register int i, j, k;

    // BtB = B transpose * B
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            register double acumulator = 0;
            for (k = 0; k < N; k++) {
                acumulator += B[k * N + i] * B[k * N + j];
            }
            BtB[i * N + j] = acumulator;
        }
    }

    // AAt = A * A transpose
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            register double acumulator = 0;
            for (k = 0; k < N; k++) {
                acumulator += A[i * N + k] * A[j * N + k];
            }
            AAt[i * N + j] = acumulator;
        }
    }

    // BAAt = B * AAt
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            register double acumulator = 0;
            for (k = 0; k < N; k++) {
                acumulator += B[i * N + k] * AAt[k * N + j];
            }
            BAAt[i * N + j] = acumulator;
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