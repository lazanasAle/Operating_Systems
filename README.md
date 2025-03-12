# Operating Systems Course Projects

This repository contains the code developed by me and my teammate for the **Operating Systems** course projects. The work is split into two main projects: a **Multiprocessing System Scheduler Simulation** and a **Shell-Memory Management-Threading Project**.

---

## 📂 Project 1: Scheduler Project (Located in folder scheduler_project)

### Overview

This project simulates a **Multiprocessing System Scheduler** implemented in **C++**. The simulation models a system with:
- A **parent process** acting as the **system manager**.
- Multiple **child processes**, each representing an individual processor.
- Each processor runs a **scheduling algorithm** to manage its assigned processes.

There are **two versions** of the scheduler:

---

### 📁 Folder: `scheduler_v1`

In this version, all processors use the **same scheduling algorithm** during a single execution. The supported algorithms include:

| Algorithm | Description |
|---|---|
| **FCFS** | First-Come-First-Served (FIFO queue) |
| **RR** | Round Robin (configurable time quantum) |
| **RRAFF** | Round Robin with **Processor Affinity** (a process always runs on the same processor once assigned) |

---

### 🔧 Technologies Used

- **Language:** C++
- **System Calls:** `fork()`, `execl()`, `waitpid()`, `kill()`
- **Inter-Process Communication (IPC):**
    - Shared Memory: `boost::interprocess::managed_shared_memory`
    - Synchronization: `boost::interprocess::interprocess_mutex`
    - Signal Handling: `<csignal>` (handling `SIGCHLD` when processes terminate)

---

### 🚀 Setup & Usage

1. Clone the repository.
2. Navigate to `scheduler_v1`. Inside, you’ll find:
    - `scheduler/` - Contains the scheduler’s source code and configuration files.
    - `work/` - Contains the 7 processes to be scheduled.
3. In `work/`, run:
    ```sh
    make
    ```
    This will compile the 7 processes into executables.
4. In `scheduler/`, run:
    ```sh
    make
    ```
5. Run the scheduler with:
    ```sh
    ./scheduler FCFS file.txt         # For FCFS algorithm
    ./scheduler RR <time-slice> file.txt  # For Round Robin (time-slice in ms)
    ./scheduler RRAFF <time-slice> file.txt  # For RRAFF (time-slice in ms)
    ```
6. `file.txt` must follow the structure of the provided `reverse.txt` in the `scheduler` folder.

---

### 📁 Folder: `scheduler_v2`

In this version, a process can reserve **multiple processors** at the same time. All processors run the **FCFS algorithm**.

---

### 🚀 Setup & Usage

1. Clone the repository.
2. Navigate to `scheduler_v2`. Inside, you’ll find:
    - `scheduler/` - Contains the scheduler’s source code and configuration files.
    - `work/` - Contains the 7 processes to be scheduled.
3. In `work/`, run:
    ```sh
    make
    ```
4. In `scheduler/`, run:
    ```sh
    make
    ```
5. Run the scheduler with:
    ```sh
    ./scheduler file.txt
    ```
6. `file.txt` must follow the structure of `reverse.txt` in the `scheduler` folder.

---

### 📁 Folder: `c_scheduler`
This is just a C-language version of scheduler_v1

---

1. Clone the repository.
2. Navigate to `c_scheduler`. Inside, you’ll find:
    - `scheduler/` - Contains the scheduler’s source code and configuration files.
    - `work/` - Contains the 7 processes to be scheduled.
3. In `work/`, run:
    ```sh
    make
    ```
    This will compile the 7 processes into executables.
4. In `scheduler/`, run:
    ```sh
    make
    ```
5. Run the scheduler with:
    ```sh
    ./scheduler FCFS file.txt         # For FCFS algorithm
    ./scheduler RR <time-slice> file.txt  # For Round Robin (time-slice in ms)
    ./scheduler RRAFF <time-slice> file.txt  # For RRAFF (time-slice in ms)
    ```
6. `file.txt` must follow the structure of the provided `reverse.txt` in the `scheduler` folder.

---

## 📂 Project 2: Shell, Memory Management & Threading Project (Located in the folder shell_memoryMan_threading)

This project contains **3 exercises**, each solving different problems related to operating systems:

### 📁 Folder: `exrs1`

- Contains a **shell script** that performs basic file configurations using `awk`.

---

### 📁 Folder: `exrs2`

- Contains the implementation of the **Boat Rescuing Concurrency Problem**, demonstrating synchronization techniques (threads, mutexes, semaphores).

---

### 📁 Folder: `exrs3`

- Implements the **First-Fit Memory Allocation Algorithm**, demonstrating dynamic memory management in operating systems.

---

## 📜 Final Notes

These projects were part of the **Operating Systems** course and aim to demonstrate practical understanding of:
- Process management
- Scheduling algorithms
- Inter-process communication
- Synchronization
- Memory management
- Basic shell scripting

---
