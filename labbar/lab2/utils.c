#include "utils.h"
#include <stdio.h>
#include <string.h>

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