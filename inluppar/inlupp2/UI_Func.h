/**
 * @file UI_Func.h
 * @brief Contains declarations for the store management system.
 *
 * Handles merchandise management, stock management, shopping carts,
 * checkout operations and store cleanup.
 */

#ifndef UI_FUNC_H
#define UI_FUNC_H

#include <stdbool.h>
#include "data_structure.h"

/**
 * @brief Initializes the merchandise hash table.
 *
 * Sets all buckets to empty and assigns the hash and equality functions.
 *
 * @param ht Pointer to the hash table that gets initialized.
 */
void ioopm_hash_table_init(ioopm_hash_table_t *ht);

/**
 * @brief Adds one new merchandise item to the store without adding anything to a warehouse.
 *
 * Creates a merchandise entry containing its name, description, price
 * and a list for stock and respective warehouse locations. Duplicates can not be added.
 *
 * @param ht Pointer to the merchandise hash table.
 * @param name Name of the merchandise.
 * @param description Description of the merchandise.
 * @param price Price of the merchandise.
 * @param locs Linked list containing the shelves where the merchandise is stored.
 */
void ioopm_merch_add(ioopm_hash_table_t *ht, char *name,
                     char *description, int price, ioopm_list_t *locs);

/**
 * @brief Removes merchandise from the store.
 *
 * Removes the merchandise entry and frees its data and shelf locations.
 *
 * @param ht Pointer to the merchandise hash table.
 * @param name Name of the merchandise to remove.
 */
void merch_remove(ioopm_hash_table_t *ht, char *name);

/**
 * @brief Edits the information of existing merchandise.
 *
 * Updates the name, description and price of a merchandise item.
 * If the name changes, references in shopping carts
 * are updated as well.
 *
 * @param ht Pointer to the merchandise hash table.
 * @param old_name Old (current) name of the merchandise.
 * @param new_name New name of the merchandise.
 * @param new_description New description of the merchandise.
 * @param new_price New price of the merchandise.
 */
void merch_edit(ioopm_hash_table_t *ht, char *old_name,
                char *new_name, char *new_description, int new_price);

/**
 * @brief Displays all merchandise in the store.
 *
 * Prints merchandise names in alphabetical order, displaying up to
 * 20 names at a time and allowing the user to continue to the next page or stop.
 *
 * @param ht Pointer to the merchandise hash table.
 */
void merch_display(ioopm_hash_table_t *ht);

/**
 * @brief Displays the stock of a merchandise item.
 *
 * Prints the shelves containing the desired merchandise and their
 * respective quantities, sorted alphabetically by their aisle name.
 *
 * @param ht Pointer to the merchandise hash table.
 * @param name Name of the merchandise to display.
 */
void ioopm_stock_show(ioopm_hash_table_t *ht, char *name);

/**
 * @brief Replenishes the stock of a merchandise item.
 *
 * Adds the desired quantity to an existing shelf or creates a new
 * shelf if the merchandise is not already stored in that aisle.
 *
 * @param ht Pointer to the merchandise hash table.
 * @param name Name of the merchandise to replenish.
 * @param aisle Aisle where the merchandise should be stored.
 * @param amount Quantity to add to the stock.
 */
void ioopm_stock_replenish(ioopm_hash_table_t *ht, char *name,
                           char *aisle, int amount);

/**
 * @brief Creates a new shopping cart.
 *
 * Assigns the cart a unique ID and adds it to the list of existing carts.
 */
void ioopm_cart_create(void);

/**
 * @brief Removes a shopping cart.
 *
 * Removes the cart with the specified ID and frees all its items.
 *
 * @param cart_id ID of the cart to remove.
 */
void ioopm_cart_remove(int cart_id);

/**
 * @brief Adds merchandise to a shopping cart.
 *
 * Adds the desired quantity of merchandise to the cart.
 * If the merchandise already exists in the cart, the quantity is increased.
 * The total quantity spread across all carts can not exceed available stock.
 *
 * @param ht Pointer to the merchandise hash table.
 * @param cart_id ID of the cart to add merchandise to.
 * @param name Name of the merchandise to add.
 * @param quantity Quantity of merchandise to add.
 */
void ioopm_cart_add_item(ioopm_hash_table_t *ht, int cart_id,
                         char *name, int quantity);

/**
 * @brief Removes merchandise from a shopping cart.
 *
 * Decreases the quantity of the specified merchandise.
 * If the quantity reaches zero, the merchandise is removed from the cart.
 *
 * @param cart_id ID of the cart.
 * @param name Name of the merchandise to remove.
 * @param quantity Quantity of merchandise to remove.
 */
void ioopm_cart_remove_item(int cart_id, char *name, int quantity);

/**
 * @brief Calculates the total cost of a shopping cart.
 *
 * Calculates the total price of all merchandise in the specified cart
 * and prints the resulting amount.
 *
 * @param ht Pointer to the merchandise hash table.
 * @param cart_id ID of the cart to calculate the cost for.
 */
void ioopm_calculate_cost(ioopm_hash_table_t *ht, int cart_id);

/**
 * @brief Completes the checkout process for a shopping cart.
 *
 * Checks that all merchandise exists and that enough of the stock is available.
 * If it succeeds, the stock is reduced by the amount that is checked out and the cart
 * is removed.
 *
 * @param ht Pointer to the merchandise hash table.
 * @param cart_id ID of the cart to check out.
 */
void ioopm_checkout(ioopm_hash_table_t *ht, int cart_id);

/**
 * @brief Undoes the most recent program operation.
 *
 * is not implemented
 */
void ioopm_program_undo(void);

/**
 * @brief Frees all meemory and resources used by the store.
 *
 * Removes all shopping carts, and their contents. Frees all merchandise
 * and their shelf data, and terminates the program.
 *
 * @param ht Pointer to the merchandise hash table.
 */
void ioopm_program_exit(ioopm_hash_table_t *ht);

/**
 * @brief Handles file persistence for the store.
 *
 * is not implemented
 */
void ioopm_file_persistance(void);

#endif
