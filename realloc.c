#include "realloc.h"

void * realloc(void *ptr, size_t size) {
	int oldSize;
	void *retval=NULL;

	if (size >0){
	//if new size > 0 
		//get new memseg with malloc
		retval = malloc(size);

		if (retval != NULL){ //malloc worked
			//if ptr != null
			if (ptr != NULL) {
				//when no alignment, terms in second part sum to 0
				oldSize= /*len_mem - (cur data pos - orig data pos)*/
					((struct mem_seg*)(ptr-MEM_SEG_OFFSET))->len - 
					( ptr /*beginning of data*/  - 
						( /* end of orig mem_seg at beginning of region */
							((struct mem_seg*)(ptr-MEM_SEG_OFFSET))->start + 
							MEM_SEG_OFFSET
						)
					);
				//copy over min(oldSize,size) bytes to new region
				memcpy(retval,ptr,(oldSize > size)?size:oldSize);
			}
		}
	}

	free(ptr); //free old ptr
	return retval;
}