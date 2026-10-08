#include <CUnit/Basic.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#include "common.h"
#include "hash_table.h"
#include "hash_table_struct.h"
#include "hash_table_iterator.h"
#include "linked_list.h"
#include "list_iterator.h"

size_t string_hash(elem_t key)
{
    size_t result = 0;

    while (*key.s != '\0')
    {
        result = result * 31 + (unsigned char)*key.s;
        key.s++;
    }

    return result;
}

bool string_eq(elem_t a, elem_t b)
{
    return strcmp(a.s, b.s) == 0;
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

// These are example test functions. You should replace them with
// functions of your own.
void test_create_destroy() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);
  CU_ASSERT_PTR_NOT_NULL(ht);
  ioopm_hash_table_destroy(ht);
}


void test_ioopm_hash_table_create() {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);
  CU_ASSERT_PTR_NOT_NULL(ht);
  ioopm_hash_table_destroy(ht);
}

void test_insert_once() {

  // create new hash table
  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  elem_t key = string_elem("abc");
  elem_t value = int_elem(123);

  // check that key is not in ht
  elem_t result = ioopm_hash_table_lookup(ht, key);
  CU_ASSERT_EQUAL(result.i, 0);

  // insert key-value pair and check that the mapping exists
  ioopm_hash_table_insert(ht, key, value);

  result = ioopm_hash_table_lookup(ht, key);
  CU_ASSERT_EQUAL(result.i, value.i);

  // destroy hash table
  ioopm_hash_table_destroy(ht);
}

void test_insert_twice() {

  // create new hash table
  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  elem_t key = string_elem("abc");
  elem_t value = int_elem(123);

  // check that key is not in ht
  elem_t result = ioopm_hash_table_lookup(ht, key);
  CU_ASSERT_EQUAL(result.i, 0);

  // insert key-value pair and check that the mapping exists
  ioopm_hash_table_insert(ht, key, value);
  result = ioopm_hash_table_lookup(ht, key);
  CU_ASSERT_EQUAL(result.i, value.i);

  // change value but keep the same key
  elem_t new_value = int_elem(124);

  ioopm_hash_table_insert(ht, key, new_value);
  result = ioopm_hash_table_lookup(ht, key);
  CU_ASSERT_EQUAL(result.i, 124);

  // destroy hash table
  ioopm_hash_table_destroy(ht);
}


void test_remove() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  elem_t key = string_elem("abc");
  elem_t value = int_elem(123);

  ioopm_hash_table_insert(ht, key, value);


  elem_t result = ioopm_hash_table_lookup(ht, key);
  CU_ASSERT_EQUAL(result.i, value.i);

  elem_t removed_value = ioopm_hash_table_remove(ht, key);
  CU_ASSERT_EQUAL(removed_value.i, value.i);

  //key inte finns
  result = ioopm_hash_table_lookup(ht, key);
  CU_ASSERT_EQUAL(result.i, 0);

  ioopm_hash_table_destroy(ht);
}

///HAS KEY TESTS////

void test_has_key_empty(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, string_elem("key")));

  ioopm_hash_table_destroy(ht);
}

void test_has_key_insert(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  ioopm_hash_table_insert(ht, string_elem("k"), int_elem(123));

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("k")));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, string_elem("k2")));

  ioopm_hash_table_destroy(ht);
}

void test_has_multiple_keys(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  ioopm_hash_table_insert(ht, string_elem("k1"), int_elem(1));
  ioopm_hash_table_insert(ht, string_elem("k2"), int_elem(2));
  ioopm_hash_table_insert(ht, string_elem("k3"), int_elem(3));

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("k1")));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("k2")));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("k3")));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, string_elem("k4")));

  ioopm_hash_table_destroy(ht);
}

void test_has_key_after_remove(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  ioopm_hash_table_insert(ht, string_elem("k"), int_elem(123));
  ioopm_hash_table_remove(ht, string_elem("k"));

  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, string_elem("k")));

  ioopm_hash_table_destroy(ht);
}

void test_has_key_after_remove_middle(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  ioopm_hash_table_insert(ht, string_elem("k1"), int_elem(1));
  ioopm_hash_table_insert(ht, string_elem("k2"), int_elem(2));
  ioopm_hash_table_insert(ht, string_elem("k3"), int_elem(3));

  ioopm_hash_table_remove(ht, string_elem("k2"));

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("k1")));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, string_elem("k2")));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, string_elem("k3")));

  ioopm_hash_table_destroy(ht);
}

void test_remove_missing_key(void)
{
  ioopm_hash_table_t *ht =
      ioopm_hash_table_create(ioopm_string_hash, ioopm_string_eq);

  elem_t result = ioopm_hash_table_remove(ht, string_elem("missing"));

  CU_ASSERT_EQUAL(result.i, 0);
  ioopm_hash_table_destroy(ht);
}


////SIZE TESTS////

void test_size_empty(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 0);

  ioopm_hash_table_destroy(ht);
}

void test_size_single(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  ioopm_hash_table_insert(ht, string_elem("key"), int_elem(123));

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 1);

  ioopm_hash_table_destroy(ht);
}

void test_size_multiple(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  ioopm_hash_table_insert(ht, string_elem("k1"), int_elem(1));
  ioopm_hash_table_insert(ht, string_elem("k2"), int_elem(2));
  ioopm_hash_table_insert(ht, string_elem("k3"), int_elem(3));

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 3);

  ioopm_hash_table_destroy(ht);
}

void test_size_after_remove(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  ioopm_hash_table_insert(ht, string_elem("k1"), int_elem(1));
  ioopm_hash_table_insert(ht, string_elem("k2"), int_elem(2));
  ioopm_hash_table_insert(ht, string_elem("k3"), int_elem(3));

  ioopm_hash_table_remove(ht, string_elem("k2"));

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 2);

  ioopm_hash_table_destroy(ht);
}

//ITERATOR
void test_empty() {

  //gör först tom ht
  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);
  //gör iterator
  ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(ht);
  //kolla att at_end blir true
  CU_ASSERT_TRUE(ioopm_hash_table_iterator_at_end(it));

  ioopm_hash_table_iterator_destroy(it);
  ioopm_hash_table_destroy(ht);
}

void test_one_entry() {

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  ioopm_hash_table_insert(ht, string_elem("AAA"), int_elem(10));

  ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(ht);

  CU_ASSERT_FALSE(ioopm_hash_table_iterator_at_end(it));

  elem_t key = ioopm_hash_table_iterator_current_key(it);
  elem_t value = ioopm_hash_table_iterator_current_value(it);

  CU_ASSERT_STRING_EQUAL(key.s, "AAA");
  CU_ASSERT_EQUAL(value.i, 10);

  ioopm_hash_table_iterator_destroy(it);
  ioopm_hash_table_destroy(ht);
}

void test_multiple_entries() {
  
  char *keys[3] = {"abc", "qwe", "asd"};
  int values[3] = {0, 1, 2};

  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  for (size_t i = 0; i < 3; ++i) {
    ioopm_hash_table_insert(ht,
    string_elem(keys[i]),
    int_elem(values[i]));
  }

  size_t iteration_count = 0;

  ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(ht);

  while (!ioopm_hash_table_iterator_at_end(it)) {
    iteration_count++;
    ioopm_hash_table_iterator_advance(it);
  }

  ioopm_hash_table_iterator_destroy(it);
  ioopm_hash_table_destroy(ht);

  CU_ASSERT_EQUAL(iteration_count, 3);
}


void test_every_entry_once() {
  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  ioopm_hash_table_insert(ht, string_elem("abc"), int_elem(0));
  ioopm_hash_table_insert(ht, string_elem("qwe"), int_elem(1));
  ioopm_hash_table_insert(ht, string_elem("asd"), int_elem(2));

  size_t abc_count = 0;
  size_t qwe_count = 0;
  size_t asd_count = 0;

  ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(ht);

  while (!ioopm_hash_table_iterator_at_end(it))
  {
    elem_t key = ioopm_hash_table_iterator_current_key(it);
    elem_t value = ioopm_hash_table_iterator_current_value(it);

    if (strcmp(key.s, "abc") == 0) {
        abc_count++;
        CU_ASSERT_EQUAL(value.i, 0);
      } else if (strcmp(key.s, "qwe") == 0) {
        qwe_count++;
        CU_ASSERT_EQUAL(value.i, 1);
      }
      else if (strcmp(key.s, "asd") == 0) {
        asd_count++;
        CU_ASSERT_EQUAL(value.i, 2);
      }

      ioopm_hash_table_iterator_advance(it);
  }

  CU_ASSERT_EQUAL(abc_count, 1);
  CU_ASSERT_EQUAL(qwe_count, 1);
  CU_ASSERT_EQUAL(asd_count, 1);

  ioopm_hash_table_iterator_destroy(it);
  ioopm_hash_table_destroy(ht);
}

void test_multiple_entries_same_bucket(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, string_eq);

  char *keys[] = {
    "k0", "k1", "k2", "k3", "k4", "k5",
    "k6", "k7", "k8", "k9", "k10", "k11",
    "k12", "k13", "k14", "k15", "k16", "k17"
  };

  for (size_t i = 0; i < 18; i++) {
    ioopm_hash_table_insert(ht,
    string_elem(keys[i]),
    int_elem(i));
  }

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 18);

  ioopm_hash_table_destroy(ht);
}

///LINKED LISTS////

void test_create_destroy_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  CU_ASSERT_PTR_NOT_NULL(list);

  ioopm_list_destroy(list);
}


void test_append_once_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_append(list, int_elem(3));

  CU_ASSERT_EQUAL(ioopm_list_head(list).i, 3);

  ioopm_list_destroy(list);
}


void test_append_multiple_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_append(list, int_elem(3));
  ioopm_list_append(list, int_elem(4));
  ioopm_list_append(list, int_elem(5));

  CU_ASSERT_EQUAL(ioopm_list_head(list).i, 3);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 1).i, 4);
  CU_ASSERT_EQUAL(ioopm_list_last(list).i, 5);

  ioopm_list_destroy(list);
}


void test_prepend_once_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_prepend(list, int_elem(3));

  CU_ASSERT_EQUAL(ioopm_list_head(list).i, 3);

  ioopm_list_destroy(list);
}


void test_prepend_multiple_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_prepend(list, int_elem(3));
  ioopm_list_prepend(list, int_elem(2));
  ioopm_list_prepend(list, int_elem(1));

  CU_ASSERT_EQUAL(ioopm_list_head(list).i, 1);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 1).i, 2);

  ioopm_list_destroy(list);
}


void test_prepend_append_multiple_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_prepend(list, int_elem(3));
  ioopm_list_append(list, int_elem(4));
  ioopm_list_prepend(list, int_elem(2));
  ioopm_list_prepend(list, int_elem(1));
  ioopm_list_append(list, int_elem(5));

  CU_ASSERT_EQUAL(ioopm_list_head(list).i, 1);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 1).i, 2);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 2).i, 3);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 3).i, 4);
  CU_ASSERT_EQUAL(ioopm_list_last(list).i, 5);

  ioopm_list_destroy(list);
}


void test_insert_once_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_insert(list, 0, int_elem(1));

  CU_ASSERT_EQUAL(ioopm_list_head(list).i, 1);

  ioopm_list_destroy(list);
}


void test_insert_replace_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_insert(list, 0, int_elem(11));
  ioopm_list_insert(list, 1, int_elem(22));
  ioopm_list_insert(list, 0, int_elem(33));

  CU_ASSERT_EQUAL(ioopm_list_head(list).i, 33);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 1).i, 11);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 2).i, 22);

  ioopm_list_destroy(list);
}


void test_insert_multiple_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_insert(list, 0, int_elem(11));
  ioopm_list_insert(list, 1, int_elem(22));
  ioopm_list_insert(list, 2, int_elem(33));

  CU_ASSERT_EQUAL(ioopm_list_head(list).i, 11);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 1).i, 22);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 2).i, 33);

  ioopm_list_destroy(list);
}


void test_remove_first_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_append(list, int_elem(1));
  ioopm_list_append(list, int_elem(2));

  ioopm_list_remove(list, 0);

  CU_ASSERT_EQUAL(ioopm_list_head(list).i, 2);

  ioopm_list_destroy(list);
}


void test_remove_multiple_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_append(list, int_elem(1));
  ioopm_list_append(list, int_elem(2));
  ioopm_list_append(list, int_elem(3));
  ioopm_list_append(list, int_elem(4));

  ioopm_list_remove(list, 0);
  ioopm_list_remove(list, 2);

  CU_ASSERT_EQUAL(ioopm_list_head(list).i, 2);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 1).i, 3);

  ioopm_list_destroy(list);
}


void test_list_get_multiple(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_append(list, int_elem(1));
  ioopm_list_append(list, int_elem(2));
  ioopm_list_append(list, int_elem(3));

  CU_ASSERT_EQUAL(ioopm_list_get(list, 0).i, 1);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 1).i, 2);
  CU_ASSERT_EQUAL(ioopm_list_get(list, 2).i, 3);

  ioopm_list_destroy(list);
}


void test_list_get_once(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_append(list, int_elem(1));

  CU_ASSERT_EQUAL(ioopm_list_get(list, 0).i, 1);

  ioopm_list_destroy(list);
}


void test_empty_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  CU_ASSERT_TRUE(ioopm_list_is_empty(list));

  ioopm_list_destroy(list);
}


void test_non_empty_list(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_append(list, int_elem(1));

  CU_ASSERT_FALSE(ioopm_list_is_empty(list));

  ioopm_list_destroy(list);
}

///LISTITERATOR TESTS///

void test_list_iterator_create(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_append(list, int_elem(10));
  ioopm_list_append(list, int_elem(20));

  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);

  CU_ASSERT_PTR_NOT_NULL(it);
  CU_ASSERT_EQUAL(ioopm_list_iterator_current(it).i, 10);

  ioopm_list_iterator_destroy(it);
  ioopm_list_destroy(list);
}


void test_list_iterator_end(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);

  CU_ASSERT_TRUE(ioopm_list_iterator_at_end(it));

  ioopm_list_iterator_destroy(it);
  ioopm_list_destroy(list);

  list = ioopm_list_create();
  ioopm_list_append(list, int_elem(1));

  it = ioopm_list_iterator_create(list);

  CU_ASSERT_FALSE(ioopm_list_iterator_at_end(it));

  ioopm_list_iterator_destroy(it);
  ioopm_list_destroy(list);
}


void test_list_iterator_advance(void)
{
  ioopm_list_t *list = ioopm_list_create();

  ioopm_list_append(list, int_elem(1));
  ioopm_list_append(list, int_elem(2));
  ioopm_list_append(list, int_elem(3));

  ioopm_list_iterator_t *it = ioopm_list_iterator_create(list);

  CU_ASSERT_EQUAL(ioopm_list_iterator_current(it).i, 1);

  ioopm_list_iterator_advance(it);

  CU_ASSERT_EQUAL(ioopm_list_iterator_current(it).i, 2);

  ioopm_list_iterator_destroy(it);
  ioopm_list_destroy(list);
}


int main() {
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite ht_test = CU_add_suite("Basic tests", init_suite, clean_suite);
  if (ht_test == NULL) {
      // If the test suite could not be added, tear down CUnit and exit
      CU_cleanup_registry();
      return CU_get_error();
  }

   CU_pSuite has_key_tests = CU_add_suite("Has_key tests", init_suite, clean_suite);
  if (ht_test == NULL) {
      // If the test suite could not be added, tear down CUnit and exit
      CU_cleanup_registry();
      return CU_get_error();
  }

  CU_pSuite size_tests = CU_add_suite("Size tests", init_suite, clean_suite);
  if (ht_test == NULL) {
      // If the test suite could not be added, tear down CUnit and exit
      CU_cleanup_registry();
      return CU_get_error();
  }

  CU_pSuite iterator_tests = CU_add_suite("Iterator tests", init_suite, clean_suite);
  if (ht_test == NULL) {
      // If the test suite could not be added, tear down CUnit and exit
      CU_cleanup_registry();
      return CU_get_error();
  }

  CU_pSuite list_tests = CU_add_suite("List tests", init_suite, clean_suite);
  if (list_tests == NULL) {
      CU_cleanup_registry();
      return CU_get_error();
  }

  CU_pSuite list_iterator_tests = CU_add_suite("List iterator tests", init_suite, clean_suite);
  if (list_iterator_tests == NULL) {
      CU_cleanup_registry();
      return CU_get_error();
  }
  // This is where we add the test functions to our test suite.
  // For each call to CU_add_test we specify the test suite, the
  // name or description of the test, and the function that runs
  // the test in question. If you want to add another test, just
  // copy a line below and change the information
if (
  (CU_add_test(ht_test, "Hash table insert once test", test_insert_once) == NULL) ||
  (CU_add_test(ht_test, "Key inserted twice with different values", test_insert_twice) == NULL) ||
  (CU_add_test(ht_test, "Hash table remove test", test_remove) == NULL) ||
  (CU_add_test(has_key_tests, "Has key empty test", test_has_key_empty) == NULL) ||
  (CU_add_test(has_key_tests, "Has key insert test", test_has_key_insert) == NULL) ||
  (CU_add_test(has_key_tests, "Has multiple keys test", test_has_multiple_keys) == NULL) ||
  (CU_add_test(has_key_tests, "Has key after remove test", test_has_key_after_remove) == NULL) ||
  (CU_add_test(has_key_tests, "Has key after removing middle key", test_has_key_after_remove_middle) == NULL) ||
  (CU_add_test(size_tests, "Size empty test", test_size_empty) == NULL) ||
  (CU_add_test(size_tests, "Size single test", test_size_single) == NULL) ||
  (CU_add_test(size_tests, "Size multiple test", test_size_multiple) == NULL) ||
  (CU_add_test(size_tests, "Size after remove test", test_size_after_remove) == NULL) ||
  (CU_add_test(iterator_tests, "Iterator empty test", test_empty) == NULL) ||
  (CU_add_test(iterator_tests, "Iterator one entry test", test_one_entry) == NULL) ||
  (CU_add_test(iterator_tests, "Iterator multiple entries", test_multiple_entries) == NULL) ||
  (CU_add_test(iterator_tests, "Iterator every entry once", test_every_entry_once) == NULL) ||

  (CU_add_test(list_tests, "Create and destroy list", test_create_destroy_list) == NULL) ||
  (CU_add_test(list_tests, "Append once", test_append_once_list) == NULL) ||
  (CU_add_test(list_tests, "Append multiple", test_append_multiple_list) == NULL) ||
  (CU_add_test(list_tests, "Prepend once", test_prepend_once_list) == NULL) ||
  (CU_add_test(list_tests, "Prepend multiple", test_prepend_multiple_list) == NULL) ||
  (CU_add_test(list_tests, "Prepend and append multiple", test_prepend_append_multiple_list) == NULL) ||
  (CU_add_test(list_tests, "Insert once", test_insert_once_list) == NULL) ||
  (CU_add_test(list_tests, "Insert replace", test_insert_replace_list) == NULL) ||
  (CU_add_test(list_tests, "Insert multiple", test_insert_multiple_list) == NULL) ||
  (CU_add_test(list_tests, "Remove first", test_remove_first_list) == NULL) ||
  (CU_add_test(list_tests, "Remove multiple", test_remove_multiple_list) == NULL) ||
  (CU_add_test(list_tests, "Get multiple", test_list_get_multiple) == NULL) ||
  (CU_add_test(list_tests, "Get once", test_list_get_once) == NULL) ||
  (CU_add_test(list_tests, "Empty list", test_empty_list) == NULL) ||
  (CU_add_test(list_tests, "Non-empty list", test_non_empty_list) == NULL) ||
  (CU_add_test(list_iterator_tests, "Iterator create", test_list_iterator_create) == NULL) ||
  (CU_add_test(list_iterator_tests, "Iterator end", test_list_iterator_end) == NULL) ||
  (CU_add_test(list_iterator_tests, "Iterator advance", test_list_iterator_advance) == NULL) ||
  //(CU_add_test(iterator_tests, "Multiple entries same bucket", test_multiple_entries_same_bucket) == NULL ||

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