/**
 *			Main process for project
 *
 *	This main process creates and initializes all 
 *  the semaphores and shared_memory. Then main-process
 *	creates 5 processes and waits. 
 *	After all processes finish, main-process clean sems 
 *  and shms.
 *
 *	Author : Zhou YC
 *	Date   : 2018-03-19
 */

#include "pc_header.h"

HANDLE lpHandle[PRODUCER_NUM + CONSUMER_NUM];  
HANDLE shm_mapping;

// We need to prohibit sub-process to print, therefore define mutex here once.
// when createProcess(): first P(sem_mutex), then V(sem_mutex) after creating all processes.
// by then the process (which occupies sem_mutex) can print.
HANDLE sem_mutex;

void initialize()
{
	srand(time(NULL));
	
	// init 3 mutex
	sem_mutex = CreateSemaphore(NULL, 1, 1,MUTEX_KEY);  
    CreateSemaphore(NULL, FIELD_SIZE, FIELD_SIZE, EMPTY_KEY);  
    CreateSemaphore(NULL, 0, FIELD_SIZE, FULL_KEY);  
    
    // init shm
	shm_mapping = CreateFileMapping(  
        NULL,  
        NULL,  
        PAGE_READWRITE,  
        0,  
        sizeof(Field),  
        SHM_KEY
		); 
    if (shm_mapping != INVALID_HANDLE_VALUE)  
    {  
        Field * field = (Field *)MapViewOfFile(  
            shm_mapping,  
            FILE_MAP_ALL_ACCESS,  
            0,  
            0,  
            0);  
        if (field == NULL) {
        	printf("initialize shm failed.\n");
        	exit(0) ;
		}
		field->cnt = 0;
		memset(field->value, '_', FIELD_SIZE);
		field->value[FIELD_SIZE] = 0;
		for (int i = 0; i < FIELD_SIZE; i++) field->available[i] = 0;
		
		UnmapViewOfFile(field);
    }  
}

void createProcess()
{
	int cnt_producer = 0, cnt_consumer = 0;
	int pc_flag = 0;
	
	WaitForSingleObject(sem_mutex, INFINITE);
	
	for (int i = 0; i < PRODUCER_NUM + CONSUMER_NUM; i++) {
		
		PROCESS_INFORMATION pi;  
		STARTUPINFO si;
		memset(&si, 0, sizeof(si));
		
		if (cnt_producer == PRODUCER_NUM) pc_flag = 1;		
		else if (cnt_consumer == CONSUMER_NUM) pc_flag = 0;
		else pc_flag = rand() % 2;

		if (pc_flag == 0) ++cnt_producer;
		else ++cnt_consumer;
		
		char str_filename[30], str_cmd[30];
		
		if (pc_flag == 0) {
			// create producer_main.exe;
			sprintf(str_filename, "producer_main.exe");
			sprintf(str_cmd, "producer_main.exe %d", cnt_producer);
		}
		else {
			// create consumer_main.exe process.
			sprintf(str_filename, "consumer_main.exe");
			sprintf(str_cmd, "consumer_main.exe %d", cnt_consumer);
		}
		CreateProcess(
			str_filename, 
			str_cmd,
			NULL,  
	        NULL,  
	        FALSE,  
	        CREATE_DEFAULT_ERROR_MODE,  
	        NULL,  
	        NULL,  
	        &si, 
	        &pi
		);
		lpHandle[i] = pi.hProcess;
		printf("%s created.\n", str_cmd);
	}
	printf("\n");
	ReleaseSemaphore(sem_mutex, 1, NULL);  
}

int main()
{
	initialize();
	createProcess();
	
	// wait all processes end.
	WaitForMultipleObjects(5,lpHandle,true,INFINITE);  
	
	// clean shm and sem.
	CloseHandle(sem_mutex);
	CloseHandle(shm_mapping);
	printf("\nsems and shm cleaned.\n");
} 
