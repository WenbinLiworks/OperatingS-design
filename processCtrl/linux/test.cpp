
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#define TIME_COUNT 4
int main() 
{
	for (int i = 1; i <= TIME_COUNT; i++) {
		usleep(999700);				// unit is us.
		printf("%d\n", i);
	}

//	printf("test's pid: %d\n", getpid());
	return 0;
}
