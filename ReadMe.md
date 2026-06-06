# NetCal (C)

## Overview

A modular client-server calculator built in C using socket programming.
Supports concurrent multi-client handling using a **Thread Pool model**, allowing multiple users to interact with the server simultaneously while keeping resource usage under control.

---

## Features

- TCP-based client-server communication
- Thread Pool concurrency model (4 workers, circular task queue)
- Multiple clients handled simultaneously
- Expression parsing and evaluation
- File-based batch input processing
- Interactive keyboard input (character-by-character via `getch`)
- Modular architecture (Parser, Calculator, Networking, I/O)

---

## Thread Pool Architecture

### Idea

Instead of spawning a new thread per client, incoming connections are submitted as tasks to a pre-allocated pool of worker threads. This keeps memory and scheduling overhead constant regardless of connection volume.

---

### How It Works

**Main Thread:**
- Accepts incoming client connections
- Submits each connection as a task to the thread pool

**Worker Thread:**
- Waits on a condition variable when the queue is empty
- Dequeues and executes the next task when signalled
- Handles the full client lifecycle: read, evaluate, respond, close

**Task Queue:**
- Fixed-size circular buffer (MAX_QUEUE = 100)
- Producers block when the queue is full (back-pressure)
- Shutdown is cooperative: workers drain the queue, then exit cleanly on join

---

## System Flow

```
Client                          Server
  |                               |
  | ./app -C --> select 2         | ./app -C --> select 1
  |                               |
  v                               v
client()                       server()
  |                               |
  | socket()                      | socket() + bind() + listen()
  |                               |
  | connect(127.0.0.1:8080) ----> accept()
  |                               |
  |                               v
  |                          threadpool_submit(Process, client_fd)
  |                               |
  |                               v
  |                          worker thread: Process(client_fd)
  |                               |
  | send("12+5=") --------------> read(buffer)
  |                               |
  |                               v
  |                          parse_and_calculate("12+5=")
  |                               |
  |                               v
  |                          calculate(12, '+', 5)
  |                               |
  | recv("17")  <---------------- send("17")
  |                               |
  | print "Server replied: 17"    |
  |                               |
  | send("Q") ------------------> detect 'Q'
  |                               |
  v                               v
close(sock)                  close(client_fd)
```

---

## Tech Stack

- C (C99, System Programming)
- POSIX Sockets (TCP/IPv4 Networking)
- POSIX Threads (pthread, mutex, condition variables)
- Modular design (multiple translation units)

---

## Project Structure

```
NetCal/
├── src/
│   ├── Main.c
│   ├── Core/
│   │   ├── Calculator.c
│   │   └── Parser.c
│   ├── Network/
│   │   ├── Server.c
│   │   ├── Client.c
│   │   └── tpool.c
│   └── io/
│       └── Input.c
├── include/
│   ├── conio.h
│   ├── Core/
│   │   ├── Calculator.h
│   │   └── Parser.h
│   ├── Network/
│   │   ├── Server.h
│   │   ├── Client.h
│   │   └── tpool.h
│   └── io/
│       └── Input.h
└── Data/
    └── file.txt
```

---

## How to Run

### Compile

```bash
gcc src/Core/*.c src/io/*.c src/Network/*.c src/Main.c -Iinclude -lpthread -o app
```

### Run Modes

#### Keyboard

```bash
./app -k
```

Enter digits and operators one key at a time. Press `Enter` to evaluate. Press `Esc` to exit.

#### File

```bash
./app -f
```

Evaluates all expressions in `Data/file.txt` and prints results to stdout.

#### Client-Server

```bash
# Terminal 1 - start server
./app -C
# Select: 1

# Terminal 2 - start client
./app -C
# Select: 2
```

---

## Example Input

```
12+5=
10*3=
100/5=
50%6=
```

Supported operators: `+`  `-`  `*`  `/`  `%`

Multiple expressions can be chained on a single line:

```
12+5=10*3=
```

---

## Learnings

- TCP socket lifecycle (`socket`, `bind`, `listen`, `accept`, `read`/`send`, `close`)
- Thread pool design with POSIX mutex and condition variable synchronisation
- Circular task queue with producer-consumer back-pressure
- Memory management in concurrent systems (heap-allocated `client_fd` per connection)
- Modular system design in C with separated headers and translation units

---

## Future Improvements

- Event-driven model using `select()` / `epoll()`
- Async networking using Asio (standalone or Boost.Asio) as an alternative to the POSIX thread pool
- Improved error handling and validation throughout networking layer
- Configurable port and pool size via CLI arguments
- Makefile for cleaner builds
- Logging and monitoring system
- Containerization (Docker)

---

## Author

Neel Patil