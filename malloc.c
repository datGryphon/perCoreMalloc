#include "malloc.h"

void * malloc(size_t size) {
	
	if (arena_list == NULL){
		INIT();
	}

	//if new thread, set arena
	if (my_arena == NULL){
		my_arena_n = sched_getcpu();
		//default to zero if failure
		if (my_arena_n == -1) my_arena_n=0;
		my_arena = &(arena_list[my_arena_n]);

		// 	pthread_mutex_lock(&print_lock);
		// snprintf(printBuff,1024,
		// 	"New Thread initialized to arena %lu\n"
		// 	"my_arena: %p\n"
		// 	, (unsigned long)my_arena_n, my_arena);
		// write(1,printBuff,strlen(printBuff)+1);
		// 	pthread_mutex_unlock(&print_lock);
	}

	void *ptr;
	struct mem_seg_list *list;
	struct mem_seg *seg;
	size_t fit;

	pthread_mutex_lock(&(my_arena->data_lock));

	if (size > 512){
		my_arena->num_malloc_reqs_big+=1;
		//find number of pages to allocate
		fit = (size+MEM_SEG_OFFSET) / PAGE_SIZE;
		if ((size+MEM_SEG_OFFSET) % PAGE_SIZE) fit +=1;

		ptr = mmap(0,fit*PAGE_SIZE, PROT_READ | PROT_WRITE,
					MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

		if (ptr != MAP_FAILED){
			seg=ptr;
			seg->len = fit*PAGE_SIZE - MEM_SEG_OFFSET;
			seg->start=ptr;
			seg->arena=my_arena_n;
			LIST_INSERT_HEAD(&(my_arena->alloc_bin),seg,con);
			//set ptr to beginning of allocated area
			ptr+=MEM_SEG_OFFSET;
		} else {
			ptr = NULL;
			errno = ENOMEM;
		}
	} else if (size > 0) {		
		//find correct fit and increment request counter
		if (size <= 64) {
			fit=64;
			my_arena->num_malloc_reqs_64+=1;
		} else if (size <= 128) {
			fit=128;
			my_arena->num_malloc_reqs_128+=1;
		} else {
			fit=512;
			my_arena->num_malloc_reqs_512+=1;
		}
		//get correct bin
		if (fit == 64) 
			list=(struct mem_seg_list *)&(my_arena->free_bin_64);
		else if (fit == 128) 
			list=(struct mem_seg_list *)&(my_arena->free_bin_128);
		else 
			list=(struct mem_seg_list *)&(my_arena->free_bin_512);
		//check if the free bin is not empty
		if (list->lh_first != NULL) {
			//take free chunk off list
			seg=list->lh_first;
			LIST_REMOVE(seg,con);
			//put on allocated list
			LIST_INSERT_HEAD(&(my_arena->alloc_bin),seg,con);
			ptr=seg;
			ptr +=MEM_SEG_OFFSET; //set ptr to data area
		} else {
			//need to make a new memseg
			//check if there is space left before prgm break
			//else use sbreak

			pthread_mutex_lock(&break_region_lock);

			ptr=0;
			if (END_ALC + fit + MEM_SEG_OFFSET > BREAK_PTR) {
				ptr=sbrk(PAGE_SIZE);
				BREAK_PTR=sbrk(0);
			}

			if (ptr == (void *)-1){
				ptr=NULL;
				errno=ENOMEM;
			} else {
				ptr=END_ALC;	//set ptr to the end of used space
				END_ALC += fit+MEM_SEG_OFFSET; //move the end of alloced space ptr
				//create memseg, append to alloc and set ptr
				seg=ptr;
				seg->len=fit;
				seg->start=seg;
				seg->arena=my_arena_n;
				ptr = END_ALC - fit; //set ptr to data area
				//put on allocated list
				LIST_INSERT_HEAD(&(my_arena->alloc_bin),seg,con);
			}

			pthread_mutex_unlock(&break_region_lock);

		}
	} else {
		ptr = NULL; //size = 0 so return null
		my_arena->num_malloc_reqs_0+=1;
	}

	// if (ptr != NULL){
	// 	pthread_mutex_lock(&print_lock);
	// 	snprintf(printBuff,1024,
	// 		"malloc| %p { s:%p, a:%lu, l:%lu }\n"
	// 		, ptr, seg->start, seg->arena, seg->len);
	// 	write(1,printBuff,strlen(printBuff)+1);
	// 	pthread_mutex_unlock(&print_lock);
	// }
	pthread_mutex_unlock(&(my_arena->data_lock));

	return ptr;
}