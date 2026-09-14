#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#ifndef __UTILS_H__
#define __UTILS_H__

typedef union { 
  int   int_value;
  float float_value;
  char *string_value;
} answer_t;

typedef bool check_func(char *);
typedef answer_t convert_func(char *);

int read_string(char *buf, int buf_siz);
bool is_number(char *str);
bool not_empty(char *str);
answer_t ask_question(char *question, check_func *check, convert_func *convert);
char *ask_question_string(char *question);
int ask_question_int(char *question);
typedef int int_fold_func(int, int);
int foldl_int_int(int numbers[], int numbers_size, int_fold_func *f);



int string_length(char *str);


#endif
