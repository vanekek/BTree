CFLAGS = -fprofile-arcs -ftest-coverage
all: tests
tests: tests.o btree.o
	gcc $(CFLAGS) btree.o tests.o -o tests 
tests.o: tests.c 
	gcc $(CFLAGS) -c tests.c -o tests.o 
stack.o: btree.c
	gcc $(CFLAGS) -c btree.c -o btree.o 
clean: 
	rm -rf tests stack *.o *.gcov *.gcda *.gcno
