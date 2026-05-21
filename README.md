# tinyOS - Lightweight Embedded Operating System for ATmega

A minimal, bare-metal multitasking operating system designed from scratch for resource-constrained ATmega microcontroller. This project demonstrates low-level embedded software engineering, deterministic task handling, and custom scheduling mechanisms without relying on heavy third-party RTOS libraries.

## 🚀 Features

- **Bare-Metal Architecture:** Written in C++ to minimize Flash and RAM overhead.
- **Low-Level Context Switching:** Custom interrupt-driven task switching including manual stack pointer manipulation and registry saving.
- **Task Isolation:** Dedicated stack allocation per task to prevent memory corruption in a multithreaded environment.
- **Deterministic Control:** Direct hardware register manipulation for timers, interrupts, and I/O pins.

## 🏔️ Roadmap
- **Round-Robin Scheduler:** Implementing a time-sliced Round-Robin scheduling algorithm for concurrent task execution.

## 🛠️ Tech Stack & Tools

- **Language:** C++
- **Toolchain:** AVR-GCC, GNU Make / CMake
- **Hardware Target:** ATmega Series

## 🏗️ Architecture Overview

The core of `tinyOS` revolves around strict resource management. Since ATmega microcontrollers have extremely limited SRAM, the kernel bypasses dynamic allocation and uses a lightweight, static task control block (TCB) structure.

1. **Initialization:** Disables interrupts, configures system clocks, and initializes the hardware timer.
2. **Stack Preparation:** Manually crafts the initial stack frame for each registered task (injecting the function pointer and resetting registers).
3. **Interrupt Handling:** Utilizes a hardware timer ISR (Interrupt Service Routine) to trigger the context switch.
4. **Context Switch:** Saves the current CPU state (R0-R31, SREG), switches the Stack Pointer (SP) to the next task, and restores the new CPU state.

## ⚡ Quick Start

### Prerequisites
You need the AVR toolchain installed on your system:
```bash
sudo apt-get install gcc-avr avr-libc avrdude
```

### Building the Project
To compile the OS and generate the `.hex` file for flashing:
```bash
make all
```
