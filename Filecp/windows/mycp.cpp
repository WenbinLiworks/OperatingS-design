/**
*		mycp
*
*	Copy source file or directory to destination.
*	arguments and options: mycp <path> <path> [-options -count/ -show/ -time]
*
*	Author: Wenbin Li
*	Date  : 2019-12-15
*/

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <sys/stat.h>
#include <io.h>  
#include <time.h>

#define FILE_MAX		65536	/* define max size of buffer. */

/* arguments */
BOOL	condition_show	= FALSE;
BOOL	condition_time	= FALSE;
BOOL	condition_count = FALSE;
BOOL	condition_debug = FALSE;

int		cnt_file		= 0;
int		cnt_dir			= 0;

/* take path and other '-xxx' arguments from argv */
int collectArguments(int argc, char *argv[], CHAR * str_src_path, CHAR * str_dest_path)
{
	int cnt_arg_path = 0;
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (strcmp(argv[i], "-show") == 0) condition_show = TRUE;
			else if (strcmp(argv[i], "-time") == 0) condition_time = TRUE;
			else if (strcmp(argv[i], "-count") == 0) condition_count = TRUE;
			else if (strcmp(argv[i], "-debug") == 0) condition_debug = TRUE;
			else printf("Invalid argument: \"%s\".\n", argv[i]);
			continue;
		}
		else {
			switch (cnt_arg_path) {
			case 0: sprintf(str_src_path, "%s", argv[i]); break;
			case 1: sprintf(str_dest_path, "%s", argv[i]); break;
			default: printf("Invalid argument: \"%s\".\n", argv[i]);
			}
			cnt_arg_path++;
		}
	}
	if (cnt_arg_path < 2) {
		if (condition_debug) {
			if (cnt_arg_path == 0) {
				sprintf(str_src_path, "%s", "E:\\tmp\\");
				sprintf(str_dest_path, "%s", "E:\\tmp_new\\");
			}
			else if (cnt_arg_path == 1) {
				sprintf(str_dest_path, "%s", "E:\\tmp_new");
			}
			cnt_arg_path = 2;
		}
		
	}
	return cnt_arg_path;
}

BOOL isDir(CHAR * path)
{
	struct _stat64i32	stat_src;
	_stat(path, &stat_src);

	if (stat_src.st_mode & _S_IFDIR) return TRUE;
	else return FALSE;
}

/* copy attributes from src file. */
void myCopyAttribute(CHAR * path_src, CHAR * path_dest, HANDLE handle_src, HANDLE handle_dest)
{
	DWORD		dword_attrs = GetFileAttributesA(path_src);
	FILETIME	filetime_create, filetime_access, filetime_modify;

	SetFileAttributesA(path_dest, dword_attrs);

	GetFileTime(handle_src, &filetime_create, &filetime_access, &filetime_modify);
	SetFileTime(handle_dest, &filetime_create, &filetime_access, &filetime_modify);
}

/* copy file. */
BOOL myCopyFile(CHAR * path_src, CHAR * path_dest)
{
	CHAR	buffer_read[FILE_MAX+3];
	DWORD	num_read;

	HANDLE	handle_src = CreateFileA(
		path_src,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ARCHIVE,
		NULL
	);
	if (handle_src == INVALID_HANDLE_VALUE) {
		if (condition_show) printf("Failed to open source file: \"%s\".\n", path_src);
		return FALSE;
	}

	HANDLE	handle_dest = CreateFileA(
		path_dest,
		GENERIC_WRITE | GENERIC_READ,
		NULL,
		NULL,
		CREATE_ALWAYS,		// CREATE_ALWAYS: wipe file's content, OPEN_ALWAYS: just open.
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE,
		handle_src
	);
	if (handle_dest == INVALID_HANDLE_VALUE) {
		if (condition_show) printf("Failed to open destination file: \"%s\".\n", path_dest);
		return FALSE;
	}

	do {
		ReadFile(handle_src, buffer_read, FILE_MAX, &num_read, NULL);
		WriteFile(handle_dest, buffer_read, num_read, NULL, NULL);
		//printf("%d bytes is copied.\n", num_read);
	} while (num_read == FILE_MAX);

	myCopyAttribute(path_src, path_dest, handle_src, handle_dest);

	CloseHandle(handle_src);
	CloseHandle(handle_dest);

	++ cnt_file;
	return TRUE;
}

/* remove '\' or '/' in the end of path. */
void procPath(CHAR * path)
{
	int i = strlen(path) - 1; 
	if (path[i] == '\\' || path[i] == '/') {
		do {
			path[i] = 0;
			i--;
		} while (i >= 0 && (path[i] == '\\' || path[i] == '/'));
	}
	
}

/* recursively create new folder and copy all files in the folder. (copy dir with same construction.)*/
void myCopyDir(CHAR * path_src, CHAR * path_dest)
{
	/*
		for subpath in path_src:
			if subpath is file:
				myCopyFile(path_src'/'subpath, path_dest'/'subpath)
			else :	//(if subpath is dir:)
				createDir(path_dest'/'subpath)
				myCopyDir(path_src'/'subpath', path_dest'/'subpath)
	*/

	CreateDirectoryA(path_dest, NULL);
	++ cnt_dir;
	HANDLE				handle_dir_src = CreateFileA(
		path_src,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);
	HANDLE				handle_dir_dest = CreateFileA(
		path_dest,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);
	//printf("%p %p\n", handle_dir_src, handle_dir_dest);
	myCopyAttribute(path_src, path_dest, handle_dir_src, handle_dir_dest);
	CloseHandle(handle_dir_dest);
	CloseHandle(handle_dir_src);

	//CopyFileA(path_src, path_dest, FALSE);		// this function is invalid to create new folder... dont use it.
	
	HANDLE				handle_find;
	WIN32_FIND_DATAA	finddata;
	CHAR				pattern_find[FILENAME_MAX];
	CHAR				subpath_src[FILENAME_MAX], subpath_dest[FILENAME_MAX];

	memset(&finddata, 0, sizeof(WIN32_FIND_DATAA));
	sprintf(pattern_find, "%s\\*", path_src);

	handle_find = FindFirstFileA(pattern_find, &finddata);
	if (handle_find != INVALID_HANDLE_VALUE) {
		do { //循环找文件
			if (finddata.cFileName[0] == '.') continue;

			sprintf(subpath_src, "%s\\%s", path_src, finddata.cFileName);
			sprintf(subpath_dest, "%s\\%s", path_dest, finddata.cFileName);
			if (condition_show) printf("%s\n", subpath_dest);
		
			if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				myCopyDir(subpath_src, subpath_dest); //是否存在该目录
			}
			else {
				myCopyFile(subpath_src, subpath_dest); //是否是目录文件
			}
		} while (FindNextFileA(handle_find, &finddata)); //递归进入子目录复制

		CloseHandle(handle_find);
	}
}

int main(int argc, char *argv[])
{
	CHAR		str_src_path[FILENAME_MAX];
	CHAR		str_dest_path[FILENAME_MAX];
	int			cnt_arg_path = 0;
	int			time_start = time(NULL), time_end;

	cnt_arg_path = collectArguments(argc, argv, str_src_path, str_dest_path);

	if (cnt_arg_path < 2) {
		printf("Invalid arguments.\n");
		printf("usage: mycp <path> <path> [-options]\n");
		printf("options: show, time, count, debug.\n");
		return 0;
	}
	
	//printf("argc: %d, cnt_arg_path: %d, src_path: %s, dest_path: %s\n", argc, cnt_arg_path, str_src_path, str_dest_path);

	if (!isDir(str_src_path)) {
		//printf("src is a file.\n");
		if (condition_show) printf("%s\n", str_dest_path);
		myCopyFile(str_src_path, str_dest_path);
	}
	else {
		procPath(str_src_path);
		procPath(str_dest_path);

		myCopyDir(str_src_path, str_dest_path);
	}

	time_end = time(NULL);
	if (condition_show) printf("--------------------------------------------\n");
	if (condition_count) printf("Copy files: %d, directories: %d\n", cnt_file, cnt_dir);
	if (condition_time) printf("Time used: %d s.\n", time_end - time_start);
	if (condition_show || condition_count || condition_time) printf("\n");

	return 0;

}
