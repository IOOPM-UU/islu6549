#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hash_table.h"
#include "common.h"
#include "hash_table_struct.h"

static int bucket_size(entry_t *entry)
{
  int size = 0;

  while (entry != NULL)
  {
    size++;
    entry = entry->next;
  }

  return size;
}

static size_t string_to_hash(const char *str);
static entry_t *entry_create(elem_t key, elem_t value, entry_t *next);
entry_t *find_previous_entry(ioopm_hash_table_t *ht, elem_t key);
void entry_destroy(entry_t *entry);

size_t ioopm_string_hash(elem_t key) {
  return string_to_hash(key.s);
}

bool ioopm_string_eq(elem_t a, elem_t b) {
  return strcmp(a.s, b.s) == 0;
}

static size_t string_to_hash(const char *str)
{
  size_t result = 0;
  while (*str != '\0')
  {
    result = result * 31 + ((unsigned char) *str);
    str++;
  }
  return result;
}

/// @brief Create a new hash table
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(
  ioopm_hash_function *hash_fn,
  ioopm_eq_function *key_eq_fn) {

  /// Allocate zeroed-out space for a ioopm_hash_table_t = 17 pointers to entry_t's
  ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));
  
  ht->hash_fn = hash_fn;
  ht->key_eq_fn = key_eq_fn;

  return ht;
}

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht){
  for(int i = 0; i < No_Buckets; i++) {
    entry_t *previous = &ht->buckets[i];
    entry_t *current = previous->next;
    while (current != NULL)
    {
      previous = current;
      current = current->next;
      free(previous);
    }
  }
  free(ht);
}


/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value){
  // find bucket
  entry_t *previous = find_previous_entry(ht, key);

  // if the key exists, update the value, otherwise, add a new entry to the end of the list
  if (previous->next != NULL)
  {
    previous->next->value = value;
  }
  else
  {
    previous->next = entry_create(key, value, NULL);
  }
}

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @return the value mapped to by key (FIXME: what if the key does not exist?)
elem_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key) {
  entry_t *previous = find_previous_entry(ht, key);

  if (previous->next != NULL) {
    return previous->next->value;
  }
  else {
    return int_elem(0);
  }
}

entry_t *find_previous_entry(ioopm_hash_table_t *ht, elem_t key) {
  size_t bucket = ht->hash_fn(key) % No_Buckets;

  entry_t *previous = &ht->buckets[bucket];
  entry_t *current = previous->next;
  while (current != NULL && !ht->key_eq_fn(current->key, key))
  {
    previous = current;
    current = current->next;
  }
  return previous;
}

//ht->buckets[bucket] = entry_create(key, value, NULL);

static entry_t *entry_create(elem_t key, elem_t value, entry_t *next)
{
  // TODO: Stub
  entry_t *item = malloc(sizeof(entry_t));

  item->key = key;
  item->value = value;
  item->next = next;

  return item;
}

// TODO: documentation
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key) {

  entry_t *previous = find_previous_entry(ht, key);

  if (previous->next != NULL) {
    return true;
  } else {
    return false;
  }
}

// TODO: documentation
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht) {

  int size = 0;

  for (int i = 0; i < No_Buckets; i++) {
  size += bucket_size(ht->buckets[i].next);
  }

  return size;
}

// TODO: documentation
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht) {

  int size = ioopm_hash_table_size(ht);

  if (size == 0) {
    return true;
  } else {
    return false;
  }
}

void entry_destroy(entry_t *entry) {
  free(entry);
}

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return the value mapped to by key (FIXME: what if the key does not exist?)
elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key){
  entry_t *previous = find_previous_entry(ht, key);
  if (previous->next != NULL) {

    entry_t *new_current_next = previous->next->next;
    elem_t old_current_next_value = previous->next->value;

    free(previous->next);
    previous->next = new_current_next;

    return old_current_next_value;

  } else {

    printf("key not found");

    return int_elem(0);
  }
}