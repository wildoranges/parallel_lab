#include "lab1.h"

int loop1() {
    float A[11][11];
    float AA[11][11];

    memcpy(AA, A, sizeof(A));

    for (int i = 3; i <= 19; i++) {
        int x = (i+1) / 2;
        #pragma omp parallel for
        for (int j  = max(2, i-9); j <= x; j++) {
            A[j][i+1-j] = (A[j][i-j] + A[j-1][i+1-j]) * 0.5;
        }
    }

    for (int i = 2; i <= 10; i++) {
        for (int j = i; j <= 10; j++) {
            AA[i][j] = (AA[i][j-1] + AA[i-1][j]) * 0.5;
        }
    }

    return memeq(A, AA);
}

int loop2() {
    int A[20];
    int B[20];
    int AA[20];

    memcpy(AA, A, sizeof(A));

    for (int i = 1; i <= 16; i += 3) {
        int minj = min(i+2, 16);
        #pragma omp parallel for
        for (int j = i; j <= minj; j++) {
            A[j+3] = A[j] + B[j];
        }
    }

    for (int i = 1; i <= 16; i++) {
        AA[i+3] = AA[i] + B[i];
    }

    return memeq(A, AA);
}

int loop3() {
    int A[20];
    int B[20];
    int AA[20];

    memcpy(AA, A, sizeof(A));

    for (int i = 1; i <= 16; i += 3) {
        int minj = min(i+2, 16);
        #pragma omp parallel for
        for (int j = i; j <= minj; j++) {
            A[j+3] = A[j] + B[j];
        }
    }

    for (int k = 1; k <= 16; k+=5) {
        for (int i = k; i <= min(16, k+4); i++) {
            AA[i+3] = AA[i] + B[i];
        }
    } 

    return memeq(A, AA);
}

int main() {
    omp_set_num_threads(16);
    test(loop1);
    test(loop2);
    test(loop3);
    return 0;
}