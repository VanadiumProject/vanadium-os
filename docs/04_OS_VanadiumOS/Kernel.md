# Vanadium Kernel

The Vanadium Kernel is a **Minimalist Real-Time Microkernel** designed to act as the trusted supervisor for the Vanadium Console. It resides on the Main CPU (CH32V307) and manages the hardware abstraction and resource allocation.

## 1. Design Constraints
- **RAM Footprint:** < 10 KB (Shell Mode), < 2 KB (Game Mode).
- **Execution Mode:** Runs in Machine Mode (`m-mode`) on the RISC-V core to maintain full hardware control.
- **Complexity:** Simple, deterministic, and fully auditable by the user.

## 2. Core Responsibilities

### Hardware Abstraction (HAL)
The kernel provides a unified interface for system services, regardless of whether it's running on the physical CH32V307 or a QEMU emulator.
- **UART / Console:** For debugging and system logs.
- **DMA / SPI / I2C:** Managing access to peripherals like the APU and cartridge slot.

### Resource Management
The kernel controls the distribution of the 512 KB SRAM. It ensures that the Shell or a Game cannot accidentally overwrite critical system structures.
- **Physical Memory Allocator (PMM):** A simple bitmap-based allocator for the shared SRAM.

### Execution Control
The kernel manages the transition between the **Graphical Shell** and **Game Cartridges**.
- **Context Switching:** While the kernel is not a multi-tasking OS in the traditional sense, it must be able to securely jump from the Shell to a Game binary and back.
- **Trap Handling:** Manages exceptions and system calls (ECALL) from the user-level shell or game.

## 3. The "Thin Gatekeeper" Philosophy
In the Vanadium OS, the kernel is intentionally "thin." It does not provide high-level abstractions like a complex file system or TCP/IP stack. Instead, it provides the **primitives** (e.g., "Read sector from SD card", "Write to PGPU command queue") that the Shell or Game then uses to build more complex behaviors.

This ensures the user remains in control and the system remains transparent and understandable.

## 4. Interaction with the Shell
The Graphical Shell is technically a **User-Space Application** running with restricted permissions. It requests services from the Kernel via System Calls. This separation ensures that if the Shell crashes (e.g., due to a UI bug), the Kernel can recover and reset the system.
