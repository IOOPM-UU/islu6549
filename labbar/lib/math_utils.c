#include "math_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


bool is_number(char *str) {
        int length = strlen(str);
        bool number = true;

        for (int i = 0; i < length; i++) {
                printf("Char at %d: %s\n", i, (isdigit(str[i]) == 0 ? "false" : "true"));
                if (isdigit(str[i]) != false) {
                        continue;
                } else {
                        number = false;
                        break;
                }
        }

        return number;
}


int fib(int n) {
    return n == 0 
              ? 0 
              : n == 1 
              ? 1 
              : fib(n - 1) + fib(n - 2);
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


