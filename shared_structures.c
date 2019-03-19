#include "shared_structures.h"

struct core_arena *arena_list = NULL;
size_t n_arenas = 1;
__thread struct core_arena *my_arena = NULL;
__thread size_t	my_arena_n = 0;

// __attribute__((constructor)) 
void INIT() {

	// my_arena=NULL;

	void *p;
	p = sbrk(0);
	if ( p==(void *)-1 ) _exit(0); //couldn't sbrk
	BREAK_PTR=START_ALC=END_ALC=p;
	//get page size
	PAGE_SIZE=sysconf(_SC_PAGESIZE);
	if (PAGE_SIZE < 1) {
		_exit(0);
	}
	//calc offset for memory segments st data area starts on 8B boundary
	MEM_SEG_OFFSET=sizeof(struct mem_seg) + (8-(sizeof(struct mem_seg)%8));

	ARENA_SIZE=sizeof(struct core_arena);
	n_arenas = get_nprocs();

	//number bytes needed for the arena list to end on 8B boundary
	size_t num_bytes_for_arena_list = 
		ARENA_SIZE*n_arenas + (8-(ARENA_SIZE*n_arenas)%8);
	//expand break area to allocate arena list
	while (BREAK_PTR - START_ALC < num_bytes_for_arena_list){
		END_ALC = sbrk(PAGE_SIZE);
		BREAK_PTR=sbrk(0);
		if (END_ALC == (void *)-1) _exit(0);//couldn't sbrk
	}
	if (BREAK_PTR == (void *)-1) _exit(0);//couldn't sbrk
	//set arena list pointer and beginning of unallocated break area
	arena_list = START_ALC;
	END_ALC = START_ALC + num_bytes_for_arena_list;

	//initialize mutex for the ptrs to the prgm break area
	if (pthread_mutex_init(&break_region_lock, NULL)) 
		_exit(0);
	if (pthread_mutex_init(&print_lock, NULL)) 
		_exit(0);

	int i;
	for (i=0; i<n_arenas; i+=1) {
		arena_list[i].num_malloc_reqs_0=0;
		arena_list[i].num_malloc_reqs_64=0;
		arena_list[i].num_malloc_reqs_128=0;
		arena_list[i].num_malloc_reqs_512=0;
		arena_list[i].num_malloc_reqs_big=0;

		arena_list[i].num_free_reqs_64=0;
		arena_list[i].num_free_reqs_128=0;
		arena_list[i].num_free_reqs_512=0;
		arena_list[i].num_free_reqs_big=0;

		if (pthread_mutex_init(&(arena_list[i].data_lock), NULL))
			_exit(0);

		LIST_INIT(&(arena_list[i].free_bin_64));
		LIST_INIT(&(arena_list[i].free_bin_128));
		LIST_INIT(&(arena_list[i].free_bin_512));
		LIST_INIT(&(arena_list[i].alloc_bin));			
	}		

	//register fork handlers
	if (pthread_atfork(lock_all,unlock_all,unlock_all)) _exit(0);
}

void lock_all(void){
	int i;
	for (i=0; i<n_arenas; i+=1)
		pthread_mutex_lock(&(arena_list[i].data_lock));
	pthread_mutex_lock(&break_region_lock);
	pthread_mutex_lock(&print_lock);

	// snprintf(printBuff,1024,
	// 		"Thread: %lu\tProcess: %lu\tAquired all locks\n"
	// 		,(unsigned long) pthread_self(),(unsigned long) getpid());
	// write(1,printBuff,strlen(printBuff)+1);
}

void unlock_all(void){
	// snprintf(printBuff,1024,
	// 		"Thread: %lu\tProcess: %lu\tReleasing all locks\n"
	// 		,(unsigned long) pthread_self(),(unsigned long) getpid());
	// write(1,printBuff,strlen(printBuff)+1);

	pthread_mutex_unlock(&print_lock);
	pthread_mutex_unlock(&break_region_lock);
	int i;
	for (i=0; i<n_arenas; i+=1)
		pthread_mutex_unlock(&(arena_list[i].data_lock));
}