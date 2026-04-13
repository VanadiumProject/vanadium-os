# Vanadium Executable Layout (.vdm)

To keep the kernel simple and stay within the **10 KB RAM footprint**, Vanadium OS uses a custom, lightweight executable format instead of complex ELF binaries.

## 1. File Header (32 bytes)
The header provides the kernel with the essential information to load and execute the application.

| Offset | Size | Name | Description |
| :--- | :--- | :--- | :--- |
| `0x00` | 4 | `Magic` | Must be `0x56 0x41 0x4E 0x31` ("VAN1") |
| `0x04` | 4 | `Entry` | Entry point address (relative to start of code) |
| `0x08` | 4 | `CodeSize` | Size of the code section in bytes |
| `0x0C` | 4 | `DataSize` | Size of the initialized data section |
| `0x10` | 4 | `BssSize` | Size of the zero-initialized data section |
| `0x14` | 4 | `StackSize`| Minimum stack size required by the application |
| `0x18` | 8 | `Reserved` | Reserved for future use (must be 0) |

## 2. Memory Sections
The kernel loads the executable into the **Game/App Memory** (`0x20000` offset in SRAM):
1.  **Code (.text):** Read-only instructions.
2.  **Data (.data):** Initialized global/static variables.
3.  **BSS (.bss):** The kernel zero-fills this area after loading the binary.
4.  **Stack:** The kernel sets the Stack Pointer (`sp`) to the end of the allocated memory block.

## 3. The "System Call" Entry
Applications (the Shell or Games) communicate with the kernel using the RISC-V `ecall` instruction.
- **Register `a7`:** System call number.
- **Registers `a0-a3`:** Arguments.
- **Register `a0`:** Return value.

### Preliminary Syscall List
| ID | Name | Description |
| :--- | :--- | :--- |
| `0x01` | `SYS_EXIT` | Returns control to the kernel/shell. |
| `0x10` | `SYS_GET_INPUT` | Returns the current state of the controller/buttons. |
| `0x20` | `SYS_PGPU_SUBMIT` | Submits a command block to the PGPU queue. |
| `0x30` | `SYS_APU_SUBMIT` | Submits an audio packet to the APU queue. |
| `0x40` | `SYS_MALLOC` | Requests additional memory from the PMM. |

## 4. Entry Point Expectations
When the kernel jumps to the application's `Entry` point:
- `ra` (return address) is set to a kernel cleanup routine.
- `sp` (stack pointer) is correctly initialized.
- `gp` (global pointer) is set to the application's data section if needed.
- The CPU is in **User Mode (`u-mode`)** to protect the kernel's integrity.
