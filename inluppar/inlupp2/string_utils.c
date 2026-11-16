#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "string_utils.h"


int string_length(char *str) {
    return str[0] == '\0'
                    ? 0
                    : 1 + string_length(str + 1);
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

    char result[255]; // bufferstorlek
    do {
        printf("%s\n", question);

        if (scanf("%254s", result) == EOF) {
            result[0] = '\0';
            break;
        }

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
        }
    while (conversions < 1 && conversions != EOF);
    
    return result;
}


bool is_float(char *str) {
    int i = 0;
    int digits = 0;
    int dots = 0;

    if (str[i] == '-' || str[i] == '+') {
        i++;
    }

    while (str[i] != '\0') {
        if (isdigit((unsigned char)str[i])) {
            digits++;
        } else if (str[i] == '.' && dots == 0) {
            dots++;
        } else {
            return false;
        }
        i++;
    }

    return digits > 0;
}

answer_t convert_to_answer_t_float(char *str) {
    return (answer_t){ .float_value = (float)atof(str)};
}

float ask_question_float(char *question) {
    answer_t answer = ask_question(question, is_float, convert_to_answer_t_float);
    return answer.float_value;
}

char *ask_question_string(char *question) {

    char result[255]; // bufferstorlek
    do {
        printf("%s\n", question);

        if (scanf("%254s", result) == EOF) {
            break;
        }
    }
    while (strlen(result) == 0);
    return strdup(result);
}

char *ask_question_line(char *question) {
    char result[255];

    printf("%s\n", question);

    do {
        if (fgets(result, sizeof(result), stdin) == NULL) {
            result[0] = '\0';
            break;
        }
        result[strcspn(result, "\n")] = '\0';
    } while (strlen(result) == 0);

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
    char format[16];
    snprintf(format, sizeof(format), "%%%ds", buf_size - 1);
    scanf(format, buf);

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