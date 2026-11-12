
/**
 * @file webstore.h
 * @brief Contains functions for handling the store's user interface.
 *
 * Provides menu input handling and the main event loop through which
 * the user interacts with the store.
 */

#ifndef WEBSTORE_H
#define WEBSTORE_H

#include "data_structure.h"

/**
 * @brief Asks the user a question and retrieves a character answer.
 *
 * Reads a string input from the user, converts the first character
 * to uppercase and returns it.
 *
 * @param question The question to display to the user.
 *
 * @return The first character of the user's answer, converted to uppercase.
 */
char ask_question_menu(char *question);

/**
 * @brief Runs the main event loop of the store.
 *
 * Continuously displays the main menu and handles user choices.
 * Allows the user to interact with merchandise, stock and shopping carts,
 * as well as exit the program.
 *
 * @param ht Pointer to the merchandise hash table used by the store.
 */
void event_loop(ioopm_hash_table_t *ht);

#endif