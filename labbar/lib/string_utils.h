#include <stdbool.h>

#ifndef STRING_UTILS
#define STRING_UTILS

typedef union { 
  int   int_value;
  float float_value;
  char *string_value;
} answer_t;

typedef answer_t convert_func(char *);
typedef bool check_func(char *);
typedef int int_fold_func(int, int);

int string_length(char *str);
void cool_print(char *str);
void cool_println(char *str);
int ask_question_int(char *question);
float ask_question_float(char *question);
answer_t ask_question(char *question, check_func *right_format, convert_func *convert);
char *ask_question_string(char *question);
bool letter_then_numbers(char *str);
answer_t string_to_answer_t(char *str);
char *ask_question_shelf(char *question);
int read_string(char *buf, int buf_size);

#endif
