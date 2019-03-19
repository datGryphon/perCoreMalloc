#include "calloc.h"

void * calloc(size_t nmemb, size_t size) {
	void *ptr = NULL;

	if (nmemb && size){ //if nmemb and size both non-zero
		ptr = malloc(nmemb * size); //call malloc for nmemb*size B
		if (ptr != NULL) bzero(ptr,nmemb*size);
		else errno=ENOMEM;
	} 
	return ptr;
}