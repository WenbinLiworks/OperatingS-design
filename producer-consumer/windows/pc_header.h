// 名称：ProducerAndCustomer.h
// 描述：生产者消费者问题
// 作者：野狼
// 日期：2017.3.27
 
#ifndef __PRODUCERANDCUSTOMER_H
#define __PRODUCERANDCUSTOMER_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <unistd.h>
 
//1个主进程序号记为0
#define mainNum 0
//2个生产者序号(1~2)
#define Producer_Num_from 1
#define Producer_Num_to 2
//3个消费者序号(3~5)
#define Customer_Num_from 3
#define Customer_Num_to 5
 
//2个生产者，每个生产者工作6次
#define Need_Producer 2
#define Works_Producer 6
 
//3个消费者，每个消费者工作4次
#define Need_Customer 3
#define Works_Customer 4
 
//缓冲区为3
#define buffer_len 3
 
#define SHM_NAME "BUFFER"
 
//文件映射对象句柄
static HANDLE handleFileMapping;
 
//子进程句柄数组
static HANDLE subProHandleArray[5+1];
 
//缓冲区的结构
struct mybuffer
{
	char str[buffer_len];
	int head;
	int tail;
	int is_empty;
};
 
//共享主存区的结构
struct shareMemory
{
	struct mybuffer data;
	HANDLE semEmpty;
	HANDLE semFull;
};
 
//得到1000以内的一个随机数
int get_random()
{
	int digit;
	srand((unsigned)(GetCurrentProcessId() + time(NULL)));
	digit = rand() % 1000;
	return digit;
}
 
//得到A～Z的一个随机字母
char get_letter()
{
	char letter;
	srand((unsigned)(getpid() + time(NULL)));
	letter = (char)((rand() % 26) + 'A');
	return letter;
}
 
//通过参数传递进程的ID创建当前进程的克隆进程
void StartClone(int subProID)
{
	char szFilename[MAX_PATH];
	char szCmdLine[MAX_PATH];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	//获得当前可执行文件名，hModule为NULL返回当前可执行文件的路径名；存放给定模块的路径和文件名；缓冲区大小
	GetModuleFileName(NULL, szFilename, MAX_PATH);
	sprintf(szCmdLine, "\"%s\" %d",szFilename, subProID);
	
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	
	//创建子进程
	BOOL bCreateOK = CreateProcess(szFilename, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	//将新创建进程的句柄赋值给进程ID为subProID的子进程
	subProHandleArray[subProID] = pi.hProcess;	
	return;
}
 
//创建共享内存
HANDLE MakeSharedFile()
{
	//创建临时的文件映射对象(用INVALID_HANDLE_VALUE代替真正的文件句柄)
	HANDLE handleFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(struct shareMemory), SHM_NAME);
    if (handleFileMapping == NULL || handleFileMapping == INVALID_HANDLE_VALUE)
	{
		printf("创建文件映射失败:%d\n",GetLastError());
		return;
	}
	if (handleFileMapping != INVALID_HANDLE_VALUE)
	{
		//把文件映射对象的一个视图映射到当前进程的地址空间，返回值为文件映射的起始地址
		LPVOID pData = MapViewOfFile(handleFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);//高32位，低32位，整个文件映射
		if (pData == NULL)
		{
			printf("创建文件映射视图失败:%d\n",GetLastError());
			return;
		}
		if (pData != NULL)
		{
			//将指定的存储空间清0
			ZeroMemory(pData, sizeof(struct shareMemory));
		}
		//在当前进程的地址空间中解除对一个文件映射对象的映射
		UnmapViewOfFile(pData);
	}
	return handleFileMapping;
}
 
#endif

