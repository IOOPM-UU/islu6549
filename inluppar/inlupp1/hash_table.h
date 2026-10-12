#ifndef HASH_TABLE
#define HASH_TABLE
#include <stdbool.h>
#include "common.h"
/**
* @file hash_table.h
* @author Isak Lundback Öst, Olivia Emilsson
* @date 17/9-26
* @brief hash table that maps elem_t keys to elem_t values.
*
* Here typically goes a more extensive explanation of what the header
* defines. Doxygens tags are words preceeded by either a backslash @\
* or by an at symbol @@.
*
*/

typedef struct hash_table ioopm_hash_table_t;

size_t ioopm_string_hash(elem_t key);

bool ioopm_string_eq(elem_t a, elem_t b);

/// @brief Create a new hash table
/// @param hahs_fn function to hash keys
/// @param key_eq_fn function to check if key and key input are equal
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(
    ioopm_hash_function *hash_fn,
    ioopm_eq_function *key_eq_fn);

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @return value the key key contains, or int_elem(0) if not found
elem_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return the value mapped to by key, or a zero if key not found
elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key);

/// @brief looks for key in hash table equal to key parameter value
/// @param ht hash table operated upon
/// @param key key to find
/// @return a boolean confirming if key in hash table exists
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

/// @brief measures the amount entries in the hash table
/// @param ht hash table operated upon
/// @return the total amount of entries in the hash table
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);

/// @brief checks if hash table is empty
/// @param ht hash table operated upon
/// @return a boolean confirming if hash table is empty
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

#endif