#include <stdlib.h>
#include <stdio.h>

typedef int int_fold_func(int, int);
int foldl_int_int(int numbers[], int numbers_size, int_fold_func *f);
int add(int a, int b);

long sum(int numbers[], int numbers_siz);


int main(void) {
    int numb[] = {1, 2, 3, 4, 5, 6, 7};
    int res = sum(numb, 7);
    printf("Res: %d\n", res);

    return 0;
}

int foldl_int_int(int numbers[], int numbers_size, int_fold_func *f) {
    int result = 0;

  // Loopa över arrayen och för varje element e utför result = f(result, e)
    for (int i = 0; i < numbers_size; ++i) {
        result = f(result, numbers[i]);
    }

    return result;
}

int add(int a, int b) {
    return a + b;
}

long sum(int numbers[], int numbers_size) {
    int result = foldl_int_int(numbers, numbers_size, add);

    return result;
}
