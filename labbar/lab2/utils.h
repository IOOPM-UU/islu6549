#include <stdbool.h>

#ifndef __UTILS_H__
#define __UTILS_H__

int read_string(char *buf, int buf_siz);
bool is_number(char *str);
int ask_question_int(char *question);
char *ask_question_string(char *question);

#endif