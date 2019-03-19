#include <sched.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef SHARED_STRUCTURES_DEC
#define SHARED_STRUCTURES_DEC

struct mem_seg {
	void * start; //point to start of actual region, needed for memalign
	size_t arena;
	size_t len;
	LIST_ENTRY (mem_seg) con;
};

struct core_arena {
	pthread_mutex_t data_lock; //mutex for this arena
	//Bins, same format as hw4
	LIST_HEAD (free_list_64,mem_seg) free_bin_64;
	LIST_HEAD (free_list_128,mem_seg) free_bin_128;
	LIST_HEAD (free_list_512,mem_seg) free_bin_512;
	LIST_HEAD (alloc_list,mem_seg) alloc_bin;

	//malloc_stats counters
	size_t num_malloc_reqs_0;
	size_t num_malloc_reqs_64;
	size_t num_malloc_reqs_128;
	size_t num_malloc_reqs_512;
	size_t num_malloc_reqs_big;

	size_t num_free_reqs_64;
	size_t num_free_reqs_128;
	size_t num_free_reqs_512;
	size_t num_free_reqs_big;
};

//generic list head used to make typing simpler
LIST_HEAD (mem_seg_list,mem_seg) generic; 

extern struct core_arena *arena_list;
extern size_t n_arenas;

//every thread gets a pointer to its assigned areana the first time it
//calls malloc
extern __thread struct core_arena *my_arena;
extern __thread size_t my_arena_n;

size_t PAGE_SIZE;
size_t MEM_SEG_OFFSET;
size_t ARENA_SIZE;

pthread_mutex_t break_region_lock; //controls access to break region ptrs

void * BREAK_PTR; //End of the program memory
void * START_ALC; //Beginning of the small allocation blocks
void * END_ALC;   //End of the small allocation blocks

pthread_mutex_t print_lock;
char printBuff[1024]; //string buffer for print messages

// __attribute__((constructor)) 
void INIT();
void lock_all(void);
void unlock_all(void);

#endif