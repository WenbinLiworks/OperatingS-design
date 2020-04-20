// 内存监视.cpp : 定义控制台应用程序的入口点。
/* TITLE：设计一个内存监视器，能实时地显示当前系统中内存的使用情况，包括系统地址空间
         的布局，物理内存的使用情况；能实时显示某个进程的虚拟地址空间布局和工作集信息等。
*/
#include "stdafx.h"
#include <cstdio>
#include <cstdlib>
#include <iostream> 
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <iomanip>
#pragma comment(lib,"Shlwapi.lib")
using namespace std;
 
//显示保护标记，该标记表示允许应用程序对内存进行访问的类型
inline bool TestSet(DWORD dwTarget, DWORD dwMask)
{
	return ((dwTarget &dwMask) == dwMask);
}
 
#define SHOWMASK(dwTarget,type) if(TestSet(dwTarget,PAGE_##type)){cout << "," << #type;}
 
void ShowProtection(DWORD dwTarget)
{//定义的页面保护类型
	SHOWMASK(dwTarget, READONLY);
	SHOWMASK(dwTarget, GUARD);
	SHOWMASK(dwTarget, NOCACHE);
	SHOWMASK(dwTarget, READWRITE);
	SHOWMASK(dwTarget, WRITECOPY);
	SHOWMASK(dwTarget, EXECUTE);
	SHOWMASK(dwTarget, EXECUTE_READ);
	SHOWMASK(dwTarget, EXECUTE_READWRITE);
	SHOWMASK(dwTarget, EXECUTE_WRITECOPY);
	SHOWMASK(dwTarget, NOACCESS);
}
 
//遍历整个虚拟内存，并显示各内存区属性的工作程序的方法
void WalkVM(HANDLE hProcess)
{
	SYSTEM_INFO si;	//系统信息结构
	ZeroMemory(&si, sizeof(si));	//初始化
	GetSystemInfo(&si);	//获得系统信息
 
	MEMORY_BASIC_INFORMATION mbi;	//进程虚拟内存空间的基本信息结构
	ZeroMemory(&mbi, sizeof(mbi));	//分配缓冲区，用于保存信息
 
	//循环整个应用程序地址空间
	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress;
	while (pBlock < si.lpMaximumApplicationAddress)
	{
		//获得下一个虚拟内存块的信息
		if (VirtualQueryEx(
			hProcess,	//相关的进程
			pBlock,		//开始位置
			&mbi,		//缓冲区
			sizeof(mbi)) == sizeof(mbi))	//长度的确认，如果失败返回0
		{
			//计算块的结尾及其长度
			LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
			TCHAR szSize[MAX_PATH];
			//将数字转换成字符串
			StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH);
 
			//显示块地址和长度
			cout.fill('0');
			cout << hex << setw(8) << (DWORD)pBlock << "-" << hex << setw(8) << (DWORD)pEnd << (strlen(szSize) == 7 ? "(" : "(") << szSize << ")";
 
			//显示块的状态
			switch (mbi.State)
			{
			case MEM_COMMIT:
				printf("已提交");
				break;
			case MEM_FREE:
				printf("空闲");
				break;
			case MEM_RESERVE:
				printf("已预留");
				break;
			}
 
			//显示保护
			if (mbi.Protect == 0 && mbi.State != MEM_FREE)
			{
				mbi.Protect = PAGE_READONLY;
			}
			ShowProtection(mbi.Protect);
 
			//显示类型
			switch (mbi.Type)
			{
			case MEM_IMAGE:
				printf(", Image");
				break;
			case MEM_MAPPED:
				printf(", Mapped");
				break;
			case MEM_PRIVATE:
				printf(", Private");
				break;
			}
 
			//检验可执行的映像
			TCHAR szFilename[MAX_PATH];
			if (GetModuleFileName(
				(HMODULE)pBlock,			//实际虚拟内存的模块句柄
				szFilename,					//完全指定的文件名称
				MAX_PATH) > 0)				//实际使用的缓冲区长度
			{
				//除去路径并显示
				PathStripPath(szFilename);
				printf(", Module:%s", szFilename);
			}
 
			printf("\n");
			//移动块指针以获得下一个块
			pBlock = pEnd;
		}
	}
}
 
 
int main(int argc, char* argv[])
{
	MEMORYSTATUSEX statex;	//
	statex.dwLength = sizeof(statex);
	//获取系统内存信息
	GlobalMemoryStatusEx(&statex);
	printf("-----------------------内存信息-----------------------\n");
	//内存使用率
	printf("物理内存的使用率为:%ld%%\n", statex.dwMemoryLoad);
	//物理内存
	printf("物理内存的总容量为: %.2fGB.\n", (float)statex.ullTotalPhys / 1024 / 1024 / 1024);
	//可用物理内存
	printf("可用的物理内存为: %.2fGB.\n", (float)statex.ullAvailPhys / 1024 / 1024 / 1024);
	//提交的内存限制
	printf("总的交换文件为:%.2fGB.\n", (float)statex.ullTotalPageFile / 1024 / 1024 / 1024);
	//当前进程可以提交的最大内存量
	printf("可用的交换文件为：%.2fGB.\n", (float)statex.ullAvailPageFile / 1024 / 1024 / 1024);
	//虚拟内存
	printf("虚拟内存的总容量为：%.2fGB.\n", (float)statex.ullTotalVirtual/1024 / 1024 / 1024);
	//可用虚拟内存
	printf("可用的虚拟内存为：%.2fGB.\n", (float)statex.ullAvailVirtual/1024 / 1024 / 1024);
	//保留字段
	printf("保留字段的容量为：%.2fByte.\n",statex.ullAvailExtendedVirtual);
	printf("------------------------------------------------------\n");
 
	SYSTEM_INFO si;	//系统信息结构
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);	//获得系统信息
	printf("---------------------系统信息-------------------------\n");
	printf("内存页的大小为：%dKB.\n", (int)si.dwPageSize/1024);
	cout << "每个进程可用地址空间的最小内存地址为: 0x" << si.lpMinimumApplicationAddress << endl;
	cout << "每个进程可用的私有地址空间的最大内存地址为: 0x" << si.lpMaximumApplicationAddress << endl;
	cout << "能够保留地址空间区域的最小单位为: " << si.dwAllocationGranularity/1024 << "KB" << endl;
	printf("------------------------------------------------------\n");
 
	//获取系统的存储器使用情况
	PERFORMANCE_INFORMATION pi;
	pi.cb = sizeof(pi);
	GetPerformanceInfo(&pi, sizeof(pi));
	printf("----------------系统的存储器使用情况------------------\n");
	cout << "结构体的大小为: " << pi.cb << "B" << endl;
	cout << "系统当前提交的页面总数: " << pi.CommitTotal << endl;
	cout << "系统当前可提交的最大页面总数: " << pi.CommitLimit << endl;
	cout << "系统历史提交页面峰值: " << pi.CommitPeak << endl;
	cout << "按页分配的总物理内存: " << pi.PhysicalTotal << endl;
	cout << "当前可用的物理内存为: " << pi.PhysicalAvailable << endl;
	cout << "系统Cache的容量为: " << pi.SystemCache << endl;
	cout << "内存总量(按页)为: " << pi.KernelTotal << endl;
	cout << "分页池的大小为: " << pi.KernelPaged << endl;
	cout << "非分页池的大小为: " << pi.KernelNonpaged << endl;
	cout << "页的大小为: " << pi.PageSize << endl;
	cout << "打开的句柄个数为: " << pi.HandleCount << endl;
	cout << "进程个数为: " << pi.ProcessCount << endl;
	cout << "线程个数为: " << pi.ThreadCount << endl;
	printf("------------------------------------------------------\n");
 
	//获得每个进程的信息
	printf("------------------各个进程的信息----------------------\n");
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BOOL bMore = ::Process32First(hProcessSnap, &pe);
	while (bMore)
	{
		HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		PROCESS_MEMORY_COUNTERS pmc;
		ZeroMemory(&pmc, sizeof(pmc));
 
		if (GetProcessMemoryInfo(hP, &pmc, sizeof(pmc)) == TRUE)
		{
			cout << " 进程ID:";
			wcout << pe.th32ProcessID << endl;
			cout << " 进程名称:";
			wcout << pe.szExeFile << endl;
			cout << " 虚拟内存的大小为:" << (float)pmc.WorkingSetSize / 1024 << "KB" << endl;
		}
		bMore = ::Process32Next(hProcessSnap, &pe);
	}
	printf("----------------------------------------------------\n");
 
	printf("-------进程虚拟地址空间布局和工作集信息查询---------\n");
	printf("输入要查询的进程ID:");
	int x;
	cin >> x;
	HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, FALSE, x);
	WalkVM(hP);
	getchar();
	getchar();
    return 0;
}
