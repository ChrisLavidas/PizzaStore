# Pizza Store 

University assignment for the **Operating Systems** course, **4th semester**, at the
**Athens University of Economics and Business (AUEB)**.

A multithreaded simulation of a pizza store written in **C** using **POSIX Threads (pthreads)**.
Each customer is a separate thread, and orders are processed concurrently while shared data is
protected with **mutexes** and **condition variables**.

A simple, frontend-only web visualizer (`index.html`) is also included so you can watch the
simulation in your browser, without changing the C backend.

![Pizza Store Visual Simulator](screenshot.png)

## How it works

Each customer thread (`customer_routine`) goes through several stations in order — phone operator,
cook, oven, packer, deliverer — each with a limited number of resources defined in `pizza.h`.
When a resource is busy the thread waits on a condition variable (`pthread_cond_wait`) until one is
freed and signaled (`pthread_cond_signal`). Every access to a shared variable is protected with
`pthread_mutex_lock` / `pthread_mutex_unlock` to avoid race conditions.

Each order contains a random number of pizzas (Margarita, Pepperoni or Special) with fixed
probabilities and prices, and there is a small chance the payment fails and the order is cancelled.
At the end the program prints the total revenue, pizzas sold per type, successful and cancelled
orders, and the average/maximum service and cooling times.

## Files

- `pizza.c` — the main program (threads, mutexes, condition variables, logic)
- `pizza.h` — configuration constants (employees, prices, probabilities, timings)
- `index.html` — frontend-only web visualizer
- `test-res.sh` — helper script for running tests

## Building and running

You need a C compiler that supports pthreads (MSYS2/MinGW, Cygwin or WSL on Windows;
a Windows-compatible `rand_r` is already included).

The program takes two arguments — the number of customers and a random seed. Both are
required; running it without them crashes immediately, since it reads them from the
command line.

### Windows (Command Prompt)

Compile to `pizza.exe`, then run it directly — note there is no `./` prefix in `cmd`:

```cmd
gcc pizza.c -o pizza.exe -lpthread -lm
pizza.exe 15 42
```

### Linux / WSL / macOS

```bash
gcc pizza.c -o pizza -lpthread -lm
./pizza 15 42
```

## Web visualizer

Just double-click `index.html` to open it in a browser, or serve it locally:

```bash
python -m http.server 8000
# then open http://localhost:8000/index.html
```

Set the number of customers, the seed and the speed, then press Start to watch orders flow through
the stations while the live statistics update. The visualizer is for illustration only — the actual
implementation is the C backend in `pizza.c`.

