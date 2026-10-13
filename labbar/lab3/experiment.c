#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../lib/utils.h"

typedef union { 
  int   int_value;
  float float_value;
  char *string_value;
} answer_t;

typedef bool check_func(char *);
typedef answer_t convert_func(char *);

answer_t ask_question(char *question, check_func *right_format, convert_func *convert);
answer_t atoi_wrap(char *str);

/**
int main(void) {
    answer_t user_input = ask_question("Give a number", is_number, atoi_wrap);
        
    printf("%d\n", user_input.int_value);

    return 0;
}*/

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

answer_t atoi_wrap(char *str) {
    return (answer_t)atoi(str);
}
