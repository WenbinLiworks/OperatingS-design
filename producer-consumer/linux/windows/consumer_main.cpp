/**
 *		Consumer process for Windows.
 *	Consumer consumes available objects in Field
 *	or wait when there is no available object.
 *
 *	Author : Wenbin Li
 *	Date   : 2019-12-20
 */

#include "pc_header.h"
#include <windows.h>

HANDLE sem_mutex,
	   sem_empty,
	   sem_full;
	   
HANDLE shm_mapping;
struct Field * field;

void initialize()
{
	struct timeval time_now;
	gettimeofday(&time_now, NULL);
	srand(time_now.tv_usec);
	
	// open sems and shm
	sem_mutex = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, MUTEX_KEY);  
	sem_empty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, EMPTY_KEY); 
	sem_full  = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, FULL_KEY);
	shm_mapping = OpenFileMapping( 
		FILE_MAP_ALL_ACCESS,  
        0, 
		SHM_KEY
		); 
	field = (struct Field *)MapViewOfFile(  
            shm_mapping,  
            FILE_MAP_ALL_ACCESS,  
            0,  
            0,  
            0
		); 
}

void getNowTime()
{
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970到现在的秒数
	tm nowTime;
	localtime_r(&time.tv_sec, &nowTime);
	char current[20];
	sprintf(current, "%02d:%02d:%02d", nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
	printf("(%s)  ", current);
}

void consume(char * id)
{
	char val = 'x';
	for (int i = 0; i < FIELD_SIZE; i++) {
		if (field->available[i] != 0) {
			val = field->value[i];
			field->available[i] = 0;
			field->value[i] = '_';
			break;
		}
	}
	field->cnt -= 1;
	printf("Consumer %s << %c (%d left) %s\n", id, val, field->cnt, field->value);
}

void clean()
{
	CloseHandle(sem_mutex);
	CloseHandle(sem_empty);
	CloseHandle(sem_full );
	CloseHandle(shm_mapping);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Insufficient arguments in 'consumer_main'\n");
		return 0;
	}
	
	initialize();

	for (int i = 0; i < CONSUMER_LOOP; i++) {
		int sleep_msec = rand() % MAX_SLEEP_MSEC;
		Sleep(sleep_msec + 100);
	//	printf("consumer %s waits for full\n", argv[1]);
		WaitForSingleObject(sem_full , INFINITE);
	//	printf("consumer %s waits for mutex\n", argv[1]);
		WaitForSingleObject(sem_mutex, INFINITE);
	//	printf("consumer %s consumes...\n", argv[1]);
	
		getNowTime(); 
		consume(argv[1]);
		
		ReleaseSemaphore(sem_empty, 1, NULL);  
        ReleaseSemaphore(sem_mutex, 1, NULL);  
	}
	
	clean();
}
