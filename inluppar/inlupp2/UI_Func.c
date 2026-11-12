#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "data_structure.h"
#include "string_utils.h"
#include "UI_Func.h"

typedef struct cart ioopm_cart_t;
typedef struct cart_item ioopm_cart_item_t;

/**
 * @brief Represents an individual shopping cart.
 *
 * Each cart has a unique identifier and a linked list of items.
 */
struct cart {
  int cart_id;
  ioopm_cart_t *next;
  ioopm_cart_item_t *head;
  ioopm_cart_item_t *tail;
};

/**
 * @brief Represents an item inside a shopping cart.
 *
 * Stores the name of the merchandise and the quantity
 * selected by the customer.
 */
struct cart_item {
  char *merch_name;
  int quantity;
  ioopm_cart_item_t *next;
};

static ioopm_cart_t *cart_head = NULL;
/*
hash table containing ioopm_merch_t with the parameters name, and locs.
name is name of product
locs is pointer to linked list containing warehouses containing product
linked list has muiltiple nodes containing each shelf from each warehouse
containing the product and the quantity in said warehouse
*/

void ioopm_merch_add(ioopm_hash_table_t *ht, char *name, char* description, int price, ioopm_list_t *locs);
void merch_display(ioopm_hash_table_t *ht);
static ioopm_merch_t merch_create(char *name, char *description, int price, ioopm_list_t *locs);

static ioopm_cart_t *find_cart(int cart_id);
static ioopm_cart_item_t *find_cart_item(ioopm_cart_t *cart, char *name);
static ioopm_merch_t *find_merch(ioopm_hash_table_t *ht, char *name);
static int total_stock(ioopm_merch_t *merch);
static int total_in_carts(char *name);

static size_t string_hash(elem_t key) {
    size_t hash = 0;
    char *s = key.s;

    while (*s != '\0') {
        hash = hash * 31 + (unsigned char)*s;
        s++;
    }

    return hash;
}

static bool string_eq(elem_t a, elem_t b) {
    return strcmp(a.s, b.s) == 0;
}

void ioopm_hash_table_init(ioopm_hash_table_t *ht) {
    memset(ht, 0, sizeof(*ht));
    ht->hash_fn = string_hash;
    ht->key_eq_fn = string_eq;
}

void ioopm_merch_add(ioopm_hash_table_t *ht, char *name, char* description, int price, ioopm_list_t *locs) {
    for (int i = 0; i < NO_BUCKETS; i++) {
        if(ht->buckets[i].name != NULL && strcmp(ht->buckets[i].name, name) == 0) {

            printf("%s already exists", name);
            free(name);
            free(description);
            return;
        }
    }

    size_t bucket = ht->hash_fn(string_elem(name)) % NO_BUCKETS;

    size_t tries = 0;

    while (ht->buckets[bucket].name != NULL) {
        bucket = (bucket + 1) % NO_BUCKETS;
        tries++;
    }

    if (ht->buckets[bucket].name != NULL) {
        printf("The store is full\n");
        free(name);
        free(description);
        return;
    }

    ht->buckets[bucket] = merch_create(name, description, price, locs);
}

void merch_remove(ioopm_hash_table_t *ht, char *name) {
    //STUB
    for(int i = 0; i < NO_BUCKETS; i++) {

        if (ht->buckets[i].name != NULL && 
            strcmp (ht->buckets[i].name, name) == 0) {
            
            if (ht->buckets[i].locs != NULL) {
                    
                node_t *previous = ht->buckets[i].locs->head;

                while (previous != NULL) {
                    node_t *current = previous->next;
                    free(previous->element->aisle);
                    free(previous->element);
                    free(previous);
                    previous = current;

                    ht->buckets[i].locs->size--;
                }

                free(ht->buckets[i].locs);
            }
            
            free(ht->buckets[i].name);
            free(ht->buckets[i].description);

            ht->buckets[i] = (ioopm_merch_t){0};
            return;
        }
    }

    printf("%s does not exist\n", name);
}

void merch_edit(ioopm_hash_table_t *ht, char *old_name, char *new_name, char *new_description, int new_price) {

    if (new_price < 0) {
        printf("Price can not be negative\n");
        return;
    }

    ioopm_merch_t *merch = find_merch(ht, old_name);

    if (merch == NULL) {
        printf("%s does not exist\n", old_name);
        return;
    }

    if (strcmp(old_name, new_name) != 0 && find_merch(ht, new_name) != NULL) {
        printf("%s already exists\n", new_name);
        return;
    }

    char *name_copy = malloc(strlen(new_name) + 1);

    if (name_copy == NULL) {
        printf("Out of memory\n");
        return;
    }

    char *description_copy = malloc(strlen(new_description) + 1);

    if (description_copy == NULL) {
        free(name_copy);
        printf("Out of memory\n");
        return;
    }

    strcpy(name_copy, new_name);
    strcpy(description_copy, new_description);

    if (strcmp(old_name, new_name) != 0) {
        ioopm_cart_t *cart = cart_head;

        while (cart != NULL) {
            ioopm_cart_item_t *item = find_cart_item(cart, old_name);

            if (item != NULL) {
                char *cart_name = malloc(strlen(new_name) + 1);

                if (cart_name == NULL) {
                    printf("Out of memory\n");
                    free(name_copy);
                    free(description_copy);
                    return;
                }

                strcpy(cart_name, new_name);
                free(item->merch_name);
                item->merch_name = cart_name;
            }

            cart = cart->next;
        }
    }

    free(merch->name);
    free(merch->description);

    merch->name = name_copy;
    merch->description = description_copy;
    merch->price = new_price;
}

void merch_display(ioopm_hash_table_t *ht) {
    
    char *merch_array[NO_BUCKETS] = {NULL};
    int count = 0;

    for (int i = 0; i < NO_BUCKETS; i++) {
        if (ht->buckets[i].name != NULL) {
            merch_array[count] = ht->buckets[i].name;
            count++;
        }
    }

    int n = count;
    
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++){
            
            if (strcmp(merch_array[j], merch_array[j + 1]) > 0){
                
                char *temp = merch_array[j];
                merch_array[j] = merch_array[j + 1];
                merch_array[j + 1] = temp;
            }
        }
    }

    int shown = 0;
    bool run = true;

    while (run && shown < n) {

        int page_end = shown + 20;

        while (shown < page_end && shown < n) {
            printf("%s\n", merch_array[shown]);
            shown++;
        }

        if (shown < n) {
            printf("Do you want to continue? (Y/N): ");

            char answer;
            scanf(" %c", &answer);

            if (toupper((unsigned char)answer) != 'Y') {
                run = false;
            }
        }
    }
}

static ioopm_merch_t merch_create(char *name, char *description, int price, ioopm_list_t *locs) {
    ioopm_merch_t merch;

    merch.name = name;
    merch.description = description;
    merch.price = price;
    merch.locs = locs;

    return merch;
}

void ioopm_stock_show(ioopm_hash_table_t *ht, char *name) {
    
    //STUB
    ioopm_merch_t *merch = NULL;

    for (int i = 0; i < NO_BUCKETS; i++) {
        if (ht->buckets[i].name != NULL &&
            strcmp(ht->buckets[i].name, name) == 0) {

            merch = &ht->buckets[i];
            break;
        }
    }

    if (merch == NULL) {
        printf("%s does not exist\n", name);
        return;
    }

    if (merch->locs == NULL || merch->locs->size == 0) {
        printf("%s has no stock\n", name);
        return;
    }

    size_t count = merch->locs->size;
    ioopm_shelf_t **shelf_array = malloc(count * sizeof(ioopm_shelf_t *));

    if (shelf_array == NULL) {
        printf("Out of memory\n");
        return;
    }

    node_t *current = merch->locs->head;

    for (size_t i = 0; i < count; i++) {
        shelf_array[i] = current->element;
        current = current->next;
    }

    for (size_t i = 0; i < count - 1; i++) {
        for (size_t j = 0; j < count - i - 1; j++) {

            if (strcmp(shelf_array[j]->aisle, shelf_array[j + 1]->aisle) > 0) {

                ioopm_shelf_t *temp = shelf_array[j];
                shelf_array[j] = shelf_array[j + 1];
                shelf_array[j + 1] = temp;
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        printf("Aisle: %s, Quantity: %d\n",
               shelf_array[i]->aisle,
               shelf_array[i]->quantity);
    }

    free(shelf_array);
}

static bool shelf_taken(ioopm_hash_table_t *ht, ioopm_merch_t *merch, char *aisle) {

    for (int i = 0; i < NO_BUCKETS; i++) {
        ioopm_merch_t *other = &ht->buckets[i];

        if (other->name == NULL || other == merch || other->locs == NULL) {
            continue;
        }

        for (node_t *current = other->locs->head; current != NULL; current = current->next) {
            if (strcmp(current->element->aisle, aisle) == 0) {
                return true;
            }
        }
    }

    return false;
}

void ioopm_stock_replenish(ioopm_hash_table_t *ht, char *name, char *aisle, int amount) {

    //STUB
    ioopm_merch_t *merch = NULL;

    for (int i = 0; i < NO_BUCKETS; i++) {
        if (ht->buckets[i].name != NULL &&
            strcmp(ht->buckets[i].name, name) == 0) {

            merch = &ht->buckets[i];
            break;
        }
    }

    if (merch == NULL) {
        printf("%s does not exist\n", name);
        return;
    }

    if (merch->locs != NULL) {
        node_t *current = merch->locs->head;

        while (current != NULL) {

            if (strcmp(current->element->aisle, aisle) == 0) {
                current->element->quantity += amount;
                return;
            }

            current = current->next;
        }
    }

    ioopm_shelf_t *new_shelf = malloc(sizeof(ioopm_shelf_t));

    new_shelf->aisle = malloc(strlen(aisle) + 1);
    strcpy(new_shelf->aisle, aisle);
    new_shelf->quantity = amount;

    node_t *new_node = malloc(sizeof(node_t));

    new_node->element = new_shelf;
    new_node->next = NULL;

    if (merch->locs == NULL) {
        merch->locs = malloc(sizeof(ioopm_list_t));
        merch->locs->head = new_node;
        merch->locs->tail = new_node;
        merch->locs->size = 1;
    } else {
        merch->locs->tail->next = new_node;
        merch->locs->tail = new_node;
        merch->locs->size++;
    }
}

void ioopm_cart_create() {

    static int next_cart_id = 0;

    ioopm_cart_t *new_cart = malloc(sizeof(ioopm_cart_t));

    if (new_cart == NULL) {
        printf("Out of memory\n");
        return;
    }

    new_cart->cart_id = next_cart_id;
    new_cart->head = NULL;
    new_cart->tail = NULL;
    new_cart->next = NULL;

    next_cart_id++;

    if (cart_head == NULL) {
        cart_head = new_cart;
    } else {
        ioopm_cart_t *current = cart_head;

        while (current->next != NULL) {
            current = current->next;
        }

        current->next = new_cart;
    }

    printf("Cart %d created\n", new_cart->cart_id);
}

void ioopm_cart_remove(int cart_id) {

    ioopm_cart_t *previous = NULL;
    ioopm_cart_t *current = cart_head;

    while (current != NULL && current->cart_id != cart_id) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Cart %d does not exist\n", cart_id);
        return;
    }

    ioopm_cart_item_t *item = current->head;

    while (item != NULL) {
        ioopm_cart_item_t *next_item = item->next;
        free(item->merch_name);
        free(item);
        item = next_item;
    }

    if (previous == NULL) {
        cart_head = current->next;
    } else {
        previous->next = current->next;
    }

    free(current);
}

void ioopm_cart_add_item(ioopm_hash_table_t *ht, int cart_id, char *name, int quantity) {

    if (quantity <= 0) {
        printf("Quantity must be at least 1\n");
        return;
    }

    ioopm_cart_t *cart = find_cart(cart_id);

    if (cart == NULL) {
        printf("Cart %d does not exist\n", cart_id);
        return;
    }

    ioopm_merch_t *merch = find_merch(ht, name);

    if (merch == NULL) {
        printf("%s does not exist\n", name);
        return;
    }

    if (total_in_carts(name) + quantity > total_stock(merch)) {
        printf("Not enough stock of %s\n", name);
        return;
    }

    ioopm_cart_item_t *existing = find_cart_item(cart, name);

    if (existing != NULL) {
        existing->quantity += quantity;
        return;
    }

    ioopm_cart_item_t *new_item = malloc(sizeof(ioopm_cart_item_t));

    if (new_item == NULL) {
        printf("Out of memory\n");
        return;
    }

    new_item->merch_name = malloc(strlen(name) + 1);

    if (new_item->merch_name == NULL) {
        free(new_item);
        printf("Out of memory\n");
        return;
    }

    strcpy(new_item->merch_name, name);
    new_item->quantity = quantity;
    new_item->next = NULL;

    if (cart->head == NULL) {
        cart->head = new_item;
        cart->tail = new_item;
    } else {
        cart->tail->next = new_item;
        cart->tail = new_item;
    }
}

void ioopm_cart_remove_item(int cart_id, char *name, int quantity) {

    if (quantity <= 0) {
        printf("Quantity must be at least 1\n");
        return;
    }

    ioopm_cart_t *cart = find_cart(cart_id);

    if (cart == NULL) {
        printf("Cart %d does not exist\n", cart_id);
        return;
    }

    ioopm_cart_item_t *previous = NULL;
    ioopm_cart_item_t *current = cart->head;

    while (current != NULL && strcmp(current->merch_name, name) != 0) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("%s is not in cart %d\n", name, cart_id);
        return;
    }

    if (quantity > current->quantity) {
        printf("Cart only contains %d of %s\n", current->quantity, name);
        return;
    }

    if (quantity < current->quantity) {
        current->quantity -= quantity;
        return;
    }

    if (previous == NULL) {
        cart->head = current->next;
    } else {
        previous->next = current->next;
    }

    if (current == cart->tail) {
        cart->tail = previous;
    }

    free(current->merch_name);
    free(current);
}

void ioopm_calculate_cost(ioopm_hash_table_t *ht, int cart_id) {

    ioopm_cart_t *cart = find_cart(cart_id);

    if (cart == NULL) {
        printf("Cart %d does not exist\n", cart_id);
        return;
    }

    int total = 0;
    ioopm_cart_item_t *item = cart->head;

    while (item != NULL) {
        ioopm_merch_t *merch = find_merch(ht, item->merch_name);

        if (merch == NULL) {
            printf("%s no longer exists and is not counted\n", item->merch_name);
        } else {
            total += merch->price * item->quantity;
        }

        item = item->next;
    }

    printf("Total cost of cart %d: %d\n", cart_id, total);
}

void ioopm_checkout(ioopm_hash_table_t *ht, int cart_id) {

    ioopm_cart_t *cart = find_cart(cart_id);

    if (cart == NULL) {
        printf("Cart %d does not exist\n", cart_id);
        return;
    }

    ioopm_cart_item_t *item = cart->head;

    while (item != NULL) {
        ioopm_merch_t *merch = find_merch(ht, item->merch_name);

        if (merch == NULL) {
            printf("%s does not exist, checkout cancelled\n", item->merch_name);
            return;
        }

        if (total_stock(merch) < item->quantity) {
            printf("Not enough stock of %s, checkout cancelled\n", item->merch_name);
            return;
        }

        item = item->next;
    }

    item = cart->head;

    while (item != NULL) {
        ioopm_merch_t *merch = find_merch(ht, item->merch_name);

        int remaining = item->quantity;
        node_t *shelf_node = merch->locs->head;

        while (shelf_node != NULL && remaining > 0) {
            ioopm_shelf_t *shelf = shelf_node->element;

            if (shelf->quantity >= remaining) {
                shelf->quantity -= remaining;
                remaining = 0;
            } else {
                remaining -= shelf->quantity;
                shelf->quantity = 0;
            }

            shelf_node = shelf_node->next;
        }

        item = item->next;
    }

    printf("Cart %d bought\n", cart_id);
    ioopm_cart_remove(cart_id);
}

static ioopm_cart_t *find_cart(int cart_id) {

    ioopm_cart_t *current = cart_head;

    while (current != NULL) {
        if (current->cart_id == cart_id) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

static ioopm_cart_item_t *find_cart_item(ioopm_cart_t *cart, char *id) {

    ioopm_cart_item_t *current = cart->head;

    while (current != NULL) {
        if (strcmp(current->merch_name, id) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

static ioopm_merch_t *find_merch(ioopm_hash_table_t *ht, char *name) {
    for (int i = 0; i < NO_BUCKETS; i++) {
        if (ht->buckets[i].name != NULL &&
            strcmp(ht->buckets[i].name, name) == 0) {
            return &ht->buckets[i];
        }
    }

    return NULL;
}

static int total_stock(ioopm_merch_t *merch) {
    int total = 0;

    if (merch->locs != NULL) {
        node_t *current = merch->locs->head;

        while (current != NULL) {
            total += current->element->quantity;
            current = current->next;
        }
    }

    return total;
}

static int total_in_carts(char *name) {

    int total = 0;
    ioopm_cart_t *cart = cart_head;

    while (cart != NULL) {
        ioopm_cart_item_t *item = find_cart_item(cart, name);

        if (item != NULL) {
            total += item->quantity;
        }
        cart = cart->next;
    }

    return total;
}

//OPTIONAL
void ioopm_program_undo() {
    //STUB
}

void ioopm_program_exit(ioopm_hash_table_t *ht) {
    //STUB

    ioopm_cart_t *cart = cart_head;

    while (cart != NULL) {
        ioopm_cart_item_t *item = cart->head;

        while (item != NULL) {
            ioopm_cart_item_t *next_item = item->next;
            free(item->merch_name);
            free(item);
            item = next_item;
        }

        ioopm_cart_t *next_cart = cart->next;
        free(cart);
        cart = next_cart;
    }

    cart_head = NULL;

    for (int i = 0; i < NO_BUCKETS; i++) {
        if (ht->buckets[i].name != NULL) {
            merch_remove(ht, ht->buckets[i].name);
        }
    }

    printf("Goodbye\n");
    exit(0);
}

//OPTIONAL
void ioopm_file_persistance() {

}
