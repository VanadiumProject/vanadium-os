>"The OS is what we ship to a player. The SDK is what we ship to whoever wants to become one."

## What the SDK Is
A separate project from VanadiumOS, with its own repository. The OS repository holds a kernel and
nothing else; the SDK holds the C library, the console API, the startup code and the build rules
a game is compiled against.

They meet at exactly one place: **the syscall ABI**. Everything else on either side is free to
change without asking the other.

>"A naming warning, because this folder invites the mistake. The HAL inside the OS repository is
> the *kernel's* way of not caring whether it is running on the emulator or the board. It is
> internal, and a game never sees it. What a game sees is the syscall ABI and the SDK on top of
> it. Two different abstractions that happen to share a word."

- For the privilege boundary this all sits on, [see also](../02_Architecture/Execution_Model.md).
- For what a game binary looks like, [see also](../06_Game_layout/Executable_Layout.md).

## The Boundary
The kernel promises a game exactly this much, from its first instruction:

| Promise | Value |
|---|---|
| Privilege | U-mode |
| `sp` | Top of the game's stack, already set by the kernel |
| `a0`, `a1` | Zero. Reserved, do not read meaning into them |
| Descriptors | 0, 1 and 2 are already valid. There is no `open()` |
| `argc`/`argv`/`envp` | None. A console has no command line |
| Entry | The binary's entry point, entered by `sret` |

Where the descriptors physically go is the OS's business and will change. Today all three are the
debug UART, because it can be read with no shell, no screen and no driver. When there is a video
console or a Safety Modes log, they get pointed at those and **no game is recompiled**. The
number is the contract; the destination is not.

## The Syscalls
| Number | Name | Signature | Status |
|---|---|---|---|
| `0x00` | WRITE | `(fd, buf, len)` → count | Works |
| `0x01` | READ | `(fd, buf, len)` → count | Works, blocking, stops at newline |
| `0x03` | EXIT | `(code)` → never returns | Works |
| `0x30` | SBRK | `(increment)` → previous break | Works |
| `0x10` | GET_INPUT | `()` → button bitmap | Placeholder, reads a test variable |
| `0x20` | PGPU_CMD | — | Reserved, nothing behind it |
| `0x04` `0x05` `0x06` | CLOSE, FSTAT, LSEEK | — | Reserved until a filesystem exists |

## The Retarget Layer
A C library does not know about our syscalls; it knows about a handful of hooks it expects
somebody to provide. That somebody is the SDK, not the kernel.

Four of them are real and call into the OS: **write, read, exit, sbrk**.

The rest are answered locally, without a trap, because with a fixed descriptor set the answers
are compile-time constants:

- **`close`** — a no-op returning success. There is nothing to release.
- **`isatty`** — always true. All three descriptors are the console.
- **`lseek`** — always fails. A character device has no position to seek to.
- **`fstat`** — reports a character device, which is the only thing the library actually wants
  from it: enough to decide it should not buffer output that a human is waiting to read.

>"It would have been easy to make those four syscalls too, and it would have been wrong. `fstat`
> in the kernel means fixing a `struct stat` layout today, with no filesystem to inform what
> belongs in it. Reserving the numbers costs nothing; inventing the structure costs us the right
> to design it properly later."

## The Library Itself
**picolibc**, decided. RISC-V is a first class target rather than a port, the retarget surface is
small enough to write in an afternoon, and the licensing is one file instead of a mosaic spread
across hundreds — which matters for a project that promises to document everything it ships.

newlib was the real alternative and lost narrowly. Its arguments were good ones: it arrives free
inside any prebuilt `riscv64-unknown-elf` GCC toolchain, third party C code is usually tested
against it, and it is what devkitPro uses so its failure modes are already familiar here.

>"Size did not decide this, and it is worth saying so. A C library is tens of kilobytes against a
> game's twelve megabytes. That is noise. Anybody repeating the size argument for a console this
> size is arguing from a microcontroller's habits."

Whichever wins, it is **statically linked into each game** and is not shipped in the OS
filesystem. A `.a` is a link-time artifact and cannot be shared at runtime anyway; sharing would
need `.so` files and a dynamic loader. More importantly, a library living in the OS is a route by
which an OS update changes the behaviour of a game that shipped years earlier, and that is
forced obsolescence arriving through the back door of a project that
[promised the opposite](../00_Vision_Philosophy/_Manifest.md).

Practical note for building it: cross-compile once on Linux, keep the `.a` files and headers, and
record **which version and which flags produced them** next to the binaries. Same target and ABI
as the kernel — `rv64gc`, `lp64d` — or the link fails. Bring `libgcc.a` along in case the library
reaches for a helper the hardware does not do natively.

## What Realistically Remains
Honest ordering, and the good news is that the first group is not blocked by anything:

**Buildable today, the kernel side is done**
1. Choose the C library and cross-build it.
2. Write the retarget layer: four calls, four constants.
3. Write `crt0` — the `_start` that matches the contract above and calls `main`.
4. A linker script for game binaries, and a memory map that says where a game may live.
5. A build rule that turns a `.c` into something the console will accept.

At the end of that list you can have a game that prints, allocates and exits. That is a real
milestone and it needs no filesystem, no loader and no cartridge: a program linked into the
kernel image and entered with `sret` already works today.

**Blocked on the OS, not on the SDK**
6. Loading a game that is not part of the kernel image. [See also](../06_Game_layout/Executable_Layout.md).
7. Input, beyond the placeholder. [See also](Input.md).
8. Graphics, which is the largest single thing missing and has no driver behind it at all.
9. Audio, storage and saves.
10. Cartridge and game identity. [See also](../06_Game_layout/Cartridge.md).

>**TODO** items 7 through 10 have no design documents yet, and writing them before the drivers
> exist would be inventing constraints. They are listed here so the gap is visible, not so it
> looks planned.

### Conclusion
>"The SDK's job is to make the console boring to write for. Every clever thing in here is a
> failure we have not noticed yet."
