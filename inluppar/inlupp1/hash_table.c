#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hash_table.h"

#define No_Buckets 17

typedef struct hash_table ioopm_hash_table_t;

typedef struct entry entry_t;

struct entry
{
  char *key;     // holds the key
  int value;     // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
  // DODGE: hard-coding number of buckets as 17.
  // NOTE: addressing this dodge is optional.
  entry_t buckets[No_Buckets];
};

static entry_t *entry_create(char *key, int value, entry_t *next);
entry_t *find_previous_entry(ioopm_hash_table_t *ht, char *key);

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
ioopm_hash_table_t *ioopm_hash_table_create(void){

  /// Allocate zeroed-out space for a ioopm_hash_table_t = 17 pointers to entry_t's
  return calloc(1, sizeof(ioopm_hash_table_t));
 
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
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, char *key, int value){
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
bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, char *key, int* result){
     // find bucket
  entry_t *previous = find_previous_entry(ht, key);

  // if the key exists, return the value, otherwise, indicate that the lookup failed
  if (previous->next != NULL)
  {
    *result = previous->next->value;
    return true;
  }
  else
  {
    return false;
  }
}

entry_t *find_previous_entry(ioopm_hash_table_t *ht, char *key) {
  size_t bucket = string_to_hash(key) % No_Buckets;

  entry_t *previous = &ht->buckets[bucket];
  entry_t *current = previous->next;
  while (current != NULL && strcmp(current->key, key) != 0)
  {
    previous = current;
    current = current->next;
  }
  return previous;
}

//ht->buckets[bucket] = entry_create(key, value, NULL);

static entry_t *entry_create(char *key, int value, entry_t *next)
{
  // TODO: Stub
  entry_t *item = malloc(sizeof(entry_t));

  item->key = key;
  item->value = value;
  item->next = next;

  return item;
}

void entry_destroy(entry_t *entry) {
  free(entry);
}

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return the value mapped to by key (FIXME: what if the key does not exist?)
int ioopm_hash_table_remove(ioopm_hash_table_t *ht, char *key){
  entry_t *previous = find_previous_entry(ht, key);
  if (previous->next != NULL) {

    entry_t *new_current_next = previous->next->next;
    int old_current_next_value = previous->next->value;

    free(previous->next);
    previous->next = new_current_next;

    return old_current_next_value;

  } else {

    printf("key not found");

    return 0;

  }
}