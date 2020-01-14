/**
 *		mycp
 *	
 *	Copy source file or directory to destination.
 *	arguments and options: mycp <path> <path> [-options -count/ -show/ -time]
 *	
 *	Author: ZhouYC
 *	Date  : 2018-3-27
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>  
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <utime.h>


#define FILE_MAX		65536	/* define max size of buffer. */

/* arguments */
int	condition_show	= false;
int	condition_time	= false;
int	condition_count = false;
int	condition_debug = false;

int	cnt_file	= 0;
int	cnt_dir		= 0;

/* take path and other '-xxx' arguments from argv */


int isDir(char * path)
{
	struct stat _s;
	stat(path, &_s);
	return S_ISDIR(_s.st_mode);

//done
}

/* copy file attribute to dest

 */
int myCopyAttribute(char * path_src, char * path_dest, int fd_src, int fd_dest)
{
	struct	stat _s;
	struct utimbuf timebuf;

	stat(path_src, &_s); //获取文件信息，保存在stat所指的结构中
	//执行成功0
	fchmod(fd_dest, _s.st_mode); //权限保持一致
	timebuf.actime = _s.st_atime;  //accesstime 和 modifiedtime 一致
        timebuf.modtime = _s.st_mtime;  
	if(utime(path_dest, &timebuf ) < 0) {
		printf("failed to set time %s\n", path_dest);
	}
}

int myCopyFile(char * path_src, char * path_dest)
{
	int fd_src =  open(path_src, O_RDONLY, S_IRUSR);
	if (fd_src == -1) {
		printf("failed to open src file %s\n", path_src);
		return 0;
	}
	int fd_dest = creat(path_dest, 0666);		// read, write, not-exeute
	if (fd_dest == -1) {
		printf("failed to open dest file %s\n", path_dest);
		return 0;
	}

	int num_read = 0;	
	char * buffer_read[FILE_MAX+3];
	do {
		num_read = read(fd_src, buffer_read, FILE_MAX);  //
		write(fd_dest, buffer_read, num_read);  //写入buffer
	}
	while(num_read == FILE_MAX);

	myCopyAttribute(path_src, path_dest, fd_src, fd_dest);

	close(fd_src);
	close(fd_dest);

	if (condition_show) printf("%s\n", path_dest);
	++ cnt_file;
	return 1;
}

/* copy link, copy attri and time. */
int myCopyLink(char * path_src, char * path_dest)
{
	remove(path_dest);  //文件存在则清理


	int fd_src =  open(path_src, O_RDONLY, S_IRUSR);
	if (fd_src == -1) {
		printf("failed to open src file %s\n", path_src);
		return 0;
	}

	char link_src[FILENAME_MAX];
	int len = 0;
    int flag = 0;
	if ((flag = readlink(path_src, link_src, FILENAME_MAX-1)) != -1) {
        realpath(path_src,link_src);
        len= strlen(link_src);
		link_src[len] = 0;
		if (symlink(link_src, path_dest) == -1) {
			printf("%s\n", strerror(errno));
		}
	}
	int fd_dest = open(path_src, O_RDWR, S_IRUSR);	 	// read, write, not-exeute
	if (fd_dest == -1) {
		printf("failed to open dest file %s\n", path_dest);
		return 0;
	}
	struct	stat _s;
	struct	timeval timeam[2];
	lstat(path_src, &_s);
	fchmod(fd_dest, _s.st_mode);
	
	timeam[0].tv_sec = _s.st_atime;  
        timeam[1].tv_sec = _s.st_mtime;  

	if (lutimes(path_dest, timeam) == -1) {
		printf("Error %d: %s\n", errno, strerror(errno));
	}
	

	close(fd_src);
	close(fd_dest);
	if (condition_show) printf("%s\n", path_dest);
	++ cnt_file;
	return 1;

}

/* create dir with same attributes. */ 
void createDir(char * path_src, char * path_dest)
{
	struct stat _s;
	stat(path_src, &_s);
	
	if (mkdir(path_dest, _s.st_mode) == -1) { 
		//printf("cannot create dir %s (%s)\n", path_dest, strerror(errno));
	}

	if (condition_show) printf("%s\n", path_dest);
	++ cnt_dir;
}

/* recursively copy all files in dir */ 
int myCopyDir(char * path_src, char * path_dest)
{
	createDir(path_src, path_dest);
	
	DIR 		* dir = opendir(path_src);
	if (dir == NULL) {
		if (condition_show) printf("cannot open src dir \"%s\"\n", path_src);
		return 0;
	}

    	struct dirent 	* ptr;
	char 		subpath_src[FILENAME_MAX];
	char		subpath_dest[FILENAME_MAX];

	while ((ptr=readdir(dir)) != NULL) {
		if(ptr->d_name[0] == '.') continue;

		sprintf(subpath_src, "%s/%s", path_src, ptr->d_name);
		sprintf(subpath_dest, "%s/%s", path_dest, ptr->d_name);

		if(ptr->d_type ==  DT_REG)    ///file
			myCopyFile(subpath_src, subpath_dest);
		else if(ptr->d_type == DT_LNK)    ///link file
			myCopyLink(subpath_src, subpath_dest);
		else if(ptr->d_type == DT_DIR) {
			myCopyDir(subpath_src, subpath_dest);
		}
	}

	myCopyAttribute(path_src, path_dest, NULL, NULL);
	closedir(dir);
	return 1;
}

/* remove '\' or '/' in the end of path. */
void procPath(char * path)
{
	int i = strlen(path) - 1; 
	if (path[i] == '\\' || path[i] == '/') {
		do {
			path[i] = 0;
			i--;
		} while (i >= 0 && (path[i] == '\\' || path[i] == '/'));
	}
	
}

int main(int argc, char *argv[])
{
	char	str_src_path[FILENAME_MAX];
	char	str_dest_path[FILENAME_MAX];
	int	cnt_arg_path = 0;
	int	time_start = time(NULL), time_end;

	cnt_arg_path = collectArguments(argc, argv, str_src_path, str_dest_path);

	if (cnt_arg_path < 2) {
		printf("Invalid arguments.\n");
		printf("usage: mycp <path> <path> [-options]\n");
		printf("options: show, time, count, debug.\n");
		return 0;
	}
	
	procPath(str_src_path);
	procPath(str_dest_path);

	if (!isDir(str_src_path)) {
		//printf("src is a file.\n");
		if (condition_show) printf("%s\n", str_dest_path);
		myCopyFile(str_src_path, str_dest_path);
	}
	else {
		//printf("src is a dir.\n");
		
		myCopyDir(str_src_path, str_dest_path);
	}

	time_end = time(NULL);
	if (condition_show) printf("--------------------------------------------\n");
	if (condition_count) printf("Copy files: %d, directories: %d\n", cnt_file, cnt_dir);
	if (condition_time) printf("Time used: %d s.\n", time_end - time_start);
	if (condition_show || condition_count || condition_time) printf("\n");

	return 0;

}
