#include <stdlib.h>
#include "hash_table_struct.h"
#include "common.h"
#include "hash_table_iterator.h"

struct hash_table_iterator
{
    ioopm_hash_table_t *ht;
    size_t current_bucket;
    entry_t *current_entry;
};

ioopm_hash_table_iterator_t *ioopm_hash_table_iterator_create(ioopm_hash_table_t *ht);
void ioopm_hash_table_iterator_destroy(ioopm_hash_table_iterator_t *hti);
static void iterator_state_advance(ioopm_hash_table_iterator_t *hti);
void ioopm_hash_table_iterator_advance(ioopm_hash_table_iterator_t *it);
static void skip_sentinel_nodes(ioopm_hash_table_iterator_t *hti);
elem_t ioopm_hash_table_iterator_current_key(ioopm_hash_table_iterator_t *it);
elem_t ioopm_hash_table_iterator_current_value(ioopm_hash_table_iterator_t *it);
bool ioopm_hash_table_iterator_at_end(ioopm_hash_table_iterator_t *hti);

ioopm_hash_table_iterator_t *ioopm_hash_table_iterator_create(ioopm_hash_table_t *ht)
{
    ioopm_hash_table_iterator_t *hti = malloc(sizeof(ioopm_hash_table_iterator_t));

    hti->ht = ht;
    hti->current_bucket = 0;
    hti->current_entry = &ht->buckets[0];

    skip_sentinel_nodes(hti);

    return hti;
}

void ioopm_hash_table_iterator_destroy(ioopm_hash_table_iterator_t *hti) {

    free(hti);
}

static void iterator_state_advance(ioopm_hash_table_iterator_t *hti)
{
    // advance to the next entry in the bucket
    hti->current_entry = hti->current_entry->next;
    // if it was null advance to the next bucket
    if (hti->current_entry == NULL) {
        hti->current_bucket += 1;
        // if the next bucket existed, update the current entry
        if (hti->current_bucket != NO_BUCKETS) {
            hti->current_entry = &hti->ht->buckets[hti->current_bucket];
        }
    }

    skip_sentinel_nodes(hti);
}

void ioopm_hash_table_iterator_advance(ioopm_hash_table_iterator_t *it) {

    iterator_state_advance(it);
    skip_sentinel_nodes(it);
}

elem_t ioopm_hash_table_iterator_current_key(ioopm_hash_table_iterator_t *it) {

    return it->current_entry->key;
}

elem_t ioopm_hash_table_iterator_current_value(ioopm_hash_table_iterator_t *it) {

    return it->current_entry->value;
}

static void skip_sentinel_nodes(ioopm_hash_table_iterator_t *hti) {

    while (hti->current_bucket != NO_BUCKETS &&     
           hti->current_entry == &hti->ht->buckets[hti->current_bucket]) {

        iterator_state_advance(hti);
    }
}

bool ioopm_hash_table_iterator_at_end(ioopm_hash_table_iterator_t *hti) {

    return hti->current_bucket == NO_BUCKETS;
}
