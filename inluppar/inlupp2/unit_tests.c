#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/wait.h>

#include "common.h"
#include "data_structure.h"
#include "string_utils.h"
#include "UI_Func.h"
#include "webstore.h"

// function to guarantee same bucket for tests
static size_t constant_hash(elem_t key)
{
  (void) key;
  return 0;
}

int init_suite(void) {
  // Change this function if you want to do something *before* you
  // run a test suite
  return 0;
}

int clean_suite(void) {
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}

////HELPERS////

static FILE *capture_file;
static int saved_stdout;

// prints to a temporary file
static void start_capture(void) {
  fflush(stdout);
  capture_file = tmpfile();
  saved_stdout = dup(STDOUT_FILENO);
  dup2(fileno(capture_file), STDOUT_FILENO);
}

// restores stdout and copies what was printed into buf
static void stop_capture(char *buf, size_t size) {
  fflush(stdout);
  dup2(saved_stdout, STDOUT_FILENO);
  close(saved_stdout);

  rewind(capture_file);
  size_t n = fread(buf, 1, size - 1, capture_file);
  buf[n] = '\0';
  fclose(capture_file);
}

static FILE *real_stdin;

// enters a keyboard input without needing a real keyboard input
static void feed_input(char *text) {
  real_stdin = stdin;
  stdin = fmemopen(text, strlen(text), "r");
}

static void restore_input(void) {
  fclose(stdin);
  stdin = real_stdin;
}

// runs function in a child process to execute code that would normally terminate the program
static int run_in_child(void (*fun)(ioopm_hash_table_t *), ioopm_hash_table_t *ht,
                        char *input, char *buf, size_t size) {
  int fd[2];
  pipe(fd);
  fflush(stdout);

  pid_t pid = fork();

  if (pid == 0) {
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);

    if (input != NULL) {
      feed_input(input);
    }

    fun(ht);
    _exit(99);
  }

  close(fd[1]);

  size_t total = 0;
  ssize_t n;
  while ((n = read(fd[0], buf + total, size - 1 - total)) > 0) {
    total += n;
  }
  buf[total] = '\0';
  close(fd[0]);

  int status = 0;
  waitpid(pid, &status, 0);

  return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

static ioopm_merch_t *get_merch(ioopm_hash_table_t *ht, char *name) {
  for (int i = 0; i < NO_BUCKETS; i++) {
    if (ht->buckets[i].name != NULL && strcmp(ht->buckets[i].name, name) == 0) {
      return &ht->buckets[i];
    }
  }

  return NULL;
}

static int count_merch(ioopm_hash_table_t *ht) {
  int count = 0;

  for (int i = 0; i < NO_BUCKETS; i++) {
    if (ht->buckets[i].name != NULL) {
      count++;
    }
  }

  return count;
}

// merch_add takes ownership of name and description, so give it copies
static void add_merch(ioopm_hash_table_t *ht, char *name, int price) {
  ioopm_merch_add(ht, strdup(name), strdup("desc"), price, NULL);
}

// frees everything the store owns (same as ioopm_program_exit without exit)
static void clear_store(ioopm_hash_table_t *ht) {
  for (int i = 0; i < NO_BUCKETS; i++) {
    if (ht->buckets[i].name != NULL) {
      merch_remove(ht, ht->buckets[i].name);
    }
  }
}

// creates a cart and returns its id, read from "Cart <id> created"
static int create_cart(void) {
  char out[64];
  int cart_id = -1;

  start_capture();
  ioopm_cart_create();
  stop_capture(out, sizeof(out));

  sscanf(out, "Cart %d created", &cart_id);
  return cart_id;
}

// returns the total cost printed by ioopm_calculate_cost, -1 if nothing was printed
static int cart_cost(ioopm_hash_table_t *ht, int cart_id) {
  char out[256];
  int cost = -1;

  start_capture();
  ioopm_calculate_cost(ht, cart_id);
  stop_capture(out, sizeof(out));

  char *total = strstr(out, "Total cost of cart");
  if (total != NULL) {
    sscanf(total, "Total cost of cart %*d: %d", &cost);
  }

  return cost;
}

////HASH TABLE INIT TESTS////

void test_hash_table_init(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  CU_ASSERT_EQUAL(count_merch(&ht), 0);
  CU_ASSERT_PTR_NOT_NULL(ht.hash_fn);
  CU_ASSERT_PTR_NOT_NULL(ht.key_eq_fn);

  // "a" hashes to its ascii value
  CU_ASSERT_EQUAL(ht.hash_fn(string_elem("a")), 97);
  CU_ASSERT_TRUE(ht.key_eq_fn(string_elem("a"), string_elem("a")));
  CU_ASSERT_FALSE(ht.key_eq_fn(string_elem("a"), string_elem("b")));
}

////MERCH ADD TESTS////

void test_merch_add_once(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  ioopm_merch_add(&ht, strdup("Apple"), strdup("A red fruit"), 10, NULL);

  ioopm_merch_t *merch = get_merch(&ht, "Apple");

  CU_ASSERT_PTR_NOT_NULL_FATAL(merch);
  CU_ASSERT_STRING_EQUAL(merch->description, "A red fruit");
  CU_ASSERT_EQUAL(merch->price, 10);
  CU_ASSERT_PTR_NULL(merch->locs);

  clear_store(&ht);
}

void test_merch_add_multiple(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 20);
  add_merch(&ht, "Plum", 30);

  CU_ASSERT_EQUAL(count_merch(&ht), 3);
  CU_ASSERT_EQUAL(get_merch(&ht, "Apple")->price, 10);
  CU_ASSERT_EQUAL(get_merch(&ht, "Pear")->price, 20);
  CU_ASSERT_EQUAL(get_merch(&ht, "Plum")->price, 30);

  clear_store(&ht);
}

void test_merch_add_duplicate(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[128];

  add_merch(&ht, "Apple", 10);

  // same name again, the original must be kept
  start_capture();
  ioopm_merch_add(&ht, strdup("Apple"), strdup("other"), 99, NULL);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple already exists"));
  CU_ASSERT_EQUAL(count_merch(&ht), 1);
  CU_ASSERT_EQUAL(get_merch(&ht, "Apple")->price, 10);
  CU_ASSERT_STRING_EQUAL(get_merch(&ht, "Apple")->description, "desc");

  clear_store(&ht);
}

void test_merch_add_negative_price(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  start_capture();
  ioopm_merch_add(&ht, strdup("Apple"), strdup("desc"), -5, NULL);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Price can not be negative"));
  CU_ASSERT_EQUAL(count_merch(&ht), 0);
}

void test_merch_add_zero_price(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Free_Thingie", 0);

  CU_ASSERT_EQUAL(count_merch(&ht), 1);
  CU_ASSERT_EQUAL(get_merch(&ht, "Free_Thingie")->price, 0);

  clear_store(&ht);
}

void test_merch_add_collision(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  ht.hash_fn = constant_hash;

  add_merch(&ht, "first", 1);
  add_merch(&ht, "second", 2);
  add_merch(&ht, "third", 3);

  CU_ASSERT_STRING_EQUAL(ht.buckets[0].name, "first");
  CU_ASSERT_STRING_EQUAL(ht.buckets[1].name, "second");
  CU_ASSERT_STRING_EQUAL(ht.buckets[2].name, "third");
  CU_ASSERT_EQUAL(count_merch(&ht), 3);

  clear_store(&ht);
}

void test_merch_add_store_full(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];
  char name[16];

  for (int i = 0; i < NO_BUCKETS; i++) {
    snprintf(name, sizeof(name), "m%d", i);
    add_merch(&ht, name, i);
  }

  CU_ASSERT_EQUAL(count_merch(&ht), NO_BUCKETS);

  start_capture();
  add_merch(&ht, "extra", 1);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "The store is full"));
  CU_ASSERT_EQUAL(count_merch(&ht), NO_BUCKETS);
  CU_ASSERT_PTR_NULL(get_merch(&ht, "extra"));

  clear_store(&ht);
}

////MERCH REMOVE TESTS////

void test_merch_remove_once(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);

  merch_remove(&ht, "Apple");

  CU_ASSERT_PTR_NULL(get_merch(&ht, "Apple"));
  CU_ASSERT_EQUAL(count_merch(&ht), 0);
}

void test_merch_remove_missing(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  start_capture();
  merch_remove(&ht, "Apple");
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple does not exist"));
}

void test_merch_remove_one_of_many(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 20);
  add_merch(&ht, "Plum", 30);

  merch_remove(&ht, "Pear");

  CU_ASSERT_PTR_NOT_NULL(get_merch(&ht, "Apple"));
  CU_ASSERT_PTR_NULL(get_merch(&ht, "Pear"));
  CU_ASSERT_PTR_NOT_NULL(get_merch(&ht, "Plum"));
  CU_ASSERT_EQUAL(count_merch(&ht), 2);

  clear_store(&ht);
}

void test_merch_remove_with_stock(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Apple", "B2", 3);

  merch_remove(&ht, "Apple");

  CU_ASSERT_PTR_NULL(get_merch(&ht, "Apple"));
  CU_ASSERT_EQUAL(count_merch(&ht), 0);
}

void test_merch_remove_and_add_again(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  merch_remove(&ht, "Apple");
  add_merch(&ht, "Apple", 20);

  CU_ASSERT_EQUAL(count_merch(&ht), 1);
  CU_ASSERT_EQUAL(get_merch(&ht, "Apple")->price, 20);

  clear_store(&ht);
}

////MERCH EDIT TESTS////

void test_merch_edit_all_fields(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);

  merch_edit(&ht, "Apple", "Pear", "A green fruit", 20);

  ioopm_merch_t *merch = get_merch(&ht, "Pear");

  CU_ASSERT_PTR_NULL(get_merch(&ht, "Apple"));
  CU_ASSERT_PTR_NOT_NULL_FATAL(merch);
  CU_ASSERT_STRING_EQUAL(merch->description, "A green fruit");
  CU_ASSERT_EQUAL(merch->price, 20);
  CU_ASSERT_EQUAL(count_merch(&ht), 1);

  clear_store(&ht);
}

void test_merch_edit_same_name(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);

  merch_edit(&ht, "Apple", "Apple", "New description", 15);

  ioopm_merch_t *merch = get_merch(&ht, "Apple");

  CU_ASSERT_PTR_NOT_NULL_FATAL(merch);
  CU_ASSERT_STRING_EQUAL(merch->description, "New description");
  CU_ASSERT_EQUAL(merch->price, 15);
  CU_ASSERT_EQUAL(count_merch(&ht), 1);

  clear_store(&ht);
}

void test_merch_edit_missing(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  start_capture();
  merch_edit(&ht, "Apple", "Pear", "desc", 5);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple does not exist"));
  CU_ASSERT_EQUAL(count_merch(&ht), 0);
}

void test_merch_edit_negative_price(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);

  start_capture();
  merch_edit(&ht, "Apple", "Pear", "other", -1);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Price can not be negative"));
  CU_ASSERT_PTR_NOT_NULL(get_merch(&ht, "Apple"));
  CU_ASSERT_PTR_NULL(get_merch(&ht, "Pear"));
  CU_ASSERT_EQUAL(get_merch(&ht, "Apple")->price, 10);

  clear_store(&ht);
}

void test_merch_edit_name_taken(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 20);

  start_capture();
  merch_edit(&ht, "Apple", "Pear", "other", 30);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Pear already exists"));
  CU_ASSERT_EQUAL(get_merch(&ht, "Apple")->price, 10);
  CU_ASSERT_EQUAL(get_merch(&ht, "Pear")->price, 20);
  CU_ASSERT_STRING_EQUAL(get_merch(&ht, "Apple")->description, "desc");

  clear_store(&ht);
}

void test_merch_edit_keeps_stock(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  merch_edit(&ht, "Apple", "Pear", "desc", 10);

  ioopm_merch_t *merch = get_merch(&ht, "Pear");

  CU_ASSERT_PTR_NOT_NULL_FATAL(merch);
  CU_ASSERT_PTR_NOT_NULL_FATAL(merch->locs);
  CU_ASSERT_EQUAL(merch->locs->size, 1);
  CU_ASSERT_EQUAL(merch->locs->head->element->quantity, 5);

  clear_store(&ht);
}

void test_merch_edit_updates_carts(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);

  merch_edit(&ht, "Apple", "Pear", "desc", 20);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 40);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_merch_edit_updates_many_carts(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Plum", 5);
  ioopm_stock_replenish(&ht, "Apple", "A1", 10);
  ioopm_stock_replenish(&ht, "Plum", "B1", 10);

  int cart_one = create_cart();
  int cart_two = create_cart();
  int cart_three = create_cart();

  ioopm_cart_add_item(&ht, cart_one, "Apple", 1);
  ioopm_cart_add_item(&ht, cart_two, "Plum", 2);
  ioopm_cart_add_item(&ht, cart_three, "Apple", 3);

  merch_edit(&ht, "Apple", "Pear", "desc", 20);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_one), 20);
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_two), 10);
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_three), 60);

  ioopm_cart_remove(cart_one);
  ioopm_cart_remove(cart_two);
  ioopm_cart_remove(cart_three);
  clear_store(&ht);
}

////MERCH DISPLAY TESTS////

void test_merch_display_empty(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  start_capture();
  merch_display(&ht);
  stop_capture(out, sizeof(out));

  CU_ASSERT_STRING_EQUAL(out, "");
}

void test_merch_display_sorted(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Cherry", 3);
  add_merch(&ht, "Apple", 1);
  add_merch(&ht, "Banana", 2);

  start_capture();
  merch_display(&ht);
  stop_capture(out, sizeof(out));

  CU_ASSERT_STRING_EQUAL(out, "Apple\nBanana\nCherry\n");

  clear_store(&ht);
}

void test_merch_display_exactly_one_page(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[1024];
  char name[16];

  for (int i = 1; i <= 20; i++) {
    snprintf(name, sizeof(name), "item%02d", i);
    add_merch(&ht, name, i);
  }

  start_capture();
  merch_display(&ht);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "item20"));
  CU_ASSERT_PTR_NULL(strstr(out, "continue"));

  clear_store(&ht);
}

void test_merch_display_stop_after_first_page(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[1024];
  char name[16];

  for (int i = 25; i >= 1; i--) {
    snprintf(name, sizeof(name), "item%02d", i);
    add_merch(&ht, name, i);
  }

  feed_input("N\n");
  start_capture();
  merch_display(&ht);
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "item01"));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "item20"));
  CU_ASSERT_PTR_NULL(strstr(out, "item21"));

  clear_store(&ht);
}

void test_merch_display_continue_to_second_page(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[1024];
  char name[16];

  for (int i = 25; i >= 1; i--) {
    snprintf(name, sizeof(name), "item%02d", i);
    add_merch(&ht, name, i);
  }

  feed_input("y\n");
  start_capture();
  merch_display(&ht);
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "item20"));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "item21"));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "item25"));

  clear_store(&ht);
}

////STOCK TESTS////

void test_stock_show_missing_merch(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  start_capture();
  ioopm_stock_show(&ht, "Apple");
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple does not exist"));
}

void test_stock_show_no_stock(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);

  start_capture();
  ioopm_stock_show(&ht, "Apple");
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple has no stock"));

  clear_store(&ht);
}

void test_stock_show_sorted_by_aisle(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "C3", 1);
  ioopm_stock_replenish(&ht, "Apple", "A1", 3);
  ioopm_stock_replenish(&ht, "Apple", "B2", 5);

  start_capture();
  ioopm_stock_show(&ht, "Apple");
  stop_capture(out, sizeof(out));

  CU_ASSERT_STRING_EQUAL(out, "Aisle: A1, Quantity: 3\n"
                              "Aisle: B2, Quantity: 5\n"
                              "Aisle: C3, Quantity: 1\n");

  clear_store(&ht);
}

void test_stock_replenish_new_shelf(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);

  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  ioopm_merch_t *merch = get_merch(&ht, "Apple");

  CU_ASSERT_PTR_NOT_NULL_FATAL(merch->locs);
  CU_ASSERT_EQUAL(merch->locs->size, 1);
  CU_ASSERT_STRING_EQUAL(merch->locs->head->element->aisle, "A1");
  CU_ASSERT_EQUAL(merch->locs->head->element->quantity, 5);

  clear_store(&ht);
}

void test_stock_replenish_same_shelf(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);

  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Apple", "A1", 3);

  ioopm_merch_t *merch = get_merch(&ht, "Apple");

  CU_ASSERT_EQUAL(merch->locs->size, 1);
  CU_ASSERT_EQUAL(merch->locs->head->element->quantity, 8);

  clear_store(&ht);
}

void test_stock_replenish_multiple_shelves(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);

  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Apple", "B2", 7);
  ioopm_stock_replenish(&ht, "Apple", "C3", 9);

  ioopm_merch_t *merch = get_merch(&ht, "Apple");

  CU_ASSERT_EQUAL(merch->locs->size, 3);
  CU_ASSERT_STRING_EQUAL(merch->locs->head->element->aisle, "A1");
  CU_ASSERT_STRING_EQUAL(merch->locs->tail->element->aisle, "C3");
  CU_ASSERT_EQUAL(merch->locs->tail->element->quantity, 9);

  clear_store(&ht);
}

void test_stock_replenish_missing_merch(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  start_capture();
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple does not exist"));
}

void test_stock_replenish_bad_amount(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);

  start_capture();
  ioopm_stock_replenish(&ht, "Apple", "A1", 0);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Amount must be at least 1"));
  CU_ASSERT_PTR_NULL(get_merch(&ht, "Apple")->locs);

  start_capture();
  ioopm_stock_replenish(&ht, "Apple", "A1", -4);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Amount must be at least 1"));
  CU_ASSERT_PTR_NULL(get_merch(&ht, "Apple")->locs);

  clear_store(&ht);
}

void test_stock_replenish_shelf_taken(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 20);

  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  start_capture();
  ioopm_stock_replenish(&ht, "Pear", "A1", 3);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Shelf A1 is already used"));
  CU_ASSERT_PTR_NULL(get_merch(&ht, "Pear")->locs);
  CU_ASSERT_EQUAL(get_merch(&ht, "Apple")->locs->head->element->quantity, 5);

  ioopm_stock_replenish(&ht, "Pear", "A2", 3);
  CU_ASSERT_EQUAL(get_merch(&ht, "Pear")->locs->head->element->quantity, 3);

  clear_store(&ht);
}

////CART TESTS////

void test_cart_create(void) {
  int first = create_cart();
  int second = create_cart();

  CU_ASSERT_TRUE(first >= 0);
  CU_ASSERT_EQUAL(second, first + 1);

  ioopm_cart_remove(first);
  ioopm_cart_remove(second);
}

void test_cart_remove_once(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();

  ioopm_cart_remove(cart_id);

  start_capture();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 1);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "does not exist"));

  clear_store(&ht);
}

void test_cart_remove_missing(void) {
  char out[256];

  start_capture();
  ioopm_cart_remove(9999);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Cart 9999 does not exist"));
}

void test_cart_remove_middle_and_last(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  int first = create_cart();
  int second = create_cart();
  int third = create_cart();

  ioopm_cart_remove(second);
  ioopm_cart_remove(third);

  CU_ASSERT_EQUAL(cart_cost(&ht, first), 0);
  CU_ASSERT_EQUAL(cart_cost(&ht, second), -1);
  CU_ASSERT_EQUAL(cart_cost(&ht, third), -1);

  int fourth = create_cart();
  CU_ASSERT_EQUAL(cart_cost(&ht, fourth), 0);

  ioopm_cart_remove(first);
  ioopm_cart_remove(fourth);
}

void test_cart_remove_with_items(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 20);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Pear", "B1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);
  ioopm_cart_add_item(&ht, cart_id, "Pear", 2);

  ioopm_cart_remove(cart_id);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), -1);

  clear_store(&ht);
}

void test_cart_add_item_once(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 20);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cart_add_item_twice(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);
  ioopm_cart_add_item(&ht, cart_id, "Apple", 1);

  // quantity is increased, not a second item
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 30);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cart_add_item_multiple_merch(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 20);
  add_merch(&ht, "Plum", 30);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Pear", "B1", 5);
  ioopm_stock_replenish(&ht, "Plum", "C1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 1);
  ioopm_cart_add_item(&ht, cart_id, "Pear", 1);
  ioopm_cart_add_item(&ht, cart_id, "Plum", 1);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 60);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cart_add_item_bad_quantity(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();

  start_capture();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 0);
  stop_capture(out, sizeof(out));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Quantity must be at least 1"));

  start_capture();
  ioopm_cart_add_item(&ht, cart_id, "Apple", -2);
  stop_capture(out, sizeof(out));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Quantity must be at least 1"));

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 0);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cart_add_item_missing_cart(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  start_capture();
  ioopm_cart_add_item(&ht, 9999, "Apple", 1);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Cart 9999 does not exist"));

  clear_store(&ht);
}

void test_cart_add_item_missing_merch(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  int cart_id = create_cart();

  start_capture();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 1);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple does not exist"));
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 0);

  ioopm_cart_remove(cart_id);
}

void test_cart_add_item_not_enough_stock(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();

  start_capture();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 6);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Not enough stock of Apple"));
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 0);

  ioopm_cart_add_item(&ht, cart_id, "Apple", 5);
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 50);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cart_add_item_no_stock_at_all(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[128];

  add_merch(&ht, "Apple", 10);

  int cart_id = create_cart();

  start_capture();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 1);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Not enough stock of Apple"));

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cart_add_item_reserved_in_other_cart(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_one = create_cart();
  int cart_two = create_cart();

  ioopm_cart_add_item(&ht, cart_one, "Apple", 3);

  start_capture();
  ioopm_cart_add_item(&ht, cart_two, "Apple", 3);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Not enough stock of Apple"));
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_two), 0);

  ioopm_cart_add_item(&ht, cart_two, "Apple", 2);
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_two), 20);

  ioopm_cart_remove(cart_one);
  ioopm_cart_remove(cart_two);
  clear_store(&ht);
}

void test_cart_remove_item_partial(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 4);

  ioopm_cart_remove_item(cart_id, "Apple", 1);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 30);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cart_remove_item_all(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);

  ioopm_cart_remove_item(cart_id, "Apple", 2);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 0);

  start_capture();
  ioopm_cart_remove_item(cart_id, "Apple", 1);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple is not in cart"));

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cart_remove_item_first_middle_last(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 1);
  add_merch(&ht, "Pear", 10);
  add_merch(&ht, "Plum", 100);
  add_merch(&ht, "Fig", 1000);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Pear", "B1", 5);
  ioopm_stock_replenish(&ht, "Plum", "C1", 5);
  ioopm_stock_replenish(&ht, "Fig", "D1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 1);
  ioopm_cart_add_item(&ht, cart_id, "Pear", 1);
  ioopm_cart_add_item(&ht, cart_id, "Plum", 1);

  ioopm_cart_remove_item(cart_id, "Pear", 1);
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 101);

  ioopm_cart_remove_item(cart_id, "Plum", 1);
  ioopm_cart_add_item(&ht, cart_id, "Fig", 1);
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 1001);

  ioopm_cart_remove_item(cart_id, "Apple", 1);
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 1000);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cart_remove_item_errors(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);

  start_capture();
  ioopm_cart_remove_item(cart_id, "Apple", 0);
  stop_capture(out, sizeof(out));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Quantity must be at least 1"));

  start_capture();
  ioopm_cart_remove_item(9999, "Apple", 1);
  stop_capture(out, sizeof(out));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Cart 9999 does not exist"));

  start_capture();
  ioopm_cart_remove_item(cart_id, "Pear", 1);
  stop_capture(out, sizeof(out));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Pear is not in cart"));

  start_capture();
  ioopm_cart_remove_item(cart_id, "Apple", 3);
  stop_capture(out, sizeof(out));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Cart only contains 2 of Apple"));

  // none of the failed removals may have changed the cart
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 20);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

////COST CALCULATION TESTS////

void test_cost_empty_cart(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  int cart_id = create_cart();

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 0);

  ioopm_cart_remove(cart_id);
}

void test_cost_multiple_merch(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 5);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Pear", "B1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);
  ioopm_cart_add_item(&ht, cart_id, "Pear", 3);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 35);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cost_missing_cart(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  start_capture();
  ioopm_calculate_cost(&ht, 9999);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Cart 9999 does not exist"));
}

void test_cost_after_price_change(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);

  merch_edit(&ht, "Apple", "Apple", "desc", 15);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 30);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_cost_removed_merch(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 5);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Pear", "B1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);
  ioopm_cart_add_item(&ht, cart_id, "Pear", 3);

  merch_remove(&ht, "Apple");

  start_capture();
  ioopm_calculate_cost(&ht, cart_id);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple no longer exists"));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Total cost of cart"));
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 15);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

////CHECKOUT TESTS////

void test_checkout_reduces_stock(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 3);

  start_capture();
  ioopm_checkout(&ht, cart_id);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "bought"));
  CU_ASSERT_EQUAL(get_merch(&ht, "Apple")->locs->head->element->quantity, 2);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), -1);

  clear_store(&ht);
}

void test_checkout_multiple_shelves(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 2);
  ioopm_stock_replenish(&ht, "Apple", "B1", 3);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 4);

  start_capture();
  ioopm_checkout(&ht, cart_id);
  stop_capture(out, sizeof(out));

  ioopm_merch_t *merch = get_merch(&ht, "Apple");

  CU_ASSERT_EQUAL(merch->locs->head->element->quantity, 0);
  CU_ASSERT_EQUAL(merch->locs->tail->element->quantity, 1);

  clear_store(&ht);
}

void test_checkout_multiple_merch(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 20);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Pear", "B1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 5);
  ioopm_cart_add_item(&ht, cart_id, "Pear", 1);

  start_capture();
  ioopm_checkout(&ht, cart_id);
  stop_capture(out, sizeof(out));

  CU_ASSERT_EQUAL(get_merch(&ht, "Apple")->locs->head->element->quantity, 0);
  CU_ASSERT_EQUAL(get_merch(&ht, "Pear")->locs->head->element->quantity, 4);

  clear_store(&ht);
}

void test_checkout_empty_cart(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  int cart_id = create_cart();

  start_capture();
  ioopm_checkout(&ht, cart_id);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "bought"));
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), -1);
}

void test_checkout_missing_cart(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  start_capture();
  ioopm_checkout(&ht, 9999);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Cart 9999 does not exist"));
}

void test_checkout_not_enough_stock(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 20);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Pear", "B1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Pear", 2);
  ioopm_cart_add_item(&ht, cart_id, "Apple", 3);

  get_merch(&ht, "Apple")->locs->head->element->quantity = 1;

  start_capture();
  ioopm_checkout(&ht, cart_id);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Not enough stock of Apple, checkout cancelled"));
  CU_ASSERT_EQUAL(get_merch(&ht, "Apple")->locs->head->element->quantity, 1);
  CU_ASSERT_EQUAL(get_merch(&ht, "Pear")->locs->head->element->quantity, 5);

  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 70);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_checkout_removed_merch(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 20);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);
  ioopm_stock_replenish(&ht, "Pear", "B1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);
  ioopm_cart_add_item(&ht, cart_id, "Pear", 2);

  merch_remove(&ht, "Apple");

  start_capture();
  ioopm_checkout(&ht, cart_id);
  stop_capture(out, sizeof(out));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple does not exist, checkout cancelled"));
  CU_ASSERT_EQUAL(get_merch(&ht, "Pear")->locs->head->element->quantity, 5);
  CU_ASSERT_EQUAL(cart_cost(&ht, cart_id), 40);

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_checkout_two_carts(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[256];

  add_merch(&ht, "Apple", 10);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_one = create_cart();
  int cart_two = create_cart();
  ioopm_cart_add_item(&ht, cart_one, "Apple", 3);
  ioopm_cart_add_item(&ht, cart_two, "Apple", 2);

  start_capture();
  ioopm_checkout(&ht, cart_one);
  ioopm_checkout(&ht, cart_two);
  stop_capture(out, sizeof(out));

  CU_ASSERT_EQUAL(get_merch(&ht, "Apple")->locs->head->element->quantity, 0);

  clear_store(&ht);
}

////STRING UTILS TESTS////

void test_string_length(void) {
  CU_ASSERT_EQUAL(string_length(""), 0);
  CU_ASSERT_EQUAL(string_length("a"), 1);
  CU_ASSERT_EQUAL(string_length("hello"), 5);
}

void test_ask_question_int(void) {
  char out[256];

  feed_input("abc\n42\n");
  start_capture();
  int result = ask_question_int("Number?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_EQUAL(result, 42);
}

void test_ask_question_float(void) {
  char out[256];

  feed_input("abc\n2.5\n");
  start_capture();
  float result = ask_question_float("Float?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_DOUBLE_EQUAL(result, 2.5, 0.001);
}

void test_ask_question_string(void) {
  char out[256];

  feed_input("hello world\n");
  start_capture();
  char *result = ask_question_string("String?");
  stop_capture(out, sizeof(out));
  restore_input();

  // scanf with %s stops at the first space
  CU_ASSERT_STRING_EQUAL(result, "hello");

  free(result);
}

void test_ask_question_line(void) {
  char out[256];

  // empty lines are skipped, spaces are kept
  feed_input("\n\nHello world\n");
  start_capture();
  char *result = ask_question_line("Line?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_STRING_EQUAL(result, "Hello world");

  free(result);
}

void test_read_string(void) {
  char buf[16];

  feed_input("abc\n");
  int length = read_string(buf, sizeof(buf));
  restore_input();

  CU_ASSERT_EQUAL(length, 3);
  CU_ASSERT_STRING_EQUAL(buf, "abc");
}

void test_ask_question_menu(void) {
  char out[256];

  feed_input("xx\nz\na\n");
  start_capture();
  char result = ask_question_menu("Menu?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_EQUAL(result, 'A');

  feed_input("+\n");
  start_capture();
  result = ask_question_menu("Menu?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_EQUAL(result, '+');
}

void test_ask_question_menu_end_of_input(void) {
  char out[256];

  feed_input("xx\n");
  start_capture();
  char result = ask_question_menu("Menu?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_EQUAL(result, 'Q');
}

void test_ask_question_end_of_input(void) {
  char out[256];

  feed_input("abc\n");
  start_capture();
  int number = ask_question_int("Number?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_EQUAL(number, 0);

  feed_input("abc\n");
  start_capture();
  float decimal = ask_question_float("Float?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_DOUBLE_EQUAL(decimal, 0.0, 0.001);

  feed_input("   \n");
  start_capture();
  char *string = ask_question_string("String?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_STRING_EQUAL(string, "");
  free(string);

  feed_input("\n\n");
  start_capture();
  char *line = ask_question_line("Line?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_STRING_EQUAL(line, "");
  free(line);

  feed_input("1A\n");
  start_capture();
  char *shelf = ask_question_shelf("Shelf?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_STRING_EQUAL(shelf, "");
  free(shelf);
}

void test_ask_question_float_formats(void) {
  char out[256];

  feed_input("1-2\n1.2.3\n-\n.\n\n--1\n12abc\n-4.5\n");
  start_capture();
  float result = ask_question_float("Float?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_DOUBLE_EQUAL(result, -4.5, 0.001);

  feed_input("7\n");
  start_capture();
  result = ask_question_float("Float?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_DOUBLE_EQUAL(result, 7.0, 0.001);

  feed_input("+.5\n");
  start_capture();
  result = ask_question_float("Float?");
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_DOUBLE_EQUAL(result, 0.5, 0.001);
}

void test_merch_display_end_of_input(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[1024];
  char name[16];

  for (int i = 1; i <= 25; i++) {
    snprintf(name, sizeof(name), "item%02d", i);
    add_merch(&ht, name, i);
  }

  feed_input("   \n");
  start_capture();
  merch_display(&ht);
  stop_capture(out, sizeof(out));
  restore_input();

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "item20"));
  CU_ASSERT_PTR_NULL(strstr(out, "item21"));

  clear_store(&ht);
}

////WEBSTORE TESTS////

void test_program_exit(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[128];

  add_merch(&ht, "Apple", 10);
  add_merch(&ht, "Pear", 20);
  ioopm_stock_replenish(&ht, "Apple", "A1", 5);

  int cart_id = create_cart();
  ioopm_cart_add_item(&ht, cart_id, "Apple", 2);

  int status = run_in_child(ioopm_program_exit, &ht, NULL, out, sizeof(out));

  CU_ASSERT_EQUAL(status, 0);
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Goodbye"));

  ioopm_cart_remove(cart_id);
  clear_store(&ht);
}

void test_event_loop_quit(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[4096];

  int status = run_in_child(event_loop, &ht, "q\n", out, sizeof(out));

  CU_ASSERT_EQUAL(status, 0);
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "(Q) Quit"));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Goodbye"));
}

void test_event_loop_end_of_input(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[4096];

  int status = run_in_child(event_loop, &ht, "   \n", out, sizeof(out));

  CU_ASSERT_EQUAL(status, 0);
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Goodbye"));
}

void test_event_loop_session(void) {
  ioopm_hash_table_t ht;
  ioopm_hash_table_init(&ht);
  char out[32768];
  char input[1024];

  int last_id = create_cart();
  ioopm_cart_remove(last_id);
  int first_cart = last_id + 1;
  int second_cart = last_id + 2;

  snprintf(input, sizeof(input),
           "A\nApple\nA red fruit\n10\n"
           "A\nPear\nGreen\n20\n"
           "P\nApple\nA1\n5\n"
           "P\nPear\nB1\n5\n"
           "L\n"
           "E\nPear\nPlum\nPurple\n30\n"
           "S\nApple\n"
           "C\n"
           "+\n%d\nApple\n3\n"
           "+\n%d\nPlum\n1\n"
           "-\n%d\nApple\n1\n"
           "=\n%d\n"
           "O\n%d\n"
           "C\n"
           "R\n%d\n"
           "U\n"
           "D\nApple\n"
           "Q\n",
           first_cart, first_cart, first_cart, first_cart, first_cart, second_cart);

  int status = run_in_child(event_loop, &ht, input, out, sizeof(out));

  char expected[64];

  CU_ASSERT_EQUAL(status, 0);

  snprintf(expected, sizeof(expected), "Total cost of cart %d: 50", first_cart);
  CU_ASSERT_PTR_NOT_NULL(strstr(out, expected));

  snprintf(expected, sizeof(expected), "Cart %d bought", first_cart);
  CU_ASSERT_PTR_NOT_NULL(strstr(out, expected));

  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Aisle: A1, Quantity: 5"));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Apple\nPear\n"));
  CU_ASSERT_PTR_NOT_NULL(strstr(out, "Goodbye"));
}

int main() {
    // First we try to set up CUnit, and exit if we fail
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // We then create an empty test suite and specify the name and
    // the init and cleanup functions
    CU_pSuite ht_test = CU_add_suite("Hash table init tests", init_suite, clean_suite);
    if (ht_test == NULL) {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite merch_add_tests = CU_add_suite("Merch add tests", init_suite, clean_suite);
    if (merch_add_tests == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite merch_remove_tests = CU_add_suite("Merch remove tests", init_suite, clean_suite);
    if (merch_remove_tests == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite merch_edit_tests = CU_add_suite("Merch edit tests", init_suite, clean_suite);
    if (merch_edit_tests == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite merch_display_tests = CU_add_suite("Merch display tests", init_suite, clean_suite);
    if (merch_display_tests == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite stock_tests = CU_add_suite("Stock tests", init_suite, clean_suite);
    if (stock_tests == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite cart_tests = CU_add_suite("Cart tests", init_suite, clean_suite);
    if (cart_tests == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite cost_tests = CU_add_suite("Cost calculation tests", init_suite, clean_suite);
    if (cost_tests == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite checkout_tests = CU_add_suite("Checkout tests", init_suite, clean_suite);
    if (checkout_tests == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite string_utils_tests = CU_add_suite("String utils tests", init_suite, clean_suite);
    if (string_utils_tests == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pSuite webstore_tests = CU_add_suite("Webstore tests", init_suite, clean_suite);
    if (webstore_tests == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // This is where we add the test functions to our test suite.
    // For each call to CU_add_test we specify the test suite, the
    // name or description of the test, and the function that runs
    // the test in question. If you want to add another test, just
    // copy a line below and change the information
    if (
    /// HASH TABLE TESTS
    (CU_add_test(ht_test, "Hash table init test", test_hash_table_init) == NULL) ||
    // MERCH ADD TESTS
    (CU_add_test(merch_add_tests, "Add once", test_merch_add_once) == NULL) ||
    (CU_add_test(merch_add_tests, "Add multiple", test_merch_add_multiple) == NULL) ||
    (CU_add_test(merch_add_tests, "Add duplicate name", test_merch_add_duplicate) == NULL) ||
    (CU_add_test(merch_add_tests, "Add negative price", test_merch_add_negative_price) == NULL) ||
    (CU_add_test(merch_add_tests, "Add zero price", test_merch_add_zero_price) == NULL) ||
    (CU_add_test(merch_add_tests, "Add with hash collision", test_merch_add_collision) == NULL) ||
    (CU_add_test(merch_add_tests, "Add when store is full", test_merch_add_store_full) == NULL) ||
    // MERCH REMOVE TESTS
    (CU_add_test(merch_remove_tests, "Remove once", test_merch_remove_once) == NULL) ||
    (CU_add_test(merch_remove_tests, "Remove missing", test_merch_remove_missing) == NULL) ||
    (CU_add_test(merch_remove_tests, "Remove one of many", test_merch_remove_one_of_many) == NULL) ||
    (CU_add_test(merch_remove_tests, "Remove with stock", test_merch_remove_with_stock) == NULL) ||
    (CU_add_test(merch_remove_tests, "Remove and add again", test_merch_remove_and_add_again) == NULL) ||
    // MERCH EDIT TESTS
    (CU_add_test(merch_edit_tests, "Edit all fields", test_merch_edit_all_fields) == NULL) ||
    (CU_add_test(merch_edit_tests, "Edit same name", test_merch_edit_same_name) == NULL) ||
    (CU_add_test(merch_edit_tests, "Edit missing", test_merch_edit_missing) == NULL) ||
    (CU_add_test(merch_edit_tests, "Edit negative price", test_merch_edit_negative_price) == NULL) ||
    (CU_add_test(merch_edit_tests, "Edit to name already taken", test_merch_edit_name_taken) == NULL) ||
    (CU_add_test(merch_edit_tests, "Edit keeps stock", test_merch_edit_keeps_stock) == NULL) ||
    (CU_add_test(merch_edit_tests, "Edit updates cart", test_merch_edit_updates_carts) == NULL) ||
    (CU_add_test(merch_edit_tests, "Edit updates many carts", test_merch_edit_updates_many_carts) == NULL) ||
    // MERCH DISPLAY TESTS
    (CU_add_test(merch_display_tests, "Display empty", test_merch_display_empty) == NULL) ||
    (CU_add_test(merch_display_tests, "Display sorted", test_merch_display_sorted) == NULL) ||
    (CU_add_test(merch_display_tests, "Display exactly one page", test_merch_display_exactly_one_page) == NULL) ||
    (CU_add_test(merch_display_tests, "Display stop after first page", test_merch_display_stop_after_first_page) == NULL) ||
    (CU_add_test(merch_display_tests, "Display continue to second page", test_merch_display_continue_to_second_page) == NULL) ||
    // STOCK TESTS
    (CU_add_test(stock_tests, "Show missing merch", test_stock_show_missing_merch) == NULL) ||
    (CU_add_test(stock_tests, "Show no stock", test_stock_show_no_stock) == NULL) ||
    (CU_add_test(stock_tests, "Show sorted by aisle", test_stock_show_sorted_by_aisle) == NULL) ||
    (CU_add_test(stock_tests, "Replenish new shelf", test_stock_replenish_new_shelf) == NULL) ||
    (CU_add_test(stock_tests, "Replenish same shelf", test_stock_replenish_same_shelf) == NULL) ||
    (CU_add_test(stock_tests, "Replenish multiple shelves", test_stock_replenish_multiple_shelves) == NULL) ||
    (CU_add_test(stock_tests, "Replenish missing merch", test_stock_replenish_missing_merch) == NULL) ||
    (CU_add_test(stock_tests, "Replenish bad amount", test_stock_replenish_bad_amount) == NULL) ||
    (CU_add_test(stock_tests, "Replenish shelf taken", test_stock_replenish_shelf_taken) == NULL) ||
    // CART TESTS
    (CU_add_test(cart_tests, "Create cart", test_cart_create) == NULL) ||
    (CU_add_test(cart_tests, "Remove cart", test_cart_remove_once) == NULL) ||
    (CU_add_test(cart_tests, "Remove missing cart", test_cart_remove_missing) == NULL) ||
    (CU_add_test(cart_tests, "Remove middle and last cart", test_cart_remove_middle_and_last) == NULL) ||
    (CU_add_test(cart_tests, "Remove cart with items", test_cart_remove_with_items) == NULL) ||
    (CU_add_test(cart_tests, "Add item once", test_cart_add_item_once) == NULL) ||
    (CU_add_test(cart_tests, "Add item twice", test_cart_add_item_twice) == NULL) ||
    (CU_add_test(cart_tests, "Add multiple merch", test_cart_add_item_multiple_merch) == NULL) ||
    (CU_add_test(cart_tests, "Add item bad quantity", test_cart_add_item_bad_quantity) == NULL) ||
    (CU_add_test(cart_tests, "Add item missing cart", test_cart_add_item_missing_cart) == NULL) ||
    (CU_add_test(cart_tests, "Add item missing merch", test_cart_add_item_missing_merch) == NULL) ||
    (CU_add_test(cart_tests, "Add item not enough stock", test_cart_add_item_not_enough_stock) == NULL) ||
    (CU_add_test(cart_tests, "Add item no stock at all", test_cart_add_item_no_stock_at_all) == NULL) ||
    (CU_add_test(cart_tests, "Add item reserved in other cart", test_cart_add_item_reserved_in_other_cart) == NULL) ||
    (CU_add_test(cart_tests, "Remove item partial", test_cart_remove_item_partial) == NULL) ||
    (CU_add_test(cart_tests, "Remove item all", test_cart_remove_item_all) == NULL) ||
    (CU_add_test(cart_tests, "Remove item first middle last", test_cart_remove_item_first_middle_last) == NULL) ||
    (CU_add_test(cart_tests, "Remove item errors", test_cart_remove_item_errors) == NULL) ||
    // COST CALUCLATION TESTS
    (CU_add_test(cost_tests, "Cost empty cart", test_cost_empty_cart) == NULL) ||
    (CU_add_test(cost_tests, "Cost multiple merch", test_cost_multiple_merch) == NULL) ||
    (CU_add_test(cost_tests, "Cost missing cart", test_cost_missing_cart) == NULL) ||
    (CU_add_test(cost_tests, "Cost after price change", test_cost_after_price_change) == NULL) ||
    (CU_add_test(cost_tests, "Cost removed merch", test_cost_removed_merch) == NULL) ||
    // CHECKOUT TESTS
    (CU_add_test(checkout_tests, "Checkout reduces stock", test_checkout_reduces_stock) == NULL) ||
    (CU_add_test(checkout_tests, "Checkout multiple shelves", test_checkout_multiple_shelves) == NULL) ||
    (CU_add_test(checkout_tests, "Checkout multiple merch", test_checkout_multiple_merch) == NULL) ||
    (CU_add_test(checkout_tests, "Checkout empty cart", test_checkout_empty_cart) == NULL) ||
    (CU_add_test(checkout_tests, "Checkout missing cart", test_checkout_missing_cart) == NULL) ||
    (CU_add_test(checkout_tests, "Checkout not enough stock", test_checkout_not_enough_stock) == NULL) ||
    (CU_add_test(checkout_tests, "Checkout removed merch", test_checkout_removed_merch) == NULL) ||
    (CU_add_test(checkout_tests, "Checkout two carts", test_checkout_two_carts) == NULL) ||
    // STRING UTILS TESTS
    (CU_add_test(string_utils_tests, "String length", test_string_length) == NULL) ||
    (CU_add_test(string_utils_tests, "Ask question int", test_ask_question_int) == NULL) ||
    (CU_add_test(string_utils_tests, "Ask question float", test_ask_question_float) == NULL) ||
    (CU_add_test(string_utils_tests, "Ask question string", test_ask_question_string) == NULL) ||
    (CU_add_test(string_utils_tests, "Ask question line", test_ask_question_line) == NULL) ||
    (CU_add_test(string_utils_tests, "Read string", test_read_string) == NULL) ||
    (CU_add_test(string_utils_tests, "Ask question float formats", test_ask_question_float_formats) == NULL) ||
    (CU_add_test(string_utils_tests, "Ask question end of input", test_ask_question_end_of_input) == NULL) ||
    (CU_add_test(merch_display_tests, "Display end of input", test_merch_display_end_of_input) == NULL) ||
    // WEBSTORE TESTS
    (CU_add_test(webstore_tests, "Ask question menu", test_ask_question_menu) == NULL) ||
    (CU_add_test(webstore_tests, "Ask question menu end of input", test_ask_question_menu_end_of_input) == NULL) ||
    (CU_add_test(webstore_tests, "Program exit", test_program_exit) == NULL) ||
    (CU_add_test(webstore_tests, "Event loop quit", test_event_loop_quit) == NULL) ||
    (CU_add_test(webstore_tests, "Event loop end of input", test_event_loop_end_of_input) == NULL) ||
    (CU_add_test(webstore_tests, "Event loop full session", test_event_loop_session) == NULL) ||

    0
)

        {
            // If adding any of the tests fails, we tear down CUnit and exit
            CU_cleanup_registry();  
            return CU_get_error();
        }

    // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
    // Use CU_BRM_NORMAL to only print errors and a summary
    CU_basic_set_mode(CU_BRM_VERBOSE);

    // This is where the tests are actually run!
    CU_basic_run_tests();

    // Tear down CUnit before exiting
    CU_cleanup_registry();
    return CU_get_error();
}