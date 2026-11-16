#include <stdbool.h>

#ifndef STRING_UTILS
#define STRING_UTILS

/**
 * @brief the resulting types by the input for a question.
 *
 * Only the matching type is read.
 */
typedef union { 
  int   int_value;
  float float_value;
  char *string_value;
} answer_t;

typedef answer_t convert_func(char *);
typedef bool check_func(char *);
typedef int int_fold_func(int, int);

/**
 * @brief Calculates the length of a string rescursively.
 *
 * @param str A null terminated string.
 *
 * @return The number of characters in str, not counting the terminating '\0'.
 */
int string_length(char *str);

/**
 * @brief Prints a string without a newline at the end.
 *
 * @param str A null terminated string.
 */
void cool_print(char *str);

/**
 * @brief Prints a string with a newline at the end.
 *
 * @param str A null terminated string.
 */
void cool_println(char *str);

/**
 * @brief Asks the user for an integer.
 *
 * Repeats the question until the user types an integer. parts of the line that isnt an integer is discarded.
 * 
 * @param question The question to print.
 *
 * @return The integer typed by the user, or 0 if the input ended.
 */
int ask_question_int(char *question);

/**
 * @brief Asks the user for a decimal number.
 *
 * Repeats the question until the answer passes is_float,
 * such as "7", "-4.5" or "+.5".
 *
 * @param question The question to print.
 *
 * @return The number the user typed, or 0.0 if the input ended.
 */
float ask_question_float(char *question);

/**
 * @brief Asks a question and keeps asking until the answer has the right format.
 *
 * Reads one word per attempt.
 *
 * @param question The question to print.
 * @param right_format Decides if an answer is accepted.
 * @param convert Converts an accepted answer to an answer_t.
 *
 * @return The converted answer. If the input ended, the result of
 *         convert applied to the empty string.
 */
answer_t ask_question(char *question, check_func *right_format, convert_func *convert);


/**
 * @brief Asks the user for one word.
 *
 * Only the first word of the answer is used, so "hello world" would become "hello".
 *
 * @param question The question to print.
 *
 * @return A heap allocated copy of the word, or an empty string if the
 *         input ended. The caller has to free it.
 */
char *ask_question_string(char *question);

/**
 * @brief Asks the user for a whole line of text.
 *
 * Spaces are kept and empty lines are skipped. The newline is removed.
 *
 * @param question The question to print.
 *
 * @return A heap allocated copy of the line, or an empty string if the
 *         input ended. The caller has to free it.
 */
char *ask_question_line(char *question);

// UNRELATED TO ASSIGNMENT
char *ask_question_shelf(char *question);

// UNRELATED TO ASSIGNMENT
bool letter_then_numbers(char *str);
answer_t string_to_answer_t(char *str);

// UNRELATED TO ASSIGNMENT
int read_string(char *buf, int buf_size);

#endif
