/**
 *		Consumer process.
 *
 *  A consumer consumes object when there is available object
 *  in Field, or wait when there is not.	-- P(full)
 *
 *  When a consumer consumes one object, it takes one full_mutex
 *  and leave a empty_mutex for producer.	-- V(empty)
 *
 *  Author : Zhou YC
 *  Date   : 2018-03-18
 */

#include "pc_header.h"

/**
 *	P(full) -> P(mutex) -> consume -> V(empty) -> V(mutex)
 */

int sem_id_mutex, 
    sem_id_empty,
    sem_id_full;

Field * field;

void p(int sem_id, int val)
{
	struct sembuf buf;
	buf.sem_num = 0; buf.sem_op = -val; buf.sem_flg = 0;
	
	semop(sem_id, &buf, 1);
}

void v(int sem_id, int val)
{
	struct sembuf buf;
	buf.sem_num = 0; buf.sem_op = val; buf.sem_flg = 0;
	
	semop(sem_id, &buf, 1);
}

void getNowTime()
{
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time); 
	tm nowTime;
	localtime_r(&time.tv_sec, &nowTime);
	char current[20];
	sprintf(current, "%02d:%02d:%02d", nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
	printf("(%s)  ", current);
}
int initialize()
{
	sem_id_mutex = semget(MUTEX_KEY, 1, IPC_CREAT|0666);
	sem_id_empty = semget(EMPTY_KEY, 1, IPC_CREAT|0666);
	sem_id_full  = semget(FULL_KEY, 1, IPC_CREAT|0666);
	
	struct timeval time_now;
	gettimeofday(&time_now, NULL);
	srand(time_now.tv_usec);

	int shm_id = shmget(SHM_KEY, sizeof(Field), IPC_CREAT|0666);	
	field = (Field *)shmat(shm_id, NULL, 0);
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
	//int cnt_available = 0;
	//for (int i = 0; i < FIELD_SIZE; i++) cnt_available += objs[i].available;
	printf("Consumer %s << %c (%d left) %s\n", id, val, field->cnt, field->value);
}
int main(int argc, char *argv[])
{	
	printf("Consumer %s start.\n", argv[1]);
	initialize();

	for (int i = 0; i < CONSUMER_LOOP; i++) {
		int sleep_usec = rand() % 800000;
		usleep(sleep_usec);

		p(sem_id_full, 1);
		//printf("consumer %s use 1 full, mutex=%d\n", argv[1], semctl(sem_id_mutex, 0, GETVAL));
		p(sem_id_mutex, 1);
		//printf("consumer %s use 1 mutex\n", argv[1]);
		v(sem_id_empty, 1);
		getNowTime();
		consume(argv[1]);
		v(sem_id_mutex, 1);
	}
	//printf("consumer %s done, mutex=%d\n", argv[1], semctl(sem_id_mutex, 0, GETVAL));
	return 0;
}
