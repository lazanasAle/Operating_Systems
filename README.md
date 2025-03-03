
# My Operating_Systems course projects

This repository contains the code me and my teampartner used for the project in the course Operating Systems

1) The folder scheduler_project:

   This folder contains a **Multiprocessing System Scheduler Simulation** implemented in **C++**. The simulation models a system with **a parent process acting as the system manager**, and multiple **child processes acting as individual processors**. Each **processor (child process)** runs a **scheduling algorithm** to manage its            assigned processes. Two versions of this sv=cheduler exist there:
   
     1) Folder scheduler_v1:
        All processors use the **same scheduling algorithm** during a given execution, and the supported algorithms include:

      - **Round Robin (RR)**
      - **First-Come-First-Served (FCFS)**
      - **Round Robin with Affinity (RRAFF)**

        ## 💡 Project Overview

      This project simulates a **multi-processor system scheduler** with:

      ✅ A **parent process** that spawns a constant number of **child processes** (each representing a processor).  
      ✅ Each **processor (child)** executes processes using a common scheduling algorithm.  
      ✅ Communication between parent and children is achieved via **IPC (shared memory and signals)**.  
      ✅ The output tracks key scheduling events (process start, preemption, completion, etc.).

      ---

      ## 🔄 Supported Scheduling Algorithms

      | Algorithm | Description |
      |---|---|
      | **FCFS** | First-Come-First-Served (simple FIFO queue) |
      | **RR** | Round Robin with configurable time quantum |
      | **RRAFF** | Round Robin with **Affinity** - Once a process starts on a processor, it **must always run on the same processor** (processor affinity) |

      ---

      ## 💻 Technologies

      - Language: **C++**
      - System Calls: `fork()`, 'execl()', `waitpid()`, 'kill()'
      - IPC: We used 'boost::interprocess::managed_shared_memory' for shared memory usage, 'boost::interprocess_mutex' for mutual exclusion when processors access the shared queue. For signal handling we used the library '<csignal>' and handled SIGCHLD signal when the scheduled processes (child processes of the processors) terminate.
      -      

      --

      ## 🚀 Setup & Usage

            - clone the repo
            - go to the folder scheduler_v1, you will find two folders scheduler and work, the folder work is destined to contain the seven processes that the system shall schedule, the folder scheduler contains all the source files of the scheduler and two text files that just have an order for the to-be scheduled processes to be                     started. Run the command make in the folder work to create the 7 executable files.
            - Now go to the folder scheduler and run make then to test run the command: ./scheduler FCFS ''file.txt'' (for FCFS algorithm) ./scheduler RR ''time-slice in milliseconds'' ''file.txt''(for RR replace RR with RRAFF if you want to run the RRAFF algorithm (for operational purposes timeslice should be 2 seconds or more))
   2) Folder scheduler_v2:
      Now a to be scheduled process can reserve more than one processors simultaneously and each one runs only FCFS algorithm. We used the same technologies with scheduler_v1

      ## 🚀 Setup & Usage

            - clone the repo
            - go to the folder scheduler_v2, you will find two folders scheduler and work, the folder work is destined to contain the seven processes that the system shall schedule, the folder scheduler contains all the source files of the scheduler and two text files that just have an order for the to-be scheduled processes to be                     started. Run the command make in the folder work to create the 7 executable files.
            - Now go to the folder scheduler and run make then to test run the command: ./scheduler ''file.txt''
      In both versions ''file.txt'' must have the respective structure of the ''reverse.txt'' file that exists in the folder ''scheduler'' of both versions




  2) Folder shell_memoryMan_threading:
     This was the easy project of the couse we just have 3 folders exrsX for each exercise of the project. In the first folder (exrs1) there is a basic shell-script that achieves some file configuration using awk. In the second we have the implementation of the solution of a boat rescuing concurrency problem. In the third we have the implementation of the first-fit algorithm in a system.
                        
