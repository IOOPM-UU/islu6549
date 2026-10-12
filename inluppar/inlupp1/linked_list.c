#include <stdbool.h>
#include <stdlib.h>
#include "linked_list.h"
#include "linked_list_struct.h"
#include "common.h"

typedef struct node node_t;

static node_t *node_create(elem_t value, node_t *next);

/// @brief Creates a new empty list
/// @return an empty linked list
ioopm_list_t *ioopm_list_create(void) {
    return calloc(1, sizeof(ioopm_list_t));
}

/// @brief Tear down the linked list and return all its memory (but not the memory of the elements)
/// @param list the list to be destroyed
void ioopm_list_destroy(ioopm_list_t *list) {
    node_t *previous = list->head;

    while (previous != NULL) {
        node_t *current = previous->next;
        free(previous);
        previous = current;
        list->size--;
    }

    free(list);
}

/// @brief Insert at the end of a linked list in O(1) time
/// @param list the linked list that will be appended
/// @param value the value to be appended
void ioopm_list_append(ioopm_list_t *list, elem_t value) {
    node_t *old_tail = list->tail;
    node_t *new_node = node_create(value, NULL);

    list->size++;

    if(old_tail == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = list->tail->next;
    }
}

/// @brief Insert at the front of a linked list in O(1) time
/// @param list the linked list that will be prepended to
/// @param value the value to be prepended
void ioopm_list_prepend(ioopm_list_t *list, elem_t value) {
    node_t *old_head = list->head;
    list->head = node_create(value, old_head);
    list->size++;

    if(old_head == NULL) {
        list->tail = list->head;
    }
}

/// @brief Return the first element of a linked list in O(1) time
/// @pre the list is non-empty
/// @param list the linked list to take the head of
elem_t ioopm_list_head(ioopm_list_t *list) {
    return list->head->value;
}

/// @brief Return the last element of a linked list in O(1) time
/// @pre the list is non-empty
/// @param list the linked list to take the last element of
elem_t ioopm_list_last(ioopm_list_t *list) {
    return list->tail->value;
}

/// @brief Insert an element into a linked list in O(n) time.
/// The valid values of index are [0,n] for a list of n elements,
/// where 0 means before the first element and n means after
/// the last element.
/// @pre 0 <= index <= length(list)
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param value the value to be inserted
void ioopm_list_insert(ioopm_list_t *list, size_t index, elem_t value) {
    
    if (index == 0) {
        ioopm_list_prepend(list, value);
        return;
    } else if (index == list->size) {
        ioopm_list_append(list, value);
        return;
    }

    node_t *value_previous = list->head;
    node_t *value_next = value_previous->next;

    for (size_t i = 0; i < index - 1; i++) {
        value_previous = value_previous->next;
        value_next = value_previous->next;
    }

    node_t *new_node = node_create(value, value_next);

    value_previous->next = new_node;
    list->size++;
}

static node_t *node_create(elem_t value, node_t *next)
{
    
    node_t *node = malloc(sizeof(node_t));

    node->value = value;
    node->next = next;

    return node;
}

/// @brief Remove an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @pre 0 <= index < length(list)
/// @param list the linked list
/// @param index the position in the list
/// @return the value removed
elem_t ioopm_list_remove(ioopm_list_t *list, size_t index) {
    list->size--;
    if (index == 0) {
        node_t *new_head = list->head->next;

        elem_t removed_value = list->head->value;
        free(list->head);
        list->head = new_head;

        if (list->size == 0) {
            list->tail = NULL;
        }

        return removed_value;
    }

    node_t *value_previous = list->head; 
    node_t *value_next = value_previous->next;

    for (size_t i = 0; i < index - 1; i++) {
        value_previous = value_previous->next;
        value_next = value_previous->next;
    }

    if (index == list->size) {
        elem_t removed_value = list->tail->value;

        value_previous->next = NULL;
        free(list->tail);
        list->tail = value_previous;

        return removed_value;
    }

    node_t *new_node = value_next->next;
    value_previous->next = new_node;

    elem_t removed_value = value_next->value;

    free(value_next); 
    
    return removed_value;
}

/// @brief Retrieve an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @pre 0 <= index < length(list)
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @return the value at the given position
elem_t ioopm_list_get(ioopm_list_t *list, size_t index) {
    node_t *current_value = list->head; 

    for (size_t i = 0; i < index; i++) {
        current_value = current_value->next;
    }

    return current_value->value;
}

/*
static elem_t list_get_recursive(node_t *node, size_t index)
{
    if (index != 0)
    {
        return list_get_recursive(node->next, index - 1);
    }

    return node->value;
}
*/
/*
// Tail recursive version
static elem_t list_get_tail_recursive(node_t *node, size_t index)
{
    if (index == 0)
    {
        return node->value;
    }

    return list_get_tail_recursive(node->next, index - 1);
}

elem_t ioopm_list_get_tail_recursive(ioopm_list_t *list, size_t index)
{
    return list_get_tail_recursive(list->head, index);
}
*/

/// @brief Lookup the number of elements in the linked list in O(1) time
/// @param list the linked list
/// @return the number of elements in the list
size_t ioopm_list_size(ioopm_list_t *list) {
    return list->size;
}

/// @brief Test whether a list is empty or not
/// @param list the linked list
/// @return true if the number of elements int the list is 0, else false
bool ioopm_list_is_empty(ioopm_list_t *list) {
    if (list->head == NULL) {
        return true;
    } else {
        return false;
    }
}