#include "utils.h"
#include <stdio.h>
#include <string.h>

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

int string_length(char *str) {
    return str[0] == '\0'
                    ? 0
                    : 1 + string_length(str++);
}

void cool_print(char *str) {
    while (str[0] != '\0') {
        putchar(str[0]);
        str++;
    }
}

void cool_println(char *str) {
    while (str[0] != '\0') {
        putchar(str[0]);
        str++;
    }
    putchar('\n');
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


int ask_question_int(char *question) {

    int result = 0;
    int conversions = 0;
    do {
        printf("%s\n", question);
        conversions = scanf("%d", &result);
        int c;
        do {
            c = getchar();
            }
        while (c != '\n' && c != EOF);
        putchar('\n');
        }
    while (conversions < 1);
    
    return result;
}

answer_t ask_question(char *question, check_func *right_format, convert_func *convert) {

    char result[255]; // <-- bufferstorlek 255
    do {
        printf("%s\n", question);
        scanf("%254s", result); // <-- scanf vet ej bufferstorlek! (inläsning av >254 tecken möjligt!)

        if (!right_format(result)) {
            result[0] = '\0';
        }
    }
    while (strlen(result) == 0);

    return convert(result);
}


char *ask_question_string(char *question) {

    char result[255]; // <-- bufferstorlek 255
    do {
        printf("%s\n", question);
        scanf("%255s", result); // <-- scanf vet ej bufferstorlek! (inläsning av >254 tecken möjligt!)
    }
    while (strlen(result) == 0);
    return strdup(result);
}

int read_string(char *buf, int buf_size) {
    scanf("%s", buf);

    int count = 0;
    for (count = 0; count < buf_size; count++) {
        if (buf[count] != '\0') {
            continue;
        } else {
            break;
        }
    }

    return count;
}
