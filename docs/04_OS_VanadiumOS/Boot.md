>"Owning every stage sounds like sovereignty until you realize you've spent a year writing memory training code and still haven't drawn a triangle."

## The Route
VanadiumOS boots through the route the SoC manufacturer documents, and hands control to the
kernel in **S-mode**. Nothing here is exotic; that is the point.

| Stage | Privilege | Owned by | Purpose |
|---|---|---|---|
| BootROM | M | SoC (mask ROM) | Brings up the chip, finds the next stage |
| SPL | M | Vendor | DRAM init and memory training |
| OpenSBI | M | Upstream (BSD) | Machine-level runtime: timer, IPI, hart start, console |
| U-Boot | S | Upstream (GPL) | Loads VanadiumOS, and our development loop |
| **VanadiumOS** | **S** | **Us** | Everything the user will ever see |

- For what each stage is allowed to log and in what format, [see also](Safety_Modes.md).
- For the chip this route belongs to, [see also](../03_Hardware/SoC_Choice.md).
- For what we do with the other three cores once we are up, [see also](../02_Architecture/Execution_Model.md).

## Why This One
- **It boots.** This is not a small feature. When the documented route fails, the failure is
  documented too, and somebody else has already had it.
- **SBI hands us working machinery.** Timer, inter-processor interrupts, starting the other
  harts, system reset. All of it is code we would otherwise write, debug, and then maintain
  forever, to arrive at exactly where we started.
- **The device tree is a real gain for our design.** Our HAL currently picks between emulator
  and hardware at compile time. Device tree discovery turns that into a runtime decision, which
  means one binary that runs in QEMU and on the board. We were going to want this anyway.
- **The development loop.** U-Boot lets us load over network or USB. Reflashing the console for
  every single iteration is the kind of friction that quietly kills hobby projects.
- **It is harder to brick.** A console you cannot recover is a console you stop working on.

## What Was Discarded
### Owning M-mode ourselves, keeping the vendor SPL
Replace OpenSBI, keep the vendor's DRAM init, run our own kernel at machine level. This was
genuinely tempting: it preserves the M-mode trap code we already wrote, gives us the CLINT
directly with no SBI call in the way, and unlocks **PMP** for memory protection without paging.

Discarded for now, not on merit, but on ordering. See *Not a No* below.

### Owning everything from reset, DRAM training included
Maximum sovereignty, and the honest answer is that memory training on a modern DDR3 part is a
large, brittle, deeply unglamorous project that is normally a vendor blob for good reason. The
console would gain nothing a player could see. This one is a plain no.

## Not a No
Option B is a **Not-Now**, not a rejection. There can be a fully ours boot chain one day, and
the reason it isn't today is that starting it now delays literally everything else: no timer, no
scheduler, no graphics, no games, while we reimplement machinery that already exists and works.

The stages we adopt are open source. We take a stable version, we keep their LICENSE, we
iterate on top of it, and if the day comes that we want our own, we fork from something that
already boots instead of from nothing. This is what the PS4 did with FreeBSD 9, and nobody
would describe that console as having been held back by it.

>"Adopting something open is not the same as depending on something opaque. We can read every
> line of OpenSBI today, and that is the whole difference. The genuinely closed parts of this
> platform are DRAM training and the GPU, and those cost the same under every option we
> considered."

For what we still owe the user in transparency terms, [see also](../00_Vision_Philosophy/_Manifest.md).

## The Price We Agreed To Pay
Choosing this route is choosing these consequences, on purpose:
- **M-mode is not ours, so PMP is not ours.** OpenSBI uses it to protect itself. If we later
  want memory protection, the answer becomes the MMU, not PMP. Acceptable, because v1 loads one
  game and that game *is* the system; a bad game crashes and the console resets, which is
  already how [Safety Modes](Safety_Modes.md) is designed to behave.
- **The timer goes through SBI**, not through direct CLINT programming.
- **U-Boot will not take a bare ELF.** We owe it a proper image, and that is real work.
- **We inherit an upstream's release cadence** for three stages of our own boot.

## State of the Code
The kernel currently in the repository is **M-mode**, and always was: `boot.S` was written for
the original RV32 iteration, and the 32-to-64 bit migration changed the width without ever
revisiting the privilege level. It is a leftover, not a decision, and this document is where
that gets corrected.

The port is smaller than it looks. The trap frame layout, the register save/restore, the
`mcause` dispatch logic, the HAL, the syscall layer and the linker script all survive unchanged.
What moves:
- CSRs `mtvec`/`mscratch`/`mepc`/`mstatus`/`mcause`/`mtval` become their `s*` equivalents, and
  `mret` becomes `sret`.
- `mhartid` stops being readable. The hart ID arrives in `a0` and the device tree pointer in
  `a1`, and we must not clobber either before saving them.
- The ecall exception code we dispatch on moves from 11 (from M-mode) to **8, from U-mode**.
  Not 9. An ecall from S-mode is not delegatable and always traps to the firmware, so once we
  are the supervisor, `ecall` means two different things depending on who runs it: from a game
  in U-mode it is a syscall and it reaches us; from our own kernel it is an SBI call and it
  reaches OpenSBI. Verified in the emulator, not assumed.
- Our load address must agree with wherever U-Boot puts us. [See also](../02_Architecture/Memory_Map.md).

>**TODO** confirm the DRAM base address and the U-Boot load address against the manufacturer
> documentation; the linker script currently carries the emulator's values.

The whole privilege-mode port can be rehearsed in the emulator before any of the image format
or load address work starts, by letting the emulator supply OpenSBI instead of booting bare.
Two hard problems, taken one at a time.

### Conclusion
>"We did not pick the humble option because we lack ambition. We picked it because the ambition
> is a console people play, not a boot chain people admire."
