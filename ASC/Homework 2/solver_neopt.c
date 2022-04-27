/*
 * Tema 2 ASC
 * 2021 Spring
 */
#include "utils.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
/* Unoptimized implementation */
double *my_solver(int N, double *A, double *B) {
  printf("NEOPT SOLVER\n");
  double *BtB = (double *)malloc(N * N * sizeof(double));
  double *AAt = (double *)malloc(N * N * sizeof(double));
  double *BAAt = (double *)malloc(N * N * sizeof(double));
  int i, j, k;
  // calculate B transpose * B
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      BtB[i * N + j] = 0;
      for (k = 0; k < MIN(i, j); k++) {
        BtB[i * N + j] += B[i * N + k] * B[j * N + k];
      }
    }
  }
  // calculate A * A transpose
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      AAt[i * N + j] = 0;
      for (k = 0; k < N; k++) {
        AAt[i * N + j] += A[i * N + k] * A[j * N + k];
      }
    }
  }
  // calculate B * A * A transpose
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      BAAt[i * N + j] = 0;
      for (k = 0; k < N; k++) {
        BAAt[i * N + j] += B[i * N + k] * AAt[k * N + j];
      }
    }
  }
  // calculate BAAt + BtB
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      BAAt[i * N + j] += BtB[i * N + j];
    }
  }
  free(BtB);
  free(AAt);
  return BAAt;
}
