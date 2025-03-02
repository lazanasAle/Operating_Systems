scheduler_project contains a c++ process scheduler that executes many binary files (processes) and schedules them using FCFS and round robin, the system simulates a multiple processor system with each processor scheduling either processes from his own queue (rraff algorithm) or from ashared one (rr and fcfs algorithms) 
these are all in scheduler_v1 folder (each process is running only on one processor and needs only one processor to run)
In scheduler_v2 folder fcfs is implemented again but this time a process may need more than one processors to run.
work folder contains the executable files where the testing of each scheduler exists.

to use correctly run the command make inside the work folder (which exists in every scheduler_vX folder) afterwords run the same command inside the scheduler folder.

shell_memoryMan_threading has three excercises implemented in C++ one solving a multithreading problem, a shell script and a simple code for first fit algorithm in around-robin system.

