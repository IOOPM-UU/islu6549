#pragma once

#include "hash_table.h"
#include "common.h"

// Number for the amount of buckets inside of the hash_table files 
#define No_Buckets 17

typedef struct hash_table ioopm_hash_table_t;
typedef struct entry entry_t;

struct entry
{
    elem_t key;
    elem_t value;
    entry_t *next;
};

struct hash_table
{
    entry_t buckets[No_Buckets];

    // Struct typings to handle strcmp and hashing
    // of elem_t keys
    ioopm_hash_function *hash_fn;
    ioopm_eq_function *key_eq_fn;
};