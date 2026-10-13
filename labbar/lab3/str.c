#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <../lib/utils.h>

int string_length(char *str);
void cool_print(char *str);
void cool_println(char *str);

int main(int argc, char *argv[]) {
    cool_println(argv[1]);

    /**
    int our_length = string_length(argv[1]);
    int c_length = strlen(argv[1]);

    printf("Are they the same? Answer: %s\n", our_length == c_length ? "true" : "false");
    */
    return 0;
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


