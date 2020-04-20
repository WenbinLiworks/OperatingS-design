/*题目要求：设计并实现Unix的“time”命令。“mytime”命令通过命令行参数接受要运行的程序，
			创建一个独立的进程来运行该程序，并记录程序运行的时间。
在Windows下实现:
	使用CreateProcess()来创建进程
	使用WaitForSingleObject()在“mytime”命令和新创建的进程之间同步
	调用GetSystemTime()来获取时间
*/

 
#include <windows.h>
#include <stdio.h>
#include <iostream>
using namespace std;
 
int main(int argc, char **argv)
{
	int year, month, day, hour, minutes, seconds, milliseconds;
	SYSTEMTIME time_start, time_end;
	STARTUPINFO si;	//进程启动相关信息的结构体
	memset(&si,0,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);	//应用程序必须将cb初始化为sizeof(STARTUPINFO)
	si.dwFlags = STARTF_USESHOWWINDOW;	//窗口标志
	si.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION pi;	//必备参数设置结束
 
	if (!CreateProcess
	(NULL,	//不在此指定可执行文件的文件名
		argv[1],	//命令行参数
		NULL,	//默认进程安全性
		NULL,	//默认线程安全性
		FALSE,	//当前进程内的句柄不可以被子进程继承
		CREATE_NEW_CONSOLE,	//为新进程创建一个新的控制台窗口
		NULL,	//使用本进程的环境变量
		NULL,	//使用本进程的驱动器和目录
		&si,	//父进程传给子进程的一些信息
		&pi	//保存新进程信息的结构
		)) 
	{
		cout <<"Create Fail!"<< endl;
		exit(1);
	}
	else
	{
		GetSystemTime(&time_start);
 
		printf("Begin Time：%d:%d:%d-%d:%d:%d:%d\n",time_start.wYear,time_start.wMonth,time_start.wDay,time_start.wHour,time_start.wMinute,time_start.wSecond,time_start.wMilliseconds);
		cout <<"Create Success!"<< endl;
	}
	//使用等待函数来等待所创建进程的死亡
	WaitForSingleObject(pi.hProcess, INFINITE);
	
	GetSystemTime(&time_end);
	printf("End Time: %d:%d:%d-%d:%d:%d:%d",time_start.wYear,time_start.wMonth,time_start.wDay,time_end.wHour,time_end.wMinute,time_end.wSecond,time_end.wMilliseconds);
	
	milliseconds = time_end.wMilliseconds - time_start.wMilliseconds;
	seconds = time_end.wSecond - time_start.wSecond;
	minutes = time_end.wMinute - time_start.wMinute;
	hour = time_end.wHour - time_start.wHour;
	day = time_end.wDay - time_start.wDay;
	month = time_end.wMonth - time_start.wMonth;
	year = time_end.wYear - time_start.wYear;
	if (milliseconds < 0)
	{
		seconds--;
		milliseconds += 1000;
	}
	if (seconds < 0)
	{
		minutes--;
		seconds += 60;
	}
	if (minutes < 0)
	{
		hour--;
		minutes += 60;
	}
	if (hour < 0)
	{
		day--;
		hour += 24;
	}
	if (day < 0)
	{
		month--;
		day += 30;
	}
	if (month < 0)
	{
		year--;
		month += 12;
	}
	printf("\nThis program running time is: ");
	if (year > 0)
	{
		printf("%dY:",year);
	}
	if (month > 0)
	{
		printf("%dM:", month);
	}
	if (day > 0)
	{
		printf("%dD:", day);
	}
	if (hour > 0)
	{
		printf("%dH:", hour);
	}
	if (minutes > 0)
	{
		printf("%dm:", minutes);
	}
	if (seconds > 0)
	{
		printf("%ds:", seconds);
	}
	if (milliseconds > 0)
	{
		printf("%dms", milliseconds);
	}
	printf("\n");
	return 0;
}
