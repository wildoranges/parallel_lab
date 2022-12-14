#include "lab1.h"

int loop1() {
    //不能并行
    return 1;
}

int loop2() {
    int y;
    int x = y * 2;
    int z;
    int A[102];
    int B[102];
    int C[102];
    int D[102][51];

    int AA[102];
    int CC[102];
    int DD[102][51];

    memcpy(AA, A, sizeof(A));
    memcpy(CC, C, sizeof(C));
    memcpy(DD, D, sizeof(D));

    #pragma omp parallel for
    for (int i = 1; i <= 100; i++) {
        C[i] = B[i] + x;
    }
    
    #pragma omp parallel for
    for (int i = 1; i <= 100; i++) {
        A[i] = C[i-1] + z;
        for (int j = 1; j <= 50; j++) {
            D[i][j] = D[i][j-1] + x;
        }
    }
    C[101] = A[100] * B[100];

    for (int i = 1; i <= 100; i++) {
        CC[i] = B[i] + x;
        AA[i] = CC[i-1] + z;
        CC[i+1] = AA[i] * B[i];
        for (int j = 1; j <= 50; j++) {
            DD[i][j] = DD[i][j-1] + x;
        }
    }

    return memeq(A, AA) && memeq(C, CC) && memeq(D, DD);
}

int main() {
    omp_set_num_threads(16);
    test(loop1);
    test(loop2);
    return 0;
}