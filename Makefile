src_files = shared_structures.c malloc.c free.c calloc.c \
	realloc.c wrappers.c malloc_stats.c 
hdr_files = shared_structures.h malloc.h free.h calloc.h \
	realloc.h wrappers.h malloc_stats.h
flags = -fPIC -lpthread -Wno-implicit-function-declaration -g

lib: $(src_files) $(hdr_files)
	gcc -shared -o libmalloc.so $(src_files) $(flags)

shared_structures: shared_structures.c shared_structures.h
	gcc -c -o shared_structures.o shared_structures.c $(flags)

malloc: malloc.c malloc.h shared_structures.c shared_structures.h
	gcc -c -o malloc.o malloc.c $(flags)

free: free.c free.h shared_structures.c shared_structures.h
	gcc -c -o free.o free.c $(flags)

calloc: calloc.c calloc.h shared_structures.c shared_structures.h
	gcc -c -o calloc.o calloc.c $(flags)

realloc: realloc.c realloc.h shared_structures.c shared_structures.h
	gcc -c -o realloc.o realloc.c $(flags)

malloc_stats: malloc_stats.c malloc_stats.h shared_structures.c shared_structures.h
	gcc -c -o malloc_stats.o malloc_stats.c $(flags)

test: test.c
	gcc -o test test.c -g

t-test: t-test.c
	gcc -o t-test t-test.c -lpthread -g

f-test: f-test.c
	gcc -o f-test f-test.c -lpthread -g

clean:
	rm -f libmalloc.so *.o test t-test f-test