/**
 *		pc_header.h		*
 *					*
 *  Include all header project needs.	*
 *  Define MARCOS and STRUCTS here.	*
 *					*
 *  Author : ZhouYC			*
 *  Date   : 2018-03-18			*
 *					*/

#ifndef _PC_HEADER_H_
#define _PC_HEADER_H_

#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/sem.h>
#include <wait.h>
#include <sys/time.h>

#define PRODUCER_NUM	2
#define CONSUMER_NUM	3

#define MUTEX_KEY	1120
#define EMPTY_KEY	151
#define FULL_KEY	868
#define SHM_KEY		29569

#define FIELD_SIZE	3

#define PRODUCER_LOOP	6
#define CONSUMER_LOOP	4

/* Field of limited size(FIELD_SIZE) contains 'products' produced by producers.*/
struct Field {
	int cnt;
	char value[FIELD_SIZE + 1];	/* Each object is represented by a capital letter or '_'*/
	int available[FIELD_SIZE];
};

/* for function semctl() */
union semun {
        int val; 		/* value for SETVAL */
        struct semid_ds *buf; 	/* buffer for IPC_STAT, IPC_SET */
        unsigned short *array;	/* array for GETALL, SETALL */
        struct seminfo *__buf; 	/* buffer for IPC_INFO */
};



#endif
