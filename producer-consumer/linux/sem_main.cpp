/**
 *	Main process of project
 *	Initialize all semaphores and shared memory.
 *	Create 5 processes and wait for them finish.
 *
 *	Author : Wenbin Li
 *	Date   : 2019-12-30
 */

#include "pc_header.h"

int sem_id_mutex, sem_id_empty, sem_id_full;

void initialize_main()
{
	srand(time(NULL));
	union semun arg;

	// mutex
	sem_id_mutex = semget(MUTEX_KEY, 1, IPC_CREAT|0666);
	arg.val = 1;
	semctl(sem_id_mutex, 0, SETVAL, arg);

	// empty
	sem_id_empty = semget(EMPTY_KEY, 1, IPC_CREAT|0666);
	arg.val = FIELD_SIZE;
	semctl(sem_id_empty, 0, SETVAL, arg);

	// full
	sem_id_full = semget(FULL_KEY, 1, IPC_CREAT|0666);
	arg.val = 0;
	semctl(sem_id_full, 0, SETVAL, arg);

	printf("mutex: %d, empty: %d, fill: %d\n", semctl(sem_id_mutex, 0, GETVAL), semctl(sem_id_empty, 0, GETVAL), semctl(sem_id_full, 0, GETVAL));
	

	int shm_id = shmget(SHM_KEY, sizeof(Field), IPC_CREAT|0666);	
	Field * field = (Field *)shmat(shm_id, NULL, 0);
	field -> cnt = 0;
	for (int i = 0; i < FIELD_SIZE; i++) {
		field->available[i] = 0;
		field->value[i] = '_';
	}
	printf("shmid: %d\n", shm_id);
}

// close all handles of sem and shm.
void clean()
{
	int shm_id = shmget(SHM_KEY, sizeof(Field), IPC_CREAT|0666);
	shmctl(shm_id, IPC_RMID, 0);
	semctl(sem_id_mutex, IPC_RMID, 0);
	semctl(sem_id_empty, IPC_RMID, 0);
	semctl(sem_id_full , IPC_RMID, 0);
	printf("Clean all sem and shm.\n");
}

int main()
{
	int pc_flag = 0;
	int cnt_producer = 0, cnt_consumer = 0;

	initialize_main();
	
	int pid = -1;
	for (int i = 0; i < PRODUCER_NUM + CONSUMER_NUM; i++) {
		if (cnt_producer == PRODUCER_NUM) pc_flag = 1;		
		else if (cnt_consumer == CONSUMER_NUM) pc_flag = 0;
		else pc_flag = rand() % 2;

		if (pc_flag == 0) ++cnt_producer;
		else ++cnt_consumer;
		
		pid = fork();
		if (pid == 0) {
			char strid[3];
			if (pc_flag == 1) {
				sprintf(strid, "%d", cnt_consumer);
				execl("consumer_main", "consumer_main", strid, NULL);
			}
			else {
				sprintf(strid, "%d", cnt_producer);
				execl("producer_main", "producer_main", strid, NULL);
			}
			break;
		}
	}
	

	if (pid != 0) {
		// when all sub-processes finish.
		for (int i = 0; i < PRODUCER_NUM + CONSUMER_NUM; i++) wait(NULL);

		printf("done\n");
		clean();
		exit(0);
	}
	return 0;
}
