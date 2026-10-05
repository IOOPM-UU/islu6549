#pragma once

#include "hash_table.h"

#define No_Buckets 17

typedef struct entry entry_t;

struct entry
{
    char *key;
    int value;
    entry_t *next;
};

struct hash_table
{
    entry_t buckets[No_Buckets];
};