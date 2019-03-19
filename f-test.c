#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>

void * other_thread(void *ptr){
	void *temp;

	int i;
	for (i=0; i<10000; i+=1){
		printf(".");
		temp=malloc( rand()%2024 + 24 );
		printf("_");
		free(temp);
		printf("-");
	}

	printf("D");

	return NULL;
}

int main(int argc, char ** argv) {

	srand(666666);	//tested: 10101, 12345

	int retval;
	int i;
	char *str = malloc(1000);
	pthread_t other_threads[10];
	pid_t myPID;

	snprintf(str,1000,"This is a test of libmalloc.so fork/thread support\n");
	puts(str);

	printf("TID: %lu\tPID: %lu\n",(unsigned long) pthread_self(),(unsigned long) getpid());

	printf("Starting background threads\n");
	for (i=0;i<10;i+=1)
		if (pthread_create(&(other_threads[i]),NULL,other_thread,NULL)) _exit(0);
		else printf("Created new thread %lu\n", (unsigned long) other_threads[i]);

	myPID=getpid();

	printf("forking...\n");
	myPID = fork();

	if (myPID == -1) _exit(0);
	if (myPID) {
		printf("I am the parent process\n");
		free(str); 

		for (i=0; i<10; i+=1) {
			retval = pthread_join(other_threads[i],NULL);
			printf(
				(!retval)?
				"Parent tried to rejoin thread %d::%d--succeeded %d\n":
				"Parent tried to rejoin thread %d::%d--failed %d\n"
				,i,errno, retval);
				// _exit(0);
		}

		printf("Parent process joined with background threads\n"); 

		wait(myPID);

		printf("Child process terminated\n");

	} else {
		printf("I am the child process, lets start another set of threads\n");

		printf("Starting background threads\n");
		for (i=0;i<10;i+=1)
			if (pthread_create(&(other_threads[i]),NULL,other_thread,NULL)) _exit(0);
			else printf("Created new thread %lu\n", (unsigned long) other_threads[i]);

		for (i=0; i<10; i+=1) {
			retval = pthread_join(other_threads[i],NULL);
			printf(
				(!retval)?
				"Child tried to rejoin thread %d::%d--succeeded %d\n":
				"Child tried to rejoin thread %d::%d--failed %d\n"
				,i,errno, retval);
				// _exit(0);
		}

		free(str);

		char *cmd[2] = {"./t-test",NULL};
		execvp(cmd[0], cmd);

	}

	malloc_stats();
	return 0;
}