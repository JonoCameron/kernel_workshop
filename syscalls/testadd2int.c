/*
 * Test the add2int syscall (#335)
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <stdio.h>

#define SYS_add2int 335

int main(int argc, char** argv)
{
	int a, b;

	if (argc <= 2)
	{	
		printf("Must provide 2 integers to pass to the system call.\n");
		return -1;
	}
	a = atoi(argv[1]);
	b = atoi(argv[2]);

	printf("Making system call with a=%d, b=%d.\n", a, b);
	long res = syscall(SYS_add2int, a, b);
	printf("System call returned %ld.\n", res);

	return res;
}
