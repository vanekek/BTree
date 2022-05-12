FLAGS = -fprofile-arcs -ftest-coverage
all: a.out

a.out: tests.o btree.o
	gcc $(FLAGS) btree.o tests.o -o a.out
tests.o: tests.c
	gcc $(FLAGS) -c tests.c -o tests.o
btree.o: btree.c
	gcc $(FLAGS) -c btree.c -o btree.o 
clean:
	rm -rf a.out tests btree *.o *.gcov *.gcda *.gcno
