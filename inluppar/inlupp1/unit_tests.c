#include <CUnit/Basic.h>
#include "hash_table.h"
#include <stdbool.h>
#include <string.h>
#include "hash_table_struct.h"
#include "hash_table_iterator.h"

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
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  CU_ASSERT_PTR_NOT_NULL(ht);
  ioopm_hash_table_destroy(ht);
}


void test_ioopm_hash_table_create() {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  CU_ASSERT_PTR_NOT_NULL(ht);
}

void test_insert_once() {

  // create new hash table
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  char *key = "abc";
  int value = 123;

  // check that key is not in ht
  int result = 0;
  CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result, 0);

  // insert key-value pair and check that the mapping exists
  ioopm_hash_table_insert(ht, key, value);
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result, value);

  // destroy hash table
  ioopm_hash_table_destroy(ht);
}

void test_insert_twice() {

  // create new hash table
  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  char *key = "abc";
  int value = 123;

  // check that key is not in ht
  int result = 0;
  CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result, 0);

  // insert key-value pair and check that the mapping exists
  ioopm_hash_table_insert(ht, key, value);
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result, value);

  // change value but keep the same key
  int new_value = 124;

  ioopm_hash_table_insert(ht, key, new_value);
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result, 124);

  // destroy hash table
  ioopm_hash_table_destroy(ht);
}


void test_remove() {
  ioopm_hash_table_t *ht=ioopm_hash_table_create();

  char *key = "abc";
  int value=123;
  int result=0;
  ioopm_hash_table_insert(ht, key, value);


  CU_ASSERT_TRUE(ioopm_hash_table_lookup(ht, key, &result));
  CU_ASSERT_EQUAL(result, value);

  int removed_value = ioopm_hash_table_remove(ht, key);
  CU_ASSERT_EQUAL(removed_value, value);

  //key inte finns
  CU_ASSERT_FALSE(ioopm_hash_table_lookup(ht, key, &result));

  ioopm_hash_table_destroy(ht);
}

///HAS KEY TESTS////

void test_has_key_empty(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, "key"));

  ioopm_hash_table_destroy(ht);
}

void test_has_key_insert(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  ioopm_hash_table_insert(ht, "k", 123);

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, "k"));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, "k2"));

  ioopm_hash_table_destroy(ht);
}

void test_has_multiple_keys(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  ioopm_hash_table_insert(ht, "k1", 1);
  ioopm_hash_table_insert(ht, "k2", 2);
  ioopm_hash_table_insert(ht, "k3", 3);

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, "k1"));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, "k2"));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, "k3"));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, "k4"));

  ioopm_hash_table_destroy(ht);
}

void test_has_key_after_remove(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  ioopm_hash_table_insert(ht, "k", 123);
  ioopm_hash_table_remove(ht, "k");

  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, "k"));

  ioopm_hash_table_destroy(ht);
}

void test_has_key_after_remove_middle(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  ioopm_hash_table_insert(ht, "k1", 1);
  ioopm_hash_table_insert(ht, "k2", 2);
  ioopm_hash_table_insert(ht, "k3", 3);

  ioopm_hash_table_remove(ht, "k2");

  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, "k1"));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, "k2"));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, "k3"));

  ioopm_hash_table_destroy(ht);
}


////SIZE TESTS////

void test_size_empty(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 0);

  ioopm_hash_table_destroy(ht);
}

void test_size_single(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  ioopm_hash_table_insert(ht, "key", 123);

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 1);

  ioopm_hash_table_destroy(ht);
}

void test_size_multiple(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  ioopm_hash_table_insert(ht, "k1", 1);
  ioopm_hash_table_insert(ht, "k2", 2);
  ioopm_hash_table_insert(ht, "k3", 3);

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 3);

  ioopm_hash_table_destroy(ht);
}

void test_size_after_remove(void) {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  ioopm_hash_table_insert(ht, "k1", 1);
  ioopm_hash_table_insert(ht, "k2", 2);
  ioopm_hash_table_insert(ht, "k3", 3);

  ioopm_hash_table_remove(ht, "k2");

  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 2);

  ioopm_hash_table_destroy(ht);
}

//ITERATOR
void test_empty() {

  //gör först tom ht
  ioopm_hash_table_t *ht = ioopm_hash_table_create();
  //gör iterator
  ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(ht);
  //kolla att at_end blir true
  CU_ASSERT_TRUE(ioopm_hash_table_iterator_at_end(it));

  ioopm_hash_table_iterator_destroy(it);
  ioopm_hash_table_destroy(ht);
}

void test_one_entry() {

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  ioopm_hash_table_insert(ht, "AAA", 10);

  ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(ht);

  CU_ASSERT_FALSE(ioopm_hash_table_iterator_at_end(it));
  CU_ASSERT_STRING_EQUAL(ioopm_hash_table_iterator_current_key(it), "AAA");
  CU_ASSERT_EQUAL(ioopm_hash_table_iterator_current_value(it), 10);

  ioopm_hash_table_iterator_destroy(it);
  ioopm_hash_table_destroy(ht);
}

void test_multiple_entries() {
  
  char *keys[3] = {"abc", "qwe", "asd"};
  int values[3] = {0, 1, 2};

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  for (int i = 0; i != 3; ++i) {
    ioopm_hash_table_insert(ht, keys[i], values[i]);
  }

  int iteration_count = 0;

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

  ioopm_hash_table_t *ht = ioopm_hash_table_create();

  ioopm_hash_table_insert(ht, "abc", 0);
  ioopm_hash_table_insert(ht, "qwe", 1);
  ioopm_hash_table_insert(ht, "asd", 2);

  int abc_count =0;
  int qwe_count = 0;
  int asd_count = 0;

  ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(ht);

  while (!ioopm_hash_table_iterator_at_end(it)) {
    char *key = ioopm_hash_table_iterator_current_key(it);
    int value = ioopm_hash_table_iterator_current_value(it);

    if (strcmp(key, "abc") == 0) {
      abc_count++;
      CU_ASSERT_EQUAL(value, 0);
    }
    else if (strcmp(key, "qwe") == 0) {
      qwe_count++;
      CU_ASSERT_EQUAL(value, 1);
    }
    else if (strcmp(key, "asd") == 0) {
      asd_count++;
      CU_ASSERT_EQUAL(value, 2);
    }

    ioopm_hash_table_iterator_advance(it);
  }

  CU_ASSERT_EQUAL(abc_count, 1);
  CU_ASSERT_EQUAL(qwe_count, 1);
  CU_ASSERT_EQUAL(asd_count, 1);

  ioopm_hash_table_iterator_destroy(it);
  ioopm_hash_table_destroy(ht);
}

/*
void test_multiple_entries_same_bucket()
{
    //ja vet inte
}
*/


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