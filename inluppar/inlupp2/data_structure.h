#pragma once

/**
 * @file data_structure.h
 * @brief Contains structs for hash tables and their nodes, as well as linked lists and their nodes.
 *
 */

#include "common.h"

#define NO_BUCKETS 50

typedef struct hash_table ioopm_hash_table_t;
typedef struct entry entry_t;

typedef struct node node_t;
typedef struct list ioopm_list_t;

typedef struct merch ioopm_merch_t;
typedef struct shelf ioopm_shelf_t;

/**
 * @brief Represents a merchandise item.
 *
 * Contains information about a product and its storage locations.
 *
 * @param name Name of the merchandise.
 * @param description Description of the merchandise.
 * @param price Price of one unit.
 * @param locs Linked list containing the merchandise's storage locations.
 */
struct merch {
  char *name;
  char *description;
  int price;
  ioopm_list_t *locs;
};

/**
 * @brief Represents a shelf in a warehouse.
 *
 * Stores the aisle identifier and the quantity of merchandise
 * available at that location.
 */
struct shelf {
  char *aisle;
  int quantity;
};

/**
 * @brief Represents the merchandise hash table.
 *
 * Stores merchandise entries in a fixed-size array of buckets.
 * Uses a hash function to determine where merchandise is stored.
 *
 * @note The table uses linear probing to resolve collisions.
 */
struct hash_table {
  ioopm_merch_t buckets[NO_BUCKETS];
  
  // Struct typings to handle strcmp and hashing
  // of elem_t keys
  ioopm_hash_function *hash_fn;
  ioopm_eq_function *key_eq_fn;
};

/**
 * @brief Represents an entry within a hash table
 *
 * Stores the key and value of an entry.
 * has a pointer to the next entry in a linked list.
 *
 * @note The table uses linear probing to resolve collisions.
 */
struct entry {
  elem_t key;
  elem_t value;
  entry_t *next;
};

/**
 * @brief Represents a singly linked list of shelves.
 *
 * The list maintains pointers to its first and last nodes,
 * as well as the number of nodes it contains.
 *
 * @invariant size represents the number of nodes in the list.
 * @invariant head and tail are NULL when the list is empty.
 */
struct list {
  size_t size;
  node_t *head;
  node_t *tail;
};

/**
 * @brief Represents a node in a linked list.
 *
 * Each node contains a pointer to a shelf and a pointer
 * to the next node in the list.
 *
 * @invariant next is NULL for the last node.
 */
struct node {
  ioopm_shelf_t *element;
  node_t *next;
};




