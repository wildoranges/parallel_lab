#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define min(x,y) ((x) < (y) ? x : y)
#define max(x,y) ((x) > (y) ? x : y)
#define memeq(x,y) (memcmp(x, y, sizeof(x)) == 0)
#define test(a) {if (a()) printf("%s test success\n", (#a)); else printf("%s test fail\n", (#a));}
