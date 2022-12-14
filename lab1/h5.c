#include "lab1.h"

int loop1() {
    float A[101];
    float B[101];
    float C[101];
    float D[101];
    float AA[101];
    float BB[101];
    float CC[101];
    float DD[101];

    memcpy(AA, A, sizeof(A));
    memcpy(BB, B, sizeof(B));
    memcpy(CC, C, sizeof(C));
    memcpy(DD, D, sizeof(D));

    for (int i = 1; i <= 100; i++) {
        B[i] = C[i-1] * 2;
        C[i] = 1 / B[i];
    }

    #pragma omp parallel for
    for (int i = 1; i <= 100; i++) {
        A[i] = A[i] + B[i-1];
        D[i] = C[i] * C[i];
    }

    for (int i = 1; i <= 100; i++) {
        AA[i] = AA[i] + BB[i-1];
        BB[i] = CC[i-1] * 2;
        CC[i] = 1 / BB[i];
        DD[i] = CC[i] * CC[i];
    }
    
    return memeq(A, AA) && memeq(B, BB) && memeq(C, CC) && memeq(D, DD);
}

int loop2() {
    int A[1001];
    int B[1001];
    int C[1001];
    int D[1001];
    int AA[1001];
    int DD[1001];

    memcpy(AA, A, sizeof(A));
    memcpy(DD, D, sizeof(D));

    #pragma omp parallel for
    for (int i = 1; i <= 500; i++) {
        A[i] = B[i] + C[i];
        D[i] = (A[i] + A[1000-i]) / 2;
    }
    #pragma omp parallel for
    for (int i = 501; i <= 999; i++) {
        A[i] = B[i] + C[i];
        D[i] = (A[i] + A[1000-i]) / 2;
    }

    for (int i = 1; i <= 999; i++) {
        AA[i] = B[i] + C[i];
        DD[i] = (AA[i] + AA[1000-i]) / 2;
    }

    return memeq(A, AA) && memeq(D, DD);
}

int loop3() {
    int A[501][201];
    int C[101][101];
    int D[101][101];
    int AA[501][201];
    int DD[101][101];

    memcpy(AA, A, sizeof(A));
    memcpy(DD, D, sizeof(D));

    #pragma omp parallel for
    for (int i = 1; i <= 100; i++) {
        #pragma omp parallel for
        for (int j = 1; j <= 100; j++) {
            A[3*i+2*j][2*j] = C[i][j] * 2;
            D[i][j] = A[i-j+99][i+j];
        }
    }

    for (int i = 1; i <= 100; i++) {
        for (int j = 1; j <= 100; j++) {
            AA[3*i+2*j][2*j] = C[i][j] * 2;
            DD[i][j] = AA[i-j+99][i+j];
        }
    }

    return memeq(A, AA) && memeq(D, DD);
}

int main() {
    omp_set_num_threads(16);
    test(loop1);
    test(loop2);
    test(loop3);
    return 0;
}