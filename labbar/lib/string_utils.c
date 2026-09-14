#include "string_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


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
        //putchar('\n');
        }
    while (conversions < 1);
    
    return result;
}


bool is_float(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (isalpha(str[i]) && str[i] != '.') {
            return false;
        } else {
            i++;
        }
    }

    return true;
}

answer_t convert_to_answer_t_float(char *str) {
    return (answer_t){ .float_value = (float)atof(str)};
}

float ask_question_float(char *question) {
    answer_t answer = ask_question(question, is_float, convert_to_answer_t_float);
    return answer.float_value;
}

char *ask_question_string(char *question) {

    char result[255]; // <-- bufferstorlek 255
    do {
        printf("%s\n", question);
        scanf("%254s", result); // <-- scanf vet ej bufferstorlek! (inläsning av >254 tecken möjligt!)
    }
    while (strlen(result) == 0);
    return strdup(result);
}


bool letter_then_numbers(char *str) {
    if (!isalpha(str[0])) {
        return false;
    } else {
        int i = 1;
        while(str[i] != '\0') {
            if (isdigit(str[i])) {
                i++;
            } else {
                return false;
            }
        }
    }

    return true;
}


answer_t string_to_answer_t(char *str) {
    return (answer_t){ .string_value = strdup(str)};
}


char *ask_question_shelf(char *question) {
    answer_t answer = ask_question(question, &letter_then_numbers, string_to_answer_t);
    return answer.string_value;
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

