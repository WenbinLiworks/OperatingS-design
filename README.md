# OperatingS-design
 operating system design project, which contains Kernel Compilation, Process Synchronization, Producer-consumer problem, memory monitoring, Files copying, and MIT ucore

# Memory-Monitoring
## Windows
<br>(1) . learn how applications allocate memory
<br>(2) . learn about windows memory structure and virtual memory management, learn how to manage memory in applications, and realize the simplicity of using memory in Windows 
<br>(3) . understand the usage of memory in the current system, including the layout of system address space and the usage of physical memory; display the virtual address space and working set information of a process, etc
## Linux
<br>(1) . understand the command top and its subcommand functions of viewing system
<br>(2) . learn to view the command PS-A of all currently running processes, and find the PID of a specific process
<br>(3) . learn the command top-p PID to view the status of a process
<br>(4) . learn the command pmap-d PID to view the memory usage of a process

# Files Copying
Complete a directory copy command mycp, including files and subdirectories under the directory. The operation results are as follows:

```beta@bugs.com [~/]# ls –la sem

Total 56

drwxr-xr-x 3 beta beta 4096 Dec 19 02:53 ./

drwxr-xr-x 8 beta beta 4096 Nov 27 08:49 ../

-rw-r--r-- 1 beta beta 128 Nov 27 09:31 Makefile

-rwxr-xr-x 1 beta beta 5705 Nov 27 08:50 consumer*

-rw-r--r-- 1 beta beta 349 Nov 27 09:30 consumer.c

drwxr-xr-x 2 beta beta 4096 Dec 19 02:53 subdir/

beta@bugs.com [~/]# mycp sem target

beta@bugs.com [~/]# ls –la target

Total 56

drwxr-xr-x 3 beta beta 4096 Dec 19 02:53 ./

drwxr-xr-x 8 beta beta 4096 Nov 27 08:49 ../

-rw-r--r-- 1 beta beta 128 Nov 27 09:31 Makefile

-rwxr-xr-x 1 beta beta 5705 Nov 27 08:50 consumer*

-rw-r--r-- 1 beta beta 349 Nov 27 09:30 consumer.c

drwxr-xr-x 2 beta beta 4096 Dec 19 02:53 subdir/
```

### Explain:

### Linux: create, read, write and other system calls. Soft links are required

### Windows: createfile(), readfile(), writefile(), closehandle() and other functions

### Pay special attention to the consistency of permissions and time attributes after replication.
