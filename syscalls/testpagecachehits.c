#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define SYS_pagecache_counter 336

int main(){
	char hits[8];
	char miss[8];

	int numhits, nummiss;


	long ret = syscall(SYS_pagecache_counter, hits, miss);

	printf("syscall returned: %ld\n", ret);

	numhits = strtol(hits, (char**)NULL, 10);
	nummiss = strtol(miss, (char**)NULL, 10);

	printf("numhits: %d. nummiss: %d\n", numhits, nummiss);

	return 0;
}
