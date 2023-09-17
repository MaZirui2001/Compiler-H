
# sort rely on sort.o
sort: sort.o
	gcc sort.o -o sort

# sort.o rely on sort.s
sort.o: sort.s
	gcc -c sort.s -o sort.o

# sort.s rely on sort.i
sort.s: sort.i
	gcc -S sort.i -o sort.s

# sort.i rely on sort.c
sort.i: sort.c
	gcc -E sort.c -o sort.i

# gcc compile
gcc-32:
	gcc -S -m32 sort.c -o sort-gcc-32.s
gcc-64:
	gcc -S -m64 sort.c -o sort-gcc-64.s
# clang compile
clang-32:
	clang -S -m32 sort.c -o sort-clang-32.s
clang-64:
	clang -S -m64 sort.c -o sort-clang-64.s
	
clean:
	rm sort.i sort.o sort sort.s -rf