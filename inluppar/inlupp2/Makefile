CC      = gcc
CFLAGS  = -std=gnu11 -Wall -Wextra -g --coverage -MMD -MP
LDFLAGS = --coverage

LIB_SRC = UI_Func.c string_utils.c
LIB_OBJ = $(LIB_SRC:.c=.o)

.PHONY: all test coverage valgrind clean

all: webstore

webstore: $(LIB_OBJ) webstore.o
	$(CC) $(LDFLAGS) $^ -o $@

tests: $(LIB_OBJ) webstore_lib.o unit_tests.o
	$(CC) $(LDFLAGS) $^ -o $@ -lcunit

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


webstore_lib.o: webstore.c
	$(CC) $(CFLAGS) -Dmain=webstore_main -c $< -o $@

-include $(wildcard *.d)

test: tests
	@rm -f *.gcda
	./tests

valgrind: tests
	@rm -f *.gcda
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./tests

coverage: test
	gcov -b -c $(LIB_SRC) webstore_lib.o

clean:
	rm -f *.o *.d *.gcda *.gcno *.gcov webstore tests