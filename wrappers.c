#include "wrappers.h"

int posix_memalign(void **memptr, size_t alignment, size_t size) {
	void *ptr;
	size_t pos;
	struct mem_seg *seg;
	
	*memptr=NULL; //set to null in case of early return

	if (!size) return 0; //nothing to malloc
	//check if pow of 2
	if (!alignment || (alignment & !(alignment-1))!=0) return EINVAL;

	//get enough mem that there will be an address we can use
	*memptr = malloc(size + alignment + MEM_SEG_OFFSET);
	if (*memptr == NULL) return ENOMEM;

	//save the start of the memory allocation area
	ptr=*memptr; 
	seg=ptr - MEM_SEG_OFFSET; //access header from malloc

	pos = (size_t)*memptr;
	pos+= alignment;
	pos+= MEM_SEG_OFFSET; //start far enough into mem region
	*memptr=(void*)(pos-(pos%alignment));	//go back to correct pos

	//need to relocate the mem_seg struct so the call to free can find it
	//setup the mem_seg struct in the area immediately before the ptr returned
	((struct mem_seg*)(*memptr-MEM_SEG_OFFSET))->start=seg->start;
	((struct mem_seg*)(*memptr-MEM_SEG_OFFSET))->len=seg->len;
	((struct mem_seg*)(*memptr-MEM_SEG_OFFSET))->arena=seg->arena;


	pthread_mutex_lock(&(my_arena->data_lock)); //take over lock for some hacking
	LIST_REMOVE(seg,con); //take that entry out of the alloc bin
	//put relocated mem_seg back on alloc list
	LIST_INSERT_HEAD(&(my_arena->alloc_bin),(struct mem_seg *)(*memptr-MEM_SEG_OFFSET),con);
	//note: this function does not overwrite the actual struct still contained 
		//at the begining of the mem region allocated by the malloc call
		//so in free you can still insert *that* memseg at the beginning on the free list
		//because in free it calls insert_head(bin,seg->start,con)
	pthread_mutex_unlock(&(my_arena->data_lock));

	return 0;
}

void * aligned_alloc(size_t alignment, size_t size) {
	void *ptr;
	errno=posix_memalign(&ptr,alignment,size);
	return ptr;
}

void * valloc(size_t size) {
	void *ptr;
	errno=posix_memalign(&ptr,PAGE_SIZE,size);
	return ptr;
}

void * reallocarray(void *ptr, size_t nmemb, size_t size) {
	return realloc(ptr,nmemb*size);
}

void * memalign(size_t alignment, size_t size) {
	void *ptr;
	errno=posix_memalign(&ptr,alignment,size);
	return ptr;
}

void * pvalloc(size_t size) {
	if (size%PAGE_SIZE) size= (size/PAGE_SIZE) + 1;
	else size = size/PAGE_SIZE;
	return valloc(size*PAGE_SIZE);
}