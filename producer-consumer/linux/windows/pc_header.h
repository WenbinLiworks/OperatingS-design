/**
 *			pc_header.h					*
 *										*
 *  Include all header project needs.	*
 *  Define MARCOS and STRUCTS here.		*
 *										*
 *  Author : ZhouYC						*
 *  Date   : 2018-03-20					*
 *										*/
 
#ifndef _PC_HEADER_H_
#define _PC_HEADER_H_

#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <sys/time.h>

#define PRODUCER_NUM	2
#define CONSUMER_NUM	3

#define MUTEX_KEY	"MUTEX_KEY"
#define EMPTY_KEY	"EMPTY_KEY"
#define FULL_KEY	"FULL_KEY"
#define SHM_KEY		"SHM_KEY"

#define FIELD_SIZE	3

#define PRODUCER_LOOP	6
#define CONSUMER_LOOP	4

// how long Sleep() takes.
#define MAX_SLEEP_MSEC	1000

struct Field {
	int cnt;
	char value[FIELD_SIZE + 1];
	int available[FIELD_SIZE];
};
#endif
