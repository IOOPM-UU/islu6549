#include <stdbool.h>

#ifndef MATH_UTILS
#define MATH_UTILS

typedef int int_fold_func(int, int);

bool is_number(char *str);
int fib(int n);
int foldl_int_int(int numbers[], int numbers_size, int_fold_func *f);
long sum(int numbers[], int numbers_size);

#endif
