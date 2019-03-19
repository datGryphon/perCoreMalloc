#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char buf[1024];

int main(int argc, char **argv)
{
  size_t size = 50;
  void *mem;
  // posix_memalign(&mem,32,size);
  // mem = memalign(1024,size);
  int i;
  for (i=0; i<10; i+=1){
    mem = malloc(size);
    snprintf(buf,1024,"Successfully malloc'd %zu bytes at addr %p\n", size, mem);
    write(1,buf,strlen(buf)+1);
    assert(mem != NULL);
    free(mem);
    snprintf(buf,1024,"Successfully free'd %zu bytes from addr %p\n", size, mem);
    write(1,buf,strlen(buf)+1);  
  } 
  malloc_stats();
  return 0;
}