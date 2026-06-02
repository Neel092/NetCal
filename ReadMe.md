#  NetCal (C)

## Overview

A modular client-server calculator built in C using socket programming.  
Now supports **concurrent multi-client handling** using a **Thread-per-Client model**, allowing multiple users to interact with the server simultaneously.

---

##  Features

- TCP-based Client-Server communication  
- Thread-per-Client concurrency model  
- Multiple clients handled simultaneously  
- Expression parsing and evaluation  
- File-based input processing  
- Interactive keyboard input (getch-based)  
- Modular architecture (Parser, Calculator, Networking)  

---

##  Thread-per-Client Architecture

###  Idea
Each client connection is handled in a **separate thread**, enabling parallel communication.


---

###  How It Works

**Main Thread:**
- Accepts incoming client connections  
- Creates a new thread for each client  

**Worker Thread:**
- Handles full communication lifecycle  
- Reads input → processes → sends response  
- Closes connection independently  

---

##  Tech Stack

* C (System Programming)
* POSIX Sockets (Networking)
* Modular Design (Multiple source files)

---

##  Project Structure

```
src/
  core/
  network/
  io/
include/
```

---

##  How to Run

### Compile:

```
gcc src/Core/*.c src/io/*.c src/Network/*.c src/Main.c -Iinclude -o app
```

### Run modes:

#### Keyboard:

```
./app -k
```

#### File:

```
./app -f
```

#### Client-Server:

```
./app -C
```

---

##  Example Input 

```
12+5=
10*3=
```


---

##  Learnings

- Socket programming (TCP lifecycle)  
- Multithreading with POSIX Threads  
- Concurrent server design (Thread-per-Client)  
- Memory management in concurrent systems  
- Modular system design in C  

---

##  Future Improvements

- Thread Pool implementation (scalable alternative)  
- Event-driven model using `select()` / `epoll()`  
- Improved error handling & validation  
- Logging and monitoring system  
- Containerization (Docker)  

---
