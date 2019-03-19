#include "free.h"

void free(void *ptr) {

	struct mem_seg *seg;
	struct mem_seg_list *list;
	pthread_mutex_t *dl;

	if (ptr == NULL || my_arena == NULL) return; 	//if null return

	seg = ptr-MEM_SEG_OFFSET; //access the mem_seg header

	// pthread_mutex_lock(&print_lock);
	// snprintf(printBuff,1024,
	// 		"free| %p my_arena_n: %lu\t seg->arena: %lu\t seg->len: %lu \n"
	// 		, ptr, (unsigned long)my_arena_n, (unsigned long)seg->arena, (unsigned long) seg->len );
	// write(1,printBuff,strlen(printBuff)+1);
	// pthread_mutex_unlock(&print_lock);


	if (seg->len == 64) 
		list=(struct mem_seg_list *)&(arena_list[seg->arena].free_bin_64);
	else if (seg->len == 128) 
		list=(struct mem_seg_list *)&(arena_list[seg->arena].free_bin_128);
	else 
		list=(struct mem_seg_list *)&(arena_list[seg->arena].free_bin_512);
	
	//figure out which lock you need becuase you cant access seg->arena after unmap
	dl=&(arena_list[seg->arena].data_lock);
	pthread_mutex_lock(dl);
	//increase counter to avoid race conditon
	if (seg->len == 64) arena_list[seg->arena].num_free_reqs_64+=1;
	else if (seg->len == 128) arena_list[seg->arena].num_free_reqs_128+=1;
	else if (seg->len == 512) arena_list[seg->arena].num_free_reqs_512+=1;
	else arena_list[seg->arena].num_free_reqs_big+=1;

	LIST_REMOVE(seg,con); //delete memseg from alloc bin
	
	if (seg->len > 512) { //if it was a >512B use munmap
		munmap(seg->start,seg->len + MEM_SEG_OFFSET);
	} else {//append to correct free bin
		//make sure you insert the mem_seg from the beginning of the memory region
		//not the copy rewritten infront of an aligned memory ptr
		LIST_INSERT_HEAD(list,(struct mem_seg *)seg->start,con); 
	}
	
	pthread_mutex_unlock(dl);
}