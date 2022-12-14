#include "lab1.h"

int loop1() {
    float A[12][12];
    float B[12][12];

    memcpy(B, A, sizeof(A));

    for(int i = 2; i <= 10; i++) {
        #pragma omp parallel for num_threads(9)
        for (int j = 2; j <= 10; j++) {
            A[i][j] = 0.5 * (A[i-1][j-1] + A[i+1][j+1]);
        }
    }

    for(int i = 2; i <= 10; i++) {
        for (int j = 2; j <= 10; j++) {
            B[i][j] = 0.5 * (B[i-1][j-1] + B[i+1][j+1]);
        }
    }

    return memeq(A, B);
}

int loop2() {
    int A[43];
    int B[21];
    int AA[43];
    memcpy(AA, A, sizeof(A));

    for(int i = 2; i <= 20; i++) {
        int k = min(i+1, 20);
        #pragma omp parallel for
        for (int j = i; j <= k; j++) {
            A[2*j+2] = A[2*j-2] + B[j];
        }
    }

    for(int i = 2; i <= 20; i++) {
        AA[2*i+2] = AA[2*i-2] + B[i];
    }

    return memeq(A, AA);
}

int loop3() {
    int A[21];
    int B[21];
    int C[21];
    int BB[21];
    int CC[21];
    memcpy(CC, C, sizeof(C));
    memcpy(BB, B, sizeof(B));

    #pragma omp parallel for num_threads(19)
    for (int i = 2; i <= 20; i++) {
        if (A[i] <= 0) {
            C[i] = B[i] - 1;
        }
    }

    #pragma omp parallel for num_threads(19)
    for (int i = 2; i <= 20; i++) {
        if (A[i] > 0) {
            B[i] = C[i-1] + 1;
        }
    }

    for (int i = 2; i <= 20; i++) {
        if (A[i] > 0) {
            BB[i] = CC[i-1] + 1;
        } else {
            CC[i] = BB[i] - 1;
        }
    }

    return memeq(B, BB) && memeq(C, CC);
}

int main() {
    omp_set_num_threads(16);
    test(loop1);
    test(loop2);
    test(loop3);
    return 0;
}