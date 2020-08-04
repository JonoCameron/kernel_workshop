#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define SYS_pagecache_counter 336

/*
 * Things commented out are left from when the syscall sucked and
 * returned strings that had to be dealt with.
 *
 * Now the variables for pagecache_hits and pagecache_misses are
 * defined like sysctl_error_barf which is a tuneable. These
 * variables are obviously not tuneables but the way they can be
 * read out is a lot less rubbish.
 *
 * Go and have a look at the source code for pagecache_counter()
 * in linux/kernel/sys.c to see what I mean. I left the old rubbish
 * code in as legacy or whatever you call it.
 */

int main(){
	int* hits;
	int* miss;

//	int numhits, nummiss;


	long ret = syscall(SYS_pagecache_counter, &hits, &miss);

	printf("syscall returned: %ld\n", ret);

//	numhits = strtol(hits, (char**)NULL, 10);
//	nummiss = strtol(miss, (char**)NULL, 10);

	printf("hits: %d. miss: %d\n", hits, miss);

	return 0;
}
