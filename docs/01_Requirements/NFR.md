# Non-Functional Requirements (NFR)

### 1. Resource Constraints
- **Main CPU (CH32V307) RAM Usage:** 
  - The OS must occupy less than **10 KB of RAM** in shell/idle state.
  - In "Game State", the OS footprint should shrink to less than **2 KB** to maximize resources for the application.
- **Boot Time:** The system must boot to the graphical shell in under **2 seconds**.
- **Power Efficiency:** Designed for battery-operated handheld use; low-power modes must be implemented for idle periods.

### 2. Performance and Latency
- **Input Latency:** Maximum of **8ms** (approx. half a frame at 60fps) from physical input to interrupt handling.
- **Graphics Pipeline:** The command queue between the Main CPU and PGPU must support at least **60 Hz** synchronization.
- **Audio Consistency:** The APU (CH32V003) must maintain a jitter-free audio stream with low-latency updates from the Main CPU.

### 3. Architecture and Transparency
- **User Sovereignty:** Every hardware register and kernel structure must be documented such that a user can rewrite any part of the system.
- **Determinism:** The kernel should be as deterministic as possible to prevent "hiccups" in game performance.
- **Portability:** The HAL must allow the exact same game binary to run on both the physical hardware and the QEMU emulator.

### 4. Stability and Safety
- **Fault Isolation:** A crash in the graphical shell should not crash the kernel; the kernel must be able to reboot the shell or return to a "Safe Mode".
- **Hardware Protection:** The OS must prevent states that could damage the hardware (e.g., incorrect voltage settings or thermal runaway if applicable).
