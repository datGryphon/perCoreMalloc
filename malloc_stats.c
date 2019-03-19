#include "malloc_stats.h"

void malloc_stats() {

	int i,a;
	struct mem_seg * p_itr;

	lock_all(); //lock all mutexes

	snprintf(printBuff,1024,"-------------------MALLOC_STATS\n");
	write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

	snprintf(printBuff,1024,
		"Size of .data region used for allocation %ldB\n"
		"Program Break at start of run %p\n"
		"End of area used for allocation %p\n"
		"Current Program Break position %p\n",
		END_ALC-START_ALC,START_ALC,END_ALC,BREAK_PTR);
	write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

	for(a=0; a<n_arenas; a+=1){

		snprintf(printBuff,1024,"--------Arena %d\n",a);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"Total number of bins: 4\n");
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"free_bin_64: \n");
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		i=0;
		p_itr = arena_list[a].free_bin_64.lh_first;
		while (p_itr != NULL) {
			p_itr = p_itr->con.le_next;
			i+=1;
		}

		snprintf(printBuff,1024,
			"\t#blocks %d\n",i);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#free reqs  %ld\n",arena_list[a].num_free_reqs_64);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"free_bin_128: \n");
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		i=0;
		p_itr = arena_list[a].free_bin_128.lh_first;
		while (p_itr != NULL) {
			p_itr = p_itr->con.le_next;
			i+=1;
		}

		snprintf(printBuff,1024,
			"\t#blocks %d\n",i);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#free reqs  %ld\n",arena_list[a].num_free_reqs_128);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"free_bin_512: \n");
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		i=0;
		p_itr = arena_list[a].free_bin_512.lh_first;
		while (p_itr != NULL) {
			p_itr = p_itr->con.le_next;
			i+=1;
		}

		snprintf(printBuff,1024,
			"\t#blocks %d\n",i);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#free reqs  %ld\n",arena_list[a].num_free_reqs_512);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"alloc_bin: \n");
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		i=0;
		p_itr = arena_list[a].alloc_bin.lh_first;
		while (p_itr != NULL) {
			p_itr = p_itr->con.le_next;
			i+=1;
		}

		snprintf(printBuff,1024,
			"\t#blocks %d\n",i);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#blocks of 64B %ld\n",
			arena_list[a].num_malloc_reqs_64 - arena_list[a].num_free_reqs_64);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#malloc reqs for x<=64B %ld\n",arena_list[a].num_malloc_reqs_64);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#blocks of 128B %ld\n",
			arena_list[a].num_malloc_reqs_128 - arena_list[a].num_free_reqs_128);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#malloc reqs for 64B<x<=128B %ld\n",arena_list[a].num_malloc_reqs_128);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#blocks of 512B %ld\n",
			arena_list[a].num_malloc_reqs_512 - arena_list[a].num_free_reqs_512);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#malloc reqs for 128B<x<=512B %ld\n",arena_list[a].num_malloc_reqs_512);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#blocks of >512B %ld\n",
			arena_list[a].num_malloc_reqs_big - arena_list[a].num_free_reqs_big);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);

		snprintf(printBuff,1024,
			"\t#malloc reqs for >512B %ld\n",arena_list[a].num_malloc_reqs_big);
		write(STDOUT_FILENO,printBuff,strlen(printBuff)+1);
	}

	unlock_all();//release all locks

}