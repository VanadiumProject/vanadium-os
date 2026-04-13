# Vanadium Memory Map

The system uses **512 KB of shared SRAM** distributed across the three main units (Main CPU, PGPU, APU).

## SRAM Distribution (Draft)

| Memory Range (Offset) | Size | Purpose | Ownership/Access |
| :--- | :--- | :--- | :--- |
| `0x00000` - `0x01FFF` | 8 KB | **Kernel Core RAM** | Main CPU (RW) |
| `0x02000` - `0x027FF` | 2 KB | **Shared Control Block** | All CPUs (RW) |
| `0x02800` - `0x047FF` | 8 KB | **PGPU Command Queue** | Main (W) / PGPU (R) |
| `0x04800` - `0x057FF` | 4 KB | **APU Audio Buffer** | Main (W) / APU (R) |
| `0x05800` - `0x1FFFF` | 106 KB | **System/Shell Stack & Heap** | Main CPU (RW) |
| `0x20000` - `0x7FFFF` | 384 KB | **Game/App Memory / Frame Buffer**| Shared (RW) |

### Memory Regions Detail

#### 1. Kernel Core RAM (8 KB)
Used for the OS kernel's data, stack, and interrupt vectors. This must remain < 10 KB to meet NFR requirements.

#### 2. Shared Control Block (2 KB)
A synchronization area where state flags (e.g., `SYSTEM_READY`, `VBLANK_TRIGGER`, `APU_READY`) are stored.

#### 3. PGPU Command Queue (8 KB)
A ring buffer where the Main CPU writes drawing commands (e.g., `DRAW_SPRITE`, `CLEAR_SCREEN`, `UPDATE_PALETTE`) for the PGPU to execute.

#### 4. APU Audio Buffer (4 KB)
Used for passing procedural audio parameters or PCM streams from the Main CPU to the APU.

#### 5. Game/App Memory (384 KB)
The largest block, used by the loaded cartridge/application. Can be reconfigured as a frame buffer for direct pixel access or as raw asset RAM.

---
*Note: Specific physical addresses (e.g., `0x20000000` for internal RAM) depend on the hardware target and will be mapped here once finalized.*
