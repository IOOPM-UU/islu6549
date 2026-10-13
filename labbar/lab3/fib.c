#include <stdlib.h>
#include <stdio.h>

int fib(int n);

int main(int argc, char *argv[]) {
    
    printf("%d\n", fib(10));   

    return 0;
}

int fib(int n) {
    return n == 0 
              ? 0 
              : n == 1 
              ? 1 
              : fib(n - 1) + fib(n - 2);
}

