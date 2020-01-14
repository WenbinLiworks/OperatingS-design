/**
 *	Main Function: mytime
 *	
 *	Parameter: 
 *		1. executable file path.
 *		2. running limit time (measured in second).
 *
 *	Create new process of given program 
 *	(and terminate it if it doesn't terminate automatically in given time). 
 *	
 */

#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <sys/time.h>

#define MAXN 128

using namespace std;

// Calculate running time between t2(start) and t1(end).
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

int main(int argc, char *argv[])
{
	// Exe file path (test.exe as default).
	char exepath[MAXN];
	strcpy(exepath, (argc == 1 ? "C:\\windows\\system32\\notepad.exe" : argv[1]));
	
	// Second arg that limits running time.
	int limitTime = argc < 3 ? INFINITE : atoi(argv[2]) * 1000;
	int flagForced = 0;

	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	
	PROCESS_INFORMATION pi;
	//char lpCommand[52] = ;
	
	if(!CreateProcess(
		exepath, NULL,
		NULL, NULL, FALSE, 0, NULL,	NULL,
		&si, &pi )) {
			
		cout << "Create Fail!"<<endl;
		exit(1);
	} 
	else {
		cout<< "*\tOpen \"" << exepath << "\" successfully!" << "\n" << endl;
		
		struct timeval time_start, time_end;		
	
		// Get start time.
		//GetSystemTime(&time_start);
		gettimeofday(&time_start, NULL);

		// Wait for child program terminate.
		// Not terminate in given limit time. (with 3rd arg)
		if (WaitForSingleObject(pi.hProcess, limitTime)) {
		// Kill child process at given time.
			TerminateProcess(pi.hProcess, 2333);
			flagForced = 1;
		}

		//GetSystemTime(&time_end);
		gettimeofday(&time_end, NULL);
		
		cout << "\n" << "*	\"" << exepath << "\" terminated." << (flagForced ? "(forced)" : "") << endl;
		cout << "*	Running time: "; 
		outputTimeUse(&time_end, &time_start);
	}

	return 0;
} 

