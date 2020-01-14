/**
 *	Main Function: mytime
 * 
 *	Parameter: 
 *		1. A executable file path. ('./test' as default)
 *		2. Limit running time. (measured in sec.)
 *
 * 	Create a new process of given program name.
 *	Output its running time.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>

#define MAXN 128

/*
 * parameters: 
 *		char *, a (absolute) path
 * return:
 *		char *, last part of path (exclude '/')
 * e.g:
 *		'/usr/boot/grub' -> 'grub'
 */
char* getLastWord(char *path)
{
	char *p = path + strlen(path) - 1;
	while (p > path && *p != '/' && *p != '\\') p--;
	return (*p == '\\' || *p == '/') ? p+1 : p;
}


/*
 * Trasfer a str into a path
 * 
 * paramaters: 
 *		char *, src path
 *		char *, res path
 * e.g:
 *		'test' -> './test'
 *		'/usr/fred' -> '/usr/fred'
 */
void proceedPath(char *path_temp, char *path)
{
	if (path_temp[0] != '\\' && path_temp[0] != '/' && path_temp[0] != '.') {
		//strcpy(path, "./");
	}
	strcat(path, path_temp);
}

void outputTimeUse(struct timeval * t1, struct timeval * t2)
{
	long long time_use = (t1->tv_sec - t2->tv_sec)*1000000 + (t1->tv_usec - t2->tv_usec);

	printf("* %lld h %lld min %lld sec %lld ms %lld us. \n", 
		(time_use) / 3600000000,
		(time_use % 3600000000) / 60000000, 
		(time_use % 60000000) / 1000000, 
		(time_use % 1000000) / 1000, 
		time_use % 1000
	);
}

/* 
 *	Use multi-thread to check whether child process terminates in the given limit time.
 *	parameters:
 *		1.	void *, info including: int pid, int limit_time
 */
void* waitThreadFunction(void * info)
{
	for (int i = 0; i < ((int *)info)[1]; i++) {
		sleep(1);
	}
	if (kill(((int *)info)[0], SIGKILL) != -1) {
		printf("\n* Process was forced to terminate by watcher thread.\n");
	}
}

int main(int argc, char *argv[])
{
	pid_t pid = vfork();
	if (pid < 0) {
		printf("Create process failed.\n");
		return 0;
	}
	
	char path_temp[MAXN] = "./test";
	char path[MAXN] = "\0";
	if (argc > 1) {	// else: use default "./test"
		strcpy(path_temp, argv[1]);
	}
	proceedPath(path_temp, path);

	struct timeval time_start, time_end;

	// Child process.
	if (pid == 0) {

	// In vfork function, parent and child share these variables.
	// Therefore we can use gettimeofday() here.
		gettimeofday(&time_start, NULL);		

	// Create process of given program.
		execlp(path, getLastWord(path), (char *)0);
	}

	// Parents process.
	else {
		printf("* Process (%d) '%s' started. \n\n", pid, path);

		if (argc > 2) {
	// Use another watcher thread to terminate child process in certain time.
	// Thread will force process to terminate by time. (print: 'forced'.)
	// Else, ignore this thread, watcher will be killed when exit (main return).
			int limit_time = atoi(argv[2]);
			int pinfo[2] = {pid, limit_time};
			pthread_t tid;

			pthread_create(&tid, NULL, waitThreadFunction, pinfo);
		}

	// Wait for child process to terminate.
		wait(0);
		
		gettimeofday(&time_end, NULL);

		printf("\n* Process terminated.\n");
		outputTimeUse(&time_end, &time_start);

	}
	return 0;
}

