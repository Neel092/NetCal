# NetCal

A multi-threaded client-server calculator written in **C++17**. Send arithmetic expressions over TCP, get results back. Handles multiple clients simultaneously using a hand-rolled thread pool — no new thread per connection.

---

## What It Does

- Parses and evaluates expressions like `12+5=` or `100/4=`
- Accepts multiple simultaneous client connections
- Server maintains a fixed pool of 4 worker threads — clients queue up, never spin up new threads
- Three modes: keyboard, file batch, and client-server over TCP

---

## Prerequisites

**Compiler:** g++ with C++17 support

**ASIO** (standalone, no Boost):

```bash
# Ubuntu / Debian
sudo apt install libasio-dev

# Arch
sudo pacman -S asio

# macOS
brew install asio
```

> If you install ASIO manually, download from https://think-async.com/Asio/ and pass `-I/path/to/asio/include` during compilation.

---

## Build

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

No `-lboost_system` needed — this uses standalone ASIO.

---

## Run

### Keyboard Mode

```bash
./app -k
```

Enter digits and operators one key at a time. Press `Enter` to evaluate, `Esc` to quit.

```
>> 1
1
>> 2
12
>> +
>> 5
5
>> (Enter)
12 + 5 = 17
```

---

### File Mode

```bash
./app -f [filepath]
```

Pass any file path after `-f`. If no path is given, defaults to `Data/file.txt`.

```bash
./app -f                            # uses Data/file.txt (default)
./app -f Data/file.txt              # same, explicit
./app -f /home/neel/expressions.txt # absolute path
./app -f myexpressions.txt          # relative path
```

Each expression in the file must end with `=`. One expression per line.

```text
10-20=
15+25=
30*4=
100/5=
50%6=
999+1=*5=      ← chained: evaluates 999+1, then result*5
```

Output:

```
10-20= -10
15+25= 40
30*4= 120
...
```

---

### Client-Server Mode

Start the server:

```bash
./app -C
# prompt: 1 => Start Server
1
```

In separate terminals, start one or more clients:

```bash
./app -C
# prompt: 2 => Start Client
2
```

Client session:

```
Enter numeric value for calculation :-
12+5=
Result: 17

Enter numeric value for calculation :-
100/4=
Result: 25

Enter numeric value for calculation :-
q
Exiting...
```

Type `Q` or `q` to end a client session. Server keeps running for other clients.

---

## Supported Operators

| Operator | Example  | Result |
|----------|----------|--------|
| `+`      | `12+5=`  | 17     |
| `-`      | `90-100=`| -10    |
| `*`      | `10*3=`  | 30     |
| `/`      | `100/5=` | 20     |
| `%`      | `50%6=`  | 2      |

Expressions must end with `=`. Chaining is supported: `999+1=*5=` evaluates left to right.

---

## Architecture

### Thread Pool

The server creates 4 worker threads at startup. They stay alive the entire time — no thread creation per connection.

```
Main Thread                     Worker Threads (x4)
    |                                   |
accept() → client_fd                    | waiting on condition variable
    |                                   |
pool.submit(Process, client_fd)  -----> | wakes up, dequeues task
    |                                   |
socket.release()                        | socket.assign(client_fd)
(transfers fd ownership)                | read → parse → write → close
```

The task queue is a circular buffer (capacity 100). If all workers are busy and the queue is full, the main thread blocks until a slot opens — no connections are dropped.

### Shutdown

When the server exits, `~threadpool_t()` sets `shutdown = 1`, broadcasts to all sleeping workers, and joins every thread before freeing memory.

---

## Project Structure

```
NetCal/
├── src/
│   ├── Main.cpp                  Entry point, argument dispatch
│   ├── Core/
│   │   ├── Calculator.cpp        calculate(num1, num2, op)
│   │   └── Parser.cpp            parse_and_calculate(expr_string)
│   ├── Network/
│   │   ├── Server.cpp            TCP acceptor + thread pool submission
│   │   ├── Client.cpp            TCP client send/receive loop
│   │   └── tpool.cpp             Thread pool implementation
│   └── io/
│       └── Input.cpp             Mode dispatch: -k / -f / -C
│
├── include/                      Header files mirroring src/
│
└── Data/
    └── file.txt                  Expressions for file mode
```

---

## How the Code Fits Together

```
argv
 └── CheckInput()          [io/Input.cpp]
      ├── -k → convert()              keyboard loop using getch()
      ├── -f [path] → FileInput(path)  reads given file, defaults to Data/file.txt
      └── -C → server() or client()
                │
                ├── server()          [Network/Server.cpp]
                │    └── threadpool_t pool(4)
                │         └── pool.submit(Process, client_fd)
                │              └── Process()
                │                   └── parse_and_calculate(buffer)
                │                        └── calculate(n1, n2, op)
                │
                └── client()          [Network/Client.cpp]
                     └── fgets → asio::write → read_some → print
```

---

## Known Limitations

- Integer arithmetic only (no floats)
- Port is hardcoded to `8080`
- Thread count is hardcoded to `4`
- No logging — errors print to stdout

---

## Possible Next Steps

- `--port` and `--threads` CLI flags
- Float support in the parser
- File + stderr logging
- Unit tests (Google Test) for `parse_and_calculate`
- Async ASIO (`async_accept` / `async_read`) to remove the thread pool entirely
- Docker image for easy distribution

---

## Author

**Neel Patil** — Computer Science Student