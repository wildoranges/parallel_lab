#include "lab1.h"

int loop1() {
    const int M = 20;
    const int N = 20;
    const int C = 1;

    int A[M+2][N+2];
    int AA[M+2][N+2];
    memcpy(AA, A, sizeof(A));

    for (int i = 1; i <= M; i++) {
        #pragma omp parallel for num_threads(N)
        for (int j = 1; j <= N; j++) {
            A[i+1][j+1] = A[i][j] + C;
        }
    }

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= N; j++) {
            AA[i+1][j+1] = AA[i][j] + C;
        }
    }

    return memeq(A, AA);
}

int loop2() {
    const int N = 20;
    int X[102];
    int Y[202];
    int A[102][102];
    int B[102];
    int C[102][102];

    int XX[102];
    int YY[202];
    int AA[102][102];
    int BB[102];
    // int CC[102][102];

    memcpy(XX, X, sizeof(X));
    memcpy(YY, Y, sizeof(Y));
    memcpy(AA, A, sizeof(A));
    memcpy(BB, B, sizeof(B));
    // memcpy(CC, C, sizeof(C));

    #pragma omp parallel for
    for (int i = 1; i <= 100; i++) {
        X[i] = Y[i] + 10;
        #pragma omp parallel for 
        for (int j = 1; j <= 100; j++) {
            B[j] = A[j][N];
            #pragma omp parallel for 
            for (int k = 1; k <= 100; k++) {
                A[j+1][k] = B[j] + C[j][k];
            }
            Y[i+j] = A[j+1][N];
        }
    }

    for (int i = 1; i <= 100; i++) {
        XX[i] = YY[i] + 10;
        for (int j = 1; j <= 100; j++) {
            BB[j] = AA[j][N];
            for (int k = 1; k <= 100; k++) {
                AA[j+1][k] = BB[j] + C[j][k];
            }
            YY[i+j] = AA[j+1][N];
        }
    }
    
    return memeq(X, XX) && memeq(Y, YY) && memeq(A, AA) && memeq(B, BB);
}

int main() {
    omp_set_num_threads(16);
    test(loop1);
    test(loop2);
    return 0;
}