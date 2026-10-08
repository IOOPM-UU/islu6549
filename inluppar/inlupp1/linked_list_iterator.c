#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linked_list.h"
#include "linked_list_struct.h"

typedef struct list_iterator ioopm_list_iterator_t;

struct list_iterator
{
    ioopm_list_t *list;
    node_t *current_entry;
};

/// @brief Create a new iterator
/// @param l the list to iterate over
ioopm_list_iterator_t *ioopm_list_iterator_create(ioopm_list_t *l) {
    ioopm_list_iterator_t *li = malloc(sizeof(ioopm_list_iterator_t));

    li->list = l;
    li->current_entry = l->head;

    return li;
}

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_list_iterator_destroy(ioopm_list_iterator_t *iter) {
    free(iter);
}

/// @brief Checks if the iterator is positioned at-the-end
/// @param iter the iterator
/// @return true if iter is positioned at-the-end
bool ioopm_list_iterator_at_end(ioopm_list_iterator_t *iter) {
    if (iter->current_entry == NULL) {
        return true;
    } else {
        return false;
    }
}

/// @brief Step the iterator forward one step
/// @param iter the iterator
void ioopm_list_iterator_advance(ioopm_list_iterator_t *iter) {
    if (iter->current_entry != NULL) {
        iter->current_entry = iter->current_entry->next;
    } 
}

/// @brief Return the current element from the underlying list
/// @param iter the iterator
/// @return the current element
elem_t ioopm_list_iterator_current(ioopm_list_iterator_t *iter) {
    return iter->current_entry->value;
}

/// NOTE: REMOVE IS OPTIONAL TO IMPLEMENT
/// @brief Remove the current element from the underlying list
/// @param iter the iterator
/// @return the removed element
elem_t ioopm_list_iterator_remove(ioopm_list_iterator_t *iter);

/// NOTE: INSERT IS OPTIONAL TO IMPLEMENT
/// @brief Insert a new element into the underlying list making the current element it's next
/// @param iter the iterator
/// @param element the element to be inserted
void ioopm_list_iterator_insert(ioopm_list_iterator_t *iter, int element);