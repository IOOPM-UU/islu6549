CC = gcc
CFLAGS = -Wall -g
CUNIT = -lcunit

unit_tests: unit_tests.c hash_table.c hash_table_iterator.c linked_list.c
	$(CC) $(CFLAGS) $(CUNIT_INCLUDE) unit_tests.c hash_table.c hash_table_iterator.c linked_list.c list_iterator.c hash_functions.c $(CUNIT_LIB) $(CUNIT) -o unit_tests

test: unit_tests
	./unit_tests

freq: freq-count
	gcc -Wall -g freq_count.c hash_table.c hash_table_iterator.c linked_list.c list_iterator.c -o freq-count
	./freq-count test.txt

valgrindtest: unit_tests
	valgrind ./unit_tests

clean:
	rm -f unit_tests

coverage:
	rm -f *.gcda *.gcno *.gcov
	gcc -Wall -g --coverage unit_tests.c hash_table.c hash_table_iterator.c linked_list.c linked_list_iterator.c -lcunit -o unit_tests
	./unit_tests
	gcov -b unit_tests-hash_table.gcno
	gcov -b unit_tests-hash_table_iterator.gcno
	gcov -b unit_tests-linked_list.gcno
	gcov -b unit_tests-linked_list_iterator.gcno