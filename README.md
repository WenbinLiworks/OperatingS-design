# OperatingS-design
[![stable](http://badges.github.io/stability-badges/dist/stable.svg)](http://github.com/badges/stability-badges)
<br>
 operating system design project, which contains Kernel Compilation, Process Synchronization, Producer-consumer problem, memory monitoring, Files copying.

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

#### Linux: create, read, write and other system calls. Soft links are required

#### Windows: createfile(), readfile(), writefile(), closehandle() and other functions

#### Pay special attention to the consistency of permissions and time attributes after replication.

# Producer-consumer Problem
A buffer of size 3, initially empty

• 2 producers

– wait at random for a period of time to add data to the buffer,

– if the buffer is full, wait for the consumer to retrieve the data before adding

– repeat 6 times

• 3 consumers

– randomly wait for a period of time to read data from the buffer

– if the buffer is empty, wait for the producer to add data before reading

– repeat 4 times

#### Explain:

#### Display the time of each data addition and reading and the data in the buffer

#### Process simulation for producers and consumers

# Process Control
Design and implement UNIX "time" command. The "mytime" command accepts the program to be run through command line parameters, creates a separate process to run the program, and records the running time of the program.

#### Under Windows:

- Use createprocess() to create a process
- Use waitforsingleobject() to synchronize the "mytime" command with the newly created process
- Call getsystemtime() to get the time
#### Under Linux:

- Use fork() / VFORK / exec() to create a process runner
- Use wait() to wait for the newly created process to finish
- Call gettimeofday() to get the time
- Usage of mytime: $mytime.exe program1

```
Time required for output program1 to run. Pragram1 can be a program written for itself or an application in the system.

$ mytime.exe program2 t

T is the time parameter, is the input parameter of program2, and controls the running time of program2. Finally, the running time of output program2 should be close to t.

Display result: * * hour * * minute * * second * * millisecond * * microsecond
```

