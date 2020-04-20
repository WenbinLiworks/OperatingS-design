/********************************************/
/*名称：mytime.c
/*描述：用命令行的形式建立一个新的进程，并保存其运行的时间 

/********************************************/
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
 
int main(int argc, char **argv)
{
	//调用系统时间
	struct timeval time_start;
	struct timeval time_end;
	//用以记录进程运行的时间
	float time_use = 0;
	pid_t pid;
	pid = fork();
	if (pid < 0)	//如果出错
	{
		printf("Create Fail!");
		exit(0);
	}
	else if (pid == 0)	//如果是子进程
	{
		printf("Create Child\n");
		gettimeofday(&time_start,NULL);
		printf("111time_start.tv_sec:%d\n",time_start.tv_sec);
		printf("111time_start.tv_usec:%d\n\n",time_start.tv_usec);
		//在子进程中调用execv函数在命令行中来运行一个程序
		execv(argv[1],&argv[1]);
	}
	else
	{	
		gettimeofday(&time_start,NULL);
		printf("time_start.tv_sec:%d\n",time_start.tv_sec);
		printf("time_start.tv_usec:%d\n\n",time_start.tv_usec);
		wait(NULL);	//等待子进程结束
		gettimeofday(&time_end,NULL);
		printf("time_end.tv_sec:%d\n",time_end.tv_sec);
		printf("time_end.tv_usec:%d\n",time_end.tv_usec);
		time_use = (time_end.tv_sec - time_start.tv_sec)*1000000 + (time_end.tv_usec - time_start.tv_usec);
		printf("此程序运行的时间为：%lf微秒",time_use);
	}
	return 0;
}

