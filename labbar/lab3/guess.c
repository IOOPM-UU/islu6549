#include <time.h>
#include "../lib/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

int main(void) {
    srand(time(NULL));

    int rand_n = rand() % 20;

    char *user_name = ask_question_string("What is your name?");
    char q[255];
    snprintf(q, sizeof(q), "Hey you, %s,I am thinking of a number. Guess!", user_name);

    int max_tries = 15;
    for (int tries = 0; tries < max_tries; tries++) {
        int answer = ask_question_int(q);
        
        if (answer == rand_n) {
            printf("Bingo!\n");
            printf("I took %s %d guesses to get %d\n", user_name, tries, rand_n);
            return 0;
        } else {
            if (answer < rand_n) {
                printf("Too small!\n");
            } else {
                printf("Too big!\n");
            }
        }
    }

    printf("You no more guesses. The number was %d\n", rand_n);

    return 0;
}