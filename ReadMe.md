# NetCal (C++)

## Overview

**NetCal** is a modular multi-threaded client-server calculator built in **Modern C++** using **TCP socket programming** and a custom **Thread Pool** implementation.

The project demonstrates low-level networking, concurrent task execution, expression parsing, and modular software design. Instead of spawning a new thread for every client connection, NetCal uses a fixed-size thread pool to efficiently serve multiple clients while maintaining predictable resource usage.

---

## Features

* TCP-based client-server communication
* Custom Thread Pool implementation
* Fixed-size circular task queue
* Concurrent multi-client support
* Expression parsing and evaluation
* Interactive keyboard input
* File-based batch processing
* Modular C++ architecture
* Graceful worker shutdown

---

## Thread Pool Architecture

### Design Idea

Creating a new thread for every incoming connection is expensive and does not scale well.

NetCal uses a **Thread Pool model**:

* A fixed number of worker threads are created during server startup.
* Incoming client connections are converted into tasks.
* Tasks are inserted into a shared circular queue.
* Worker threads continuously fetch and execute tasks.
* The number of threads remains constant regardless of the number of clients.

This approach reduces:

* Thread creation overhead
* Context switching cost
* Memory consumption
* Scheduling overhead

---

### Components

#### Main Thread

Responsible for:

* Creating the server socket
* Binding and listening on a TCP port
* Accepting incoming connections
* Submitting client requests to the thread pool

---

#### Worker Threads

Each worker thread:

* Waits if the queue is empty
* Dequeues the next task
* Reads client requests
* Parses and evaluates expressions
* Sends the result back
* Closes the client connection

---

#### Task Queue

The thread pool uses:

* Fixed-size circular buffer
* Producer-consumer synchronization
* Mutex for shared queue protection
* Condition variables for blocking and signalling

Features:

* Queue Capacity: `MAX_QUEUE = 100`
* Producers block when queue is full
* Workers sleep when queue is empty
* Graceful shutdown support

---

## System Flow

```text
Client                          Server

  |                               |
  | socket()                      | socket()
  |                               |
  | connect() ------------------> | bind()
  |                               | listen()
  |                               |
  |                               | accept()
  |                               |
  |                               v
  |                    threadpool_submit()
  |                               |
  |                               v
  |                      Worker Thread
  |                               |
  | send("12+5=") -------------> read()
  |                               |
  |                        parse()
  |                        calculate()
  |                               |
  | recv("17") <--------------- send("17")
  |                               |
  | send("Q") -----------------> close()
  |                               |
close()                       Worker Ready
```

---

## Tech Stack

* C++17
* POSIX TCP Sockets
* POSIX Threads (pthread)
* Mutex and Condition Variables
* Circular Queue
* Linux System Programming

---

## Project Structure

```text
NetCal/

├── src
│   ├── Main.cpp
│   │
│   ├── Core
│   │   ├── Calculator.cpp
│   │   └── Parser.cpp
│   │
│   ├── Network
│   │   ├── Server.cpp
│   │   ├── Client.cpp
│   │   └── tpool.cpp
│   │
│   └── io
│       └── Input.cpp
│

├── include
│   ├── Core
│   │   ├── Calculator.hpp
│   │   └── Parser.hpp
│   │
│   ├── Network
│   │   ├── Server.hpp
│   │   ├── Client.hpp
│   │   └── tpool.hpp
│   │
│   └── io
│       └── Input.hpp
│

└── Data
    └── file.txt
```

---

## Build

Compile using:

```bash
g++ -std=c++17 \
src/Core/*.cpp \
src/io/*.cpp \
src/Network/*.cpp \
src/Main.cpp \
-Iinclude \
-lpthread \
-o app
```

---

## Run Modes

### Keyboard Mode

```bash
./app -k
```

* Enter expression using keyboard
* Press Enter to evaluate
* Press Esc to exit

---

### File Mode

```bash
./app -f
```

Reads expressions from:

```text
Data/file.txt
```

and prints results to stdout.

---

### Client-Server Mode

Start Server:

```bash
./app -C

Select:
1 -> Server
```

Start Client:

```bash
./app -C

Select:
2 -> Client
```

Multiple clients can connect simultaneously.

---

## Example Expressions

```text
12+5=

10*3=

100/5=

50%6=
```

Supported Operators:

```text
+
-
*
/
%
```

Multiple expressions can be chained:

```text
12+5=10*3=
```

---

## Key Learnings

This project helped me understand:

* TCP socket lifecycle
* Client-server architecture
* Thread Pool design
* Producer-consumer synchronization
* Circular queue implementation
* Mutex and condition variables
* Concurrent programming in C++
* Expression parsing and evaluation
* Modular software architecture

---

## Future Improvements

* Async networking using Boost.Asio

* Configurable port and thread pool size

* Logging system

* Unit testing using Google Test

* Docker support

* Event-driven server using epoll

---

## Author

**Neel Patil**

Computer Science Student
