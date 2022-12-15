#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#define ID(i, j, n) ((i) * (n) + (j)) 

void rand_mat(int *mat, int n) {
    srand(clock());
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            mat[ID(i, j, n)] = rand() % 10;
        }
    }
}

void print_mat(int *mat, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d\t", mat[ID(i, j, n)]);
        }
        printf("\n");
    }
}

int mat_eq(int *mat1, int *mat2, int n) {
    return memcmp(mat1, mat2, sizeof(int) * n * n) == 0;
}

void rand_mat_double(double *mat, int n) {
    srand(clock());
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            mat[ID(i, j, n)] = rand() % 10;
        }
    }
}

void print_mat_double(double *mat, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%lf\t", mat[ID(i, j, n)]);
        }
        printf("\n");
    }
}