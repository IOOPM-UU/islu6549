1. # Make Targets
    - Run unit tests: make test
    - Run unit_tests through Valgrind: make valgrindtest
    - Clean files: make clean 
    - Run and compile freq-count: make freq
    - Coverage with gcov: make coverage

2. # Hash Tables

    - CREATING A HASH TABLE
        - Hash tables consist of an array of buckets. 
            - A bucket is a linked list of entries
                    - An entry is a struct consisting of a key, value and a pointer to the next entry.
        - ioopm_hash_table_create(hash_fn, key_eq_fn) creates an empty hash table with a function that calculates the hash value for an element (hash_fn), as well as a function that compares the key(key_eq_fn).

    - DOCUMENTATION
        - All hashtable functions are documented in hash_table.h

    - MEMORY USAGE
        - The hashtable is created using calloc() in ioopm_create_hashtable(). 
        - Each entry is created using malloc() in a separate, static, function called entry_create.
        - To avoid leaks, the hashtable as well as every entry has to be freed using free().
        - When destroying a hashtable, each entry is freed in the entry_destroy() function. Then the hashtable is freed.

    - ERROR HANDLING
        - A lookup for a key that does not exist returns false.
        - Removing a key that does not exist returns int_elem(0).
        - Memory allocation failures are not explicitly handled.


3. # Linked Lists
    
    - CREATING A LINKED LIST
        - A list consists of its first and last link, as well as the size of the list (stored as an int).
            - A link consists of its current element as well as a pointer to the next link.

    - DOCUMENTATION
        - All functions regarding linked lists are documented in the linked_list.h file.

    - MEMORY USAGE
        - Each list is created in ioopm_list_create() using calloc().
        - Each link is created in link_create(elem, next) using malloc().
        - link_destroy(link) frees an individual link.
        - To destory the entire list, each link is destroyd using link_destroy(), and then the list is freed.

    - ERROR HANDLING
        - The list functions do not explicitly handle invalid indices.
        - Memory allocation failures are not explicitly handled.


4. # Hashtable Iterators

    - CREATING A HASHTABLE ITERATOR
        - A hashtable iterator consists of a pointer to a hashtable, an int representing the current bucket as well as a ptr to the "current" entry.
        - A hashtable iterator is created in the hash_table_iterator_create() function.
        - When creating a hash table iterator - the function skip_sentinel_nodes() is used to ensure that the "current" entry isn't a sentinel node.

    - DOCUMENTATION
        - All functions regarding hashtable iterators are documented in hash_table_iterator.h

    - MEMORY USAGE
        - The iterator is created using malloc() and freed in ioopm_hash_table_iterator_destroy() using free().

    - ERROR HANDLING
        - The iterator must be positioned at an entry when using advance, current_key or current_value.


5. # List Iterators

    - CREATING A LIST ITERATOR
        - A list iterator consists of a a ptr to a linked list, as well as a ptr to the "current" element. 
        - A list iterator is created in the list_iterator_create() function.

    - DOCUMENTATION
        - All functions related to list iterators are documented in list_iterator.h

    - MEMORY USAGE
        - The list iterator is created using calloc() and freed in ioopm_list_iterator_destroy() using free().

    - ERROR HANDLING
        - The iterator must be positioned at an entry when using current or advance.


6. # TEST COVERAGE

    - Hash_table.c:
        - Lines executed: 96.39% of 83
        - Branches executed: 100.00% of 24
        - Taken at least once: 100.00% of 24
    
    - Hash_table_iterator.c:
        - Lines executed: 100.00% of 33
        - Branches executed: 100.00% of 8
        - Taken at least once: 100.00% of 8

    - Linked_list.c:
        - Lines executed: 91.01% of 89
        - Branches executed: 100.00% of 24
        - Taken at least once: 87.50% of 24
    
    - List_iterator.c:
        - Lines executed: 100.00% of 18
        - Branches executed: 100.00% of 4
        - Taken at least once: 75.00% of 4

7. # Initial Profiling Results
    
    The program was profiled using Valgrind Callgrind, and the results were inspected using callgrind_annotate. The percentages represent individual instruction cost:

    - 1k-words.txt containing 5k words:
        1. string_to_hash - 16.60%
        2. find_previous_entry - 10.35%
        3. ioopm_string_eq - 2.93%

    - 10k-words.txt containing 50k words:
        1. string_to_hash - 17.00%
        2. find_previous_entry - 10.60%
        3. ioopm_string_eq - 3.00%

    - 16k-words.txt containing 80k words:
        1. string_to_hash - 17.02%
        2. find_previous_entry - 10.61%
        3. ioopm_string_eq - 3.00%
    
    The results are very similar across the the input files. With string_to_hash consistnently having the highest instruction cost, followed by find_previous_entry and lastly ioopm_string_eq.
    
    As the input size gets larger, the total number of instructions increases, while keeping the relative cost more or less the same. This suggests the costs scale proportionally with the amount of words needed to be scanned

    Based on these results, if optimization were needed, string_to_hash along with find_previous_entry are worth investigating.