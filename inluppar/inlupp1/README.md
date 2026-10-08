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
        - Lines executed:100.00% of 75
        - Branches executed:100.00% of 20
        - Taken at least once:100.00% of 20

    - List_iterator.c:
        - Lines executed:100.00% of 18
        - Branches executed:100.00% of 2
        - Taken at least once:100.00% of 2

    - Linked_list.c:
        - Lines executed:97.80% of 91
        - Branches executed:100.00% of 22
        - Taken at least once:100.00% of 22

    - Hash_table_iterator.c:
        - Lines executed:100.00% of 33
        - Branches executed:100.00% of 8
        - Taken at least once:100.00% of 8

7. # Initial Profiling Results
    
    The program was run with input files of different sizes. 
    The code was profiled using Valgrind Callgrind, and the results were:

    - 1k-long-words.txt:
        1. ioopm_string_knr_hash – 7.91%
        2. find_previous_entry – 0.54%
        3. ioopm_string_eq – 0.24%

    - 10k-words.txt:
        1. find_previous_entry – 9.36%
        2. ioopm_string_knr_hash – 5.64%
        3. ioopm_string_eq – 5.39%

    - 16k-words.txt:
        1. find_previous_entry – 25.08%
        2. ioopm_string_eq – 16.48%
        3. ioopm_string_knr_hash – 1.42%
    
    The results are not exactly the same for all inputs. As the input gets larger, find_previous_entry becomes a much larger part of the total number of references. ioopm_string_eq also becomes
    more significant for the larger inputs.

    This makes sense, and was somewhat expected, because a larger input causes the hash table to perform more searches and comparisons. 

    Based on these results, find_previous_entry seems like a function to investigate for further optimization.