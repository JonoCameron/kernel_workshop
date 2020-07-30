#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>

// set /proc/sys/vm/overcommit_memory to 1 to allow any size allocation
// set /proc/sys/vm/overcommit_memory to 2 to restrict allocations to RAM + SWAP
int main(int argc,char *argv[])
{
	unsigned long siz;
	char    *ptr1;

	printf("usage: jonathan <memsize GB>\n");

	siz = ((long)atol(argv[1])*1024*1024*1024);

	printf("mmaping %ld anonymous bytes\n", siz);
	ptr1 = (char *)mmap((void
	*)0,siz,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);

	if (ptr1 == (char *)-1) {
		printf("ptr1 = %lx\n", ptr1);
		printf("%d\n", errno);
		perror("");
	}
	return 0;
}


