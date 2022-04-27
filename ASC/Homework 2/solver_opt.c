/*
 * Tema 2 ASC
 * 2022 Spring
 */
#include "utils.h"
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
/*
 * Add your optimized implementation here
 */
// C = B * A * A transpose + B transpose * B
double *my_solver(int N, double *A, double *B) {
  printf("OPT SOLVER\n");

  double *BtB = (double *)malloc(N * N * sizeof(double));
  double *AAt = (double *)malloc(N * N * sizeof(double));
  double *BAAt = (double *)malloc(N * N * sizeof(double));

  register int i, j, k;

  // calculate B transpose * B
  for (i = 0; i != N; i++) {
    register double *BtB_i = BtB + i * N;
    for (j = 0; j != N; j++) {
	  register double *B_j = B + j * N;
	  register double sum = 0;
	  for (k = 0; k != N; k++) {
		sum += B_j[k] * B_j[k];
	  }
	  BtB_i[j] = sum;
	}
  }

  // calculate A * A transpose
  for (i = 0; i != N; i++) {
    register double *AAt_i = AAt + i * N;
    register double *A_i = A + i * N;

    for (j = 0; j != N; j++, AAt_i++, A_i++) {
      register double sum;
      for (k = 0; k < MIN(i, j); k++) {
        sum += A[k * N + i] * A[k * N + j];
      }
      *AAt_i += sum;
    }
  }

  // calculate B * A * A transpose
  for (i = 0; i != N; i++) {
    register double *BAAt_i = BAAt + i * N;
    register double *B_i = B + i * N;

    for (j = 0; j != N; j++, BAAt_i++, B_i++) {
      register double sum;
      for (k = 0; k < MIN(i, j); k++) {
        sum += B[k * N + i] * A[k * N + j];
      }
      *BAAt_i += sum;
    }
  }

  // calculate BAAt + BtB
  for (i = 0; i != N; i++) {
    for (j = 0; j != N; j++) {
      BAAt[i * N + j] += BtB[i * N + j];
    }
  }

  free(BtB);
  free(AAt);
  return BAAt;
}
