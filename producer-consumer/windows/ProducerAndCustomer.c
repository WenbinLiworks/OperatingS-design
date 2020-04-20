// 名称：ProducerAndCustomer.c
// 描述：生产者消费者问题

 
#include "ProducerAndCustomer.h"
 
int main(int argc, char * argv[])
{
	int i, j, k;
	int nextIndex = 1;	//下一个要执行的进程序号 
	int curProNum = mainNum;
	char lt;
	SYSTEMTIME time;
	
	//printf("缓冲区大小为:%d.\n",buffer_len);
	//printf("%d个生产者，分别写入%d次.\n",Need_Producer, Works_Producer);
	//printf("%d个消费者，分别读取%d次.\n",Need_Customer, Works_Customer);
	
	//如果有参数，就作为子进程ID
	if (argc > 1)
	{
		sscanf(argv[1], "%d", &curProNum);
	}
	
	//对于主进程
	if (curProNum == mainNum)
	{
		printf("主进程开始运行！\n");
		
		//创建共享内存 
		handleFileMapping = MakeSharedFile();
		//映射视图
		HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
		if (hFileMapping == NULL)
		{
			printf("打开文件映射失败:%d\n",GetLastError());
			return;
		}
		LPVOID pFile = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pFile == NULL)
		{
			printf("文件映射视图失败:%d\n",GetLastError());
			return;
		}
		else
		{
			struct shareMemory *sham = (struct shareMemory*)(pFile);
			sham->data.head = 0;
			sham->data.tail = 0;
			sham->semEmpty = CreateSemaphore(NULL, buffer_len, buffer_len, "SEM_EMPTY");
			sham->semFull = CreateSemaphore(NULL, 0, buffer_len, "SEM_FULL");
			//取消文件映射
			UnmapViewOfFile(pFile);
			pFile = NULL;
		}
		CloseHandle(hFileMapping);
		
		//创建5个子进程
		while (nextIndex <= 5)
		{
			StartClone(nextIndex++);
		}
		
		//等待子进程运行结束
		for (k=1; k<6; k++)
		{
			WaitForSingleObject(subProHandleArray[k], INFINITE);
			CloseHandle(subProHandleArray[k]);
		}
		
		//输出结束信息
		printf("主进程运行结束！\n");
	}
	
	//2个生产者进程
	else if (curProNum >= Producer_Num_from && curProNum <= Producer_Num_to)
	{
		//映射视图
		HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
		if (hFileMapping == NULL)
		
		{
			printf("打开文件映射失败:%d\n",GetLastError());
			return;
		}
		LPVOID pFile = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pFile == NULL)
		{
			printf("文件映射视图失败:%d\n",GetLastError());
			return;
		}
		else
		{
			struct shareMemory *sham = (struct shareMemory*)(pFile);
			sham->semEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"SEM_EMPTY");
			sham->semFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"SEM_FULL");
			
			for (i=0; i<Works_Producer; i++)
			{
				WaitForSingleObject(sham->semEmpty, INFINITE);
				Sleep(get_random());
				sham->data.str[sham->data.tail] = lt = get_letter();
				sham->data.tail = (sham->data.tail + 1) % buffer_len;
				sham->data.is_empty = 0;
 
				GetSystemTime(&time);
				printf("%04d:%02d:%02d-%02d:%02d:%02d\t",time.wYear,time.wMonth,time.wDay,time.wHour+8,time.wMinute,time.wSecond);
				
				j = (sham->data.tail-1 >= sham->data.head) ? (sham->data.tail - 1) : (sham->data.tail -1 + buffer_len);
				
				for (j; !(sham->data.is_empty)&&(j>=sham->data.head); j--)
				{
					printf("%c", sham->data.str[j % buffer_len]);
				}
				printf("\t 生产者%d进程 写入 '%c'.\n",curProNum-mainNum, lt);
				ReleaseSemaphore(sham->semFull, 1, NULL);
			}
			UnmapViewOfFile(pFile);
			pFile = NULL;
		}
		CloseHandle(hFileMapping);
	}
	
	//3个消费者进程
	else if (curProNum >= Customer_Num_from && curProNum <= Customer_Num_to)
	{
		HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
		if (hFileMapping == NULL)
		{
			printf("打开文件映射失败:%d\n",GetLastError());
			return;
		}
		LPVOID pFile = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pFile == NULL)
		{
			printf("文件映射视图失败:%d\n",GetLastError());
			return;
		}
		else
		{
			struct shareMemory *sham = (struct shareMemory*)(pFile);
			sham->semEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"SEM_EMPTY");
			sham->semFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"SEM_FULL");
			
			for (i=0; i<Works_Customer; i++)
			{
				WaitForSingleObject(sham->semFull, INFINITE);
				Sleep(get_random());
				lt = sham->data.str[sham->data.head];
				sham->data.head = (sham->data.head + 1) % buffer_len;
				sham->data.is_empty = (sham->data.head == sham->data.tail);
				GetSystemTime(&time);
				printf("%04d:%02d:%02d-%02d:%02d:%02d\t",time.wYear,time.wMonth,time.wDay,time.wHour+8,time.wMinute,time.wSecond);
				
				j = (sham->data.tail-1 >= sham->data.head) ? (sham->data.tail - 1) : (sham->data.tail -1 + buffer_len);
				
				for (j; !(sham->data.is_empty)&&(j>=sham->data.head); j--)
				{
					printf("%c", sham->data.str[j % buffer_len]);
				}
				printf("\t 消费者%d进程 读取 '%c'. \n",curProNum-Producer_Num_to, lt);
				ReleaseSemaphore(sham->semEmpty, 1, NULL);
			}
			UnmapViewOfFile(pFile);
			pFile = NULL;
		}
		CloseHandle(hFileMapping);
	}
	
	//关闭主进程的句柄
	CloseHandle(handleFileMapping);
	handleFileMapping = INVALID_HANDLE_VALUE;
	return 0;
}
