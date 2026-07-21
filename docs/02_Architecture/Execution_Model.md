>"A console runs one game. Almost everything below is just the consequence of refusing to forget that."

## Privilege Levels
The [boot route](../04_OS_VanadiumOS/Boot.md) hands us the machine already divided. We do not
own the bottom of it, and we do not want to.

| Level | Who | What it may do |
|---|---|---|
| M | OpenSBI | The machine. Timer, IPI, hart start, reset. Not ours. |
| S | **VanadiumOS** | The console. Devices, memory layout, the game's life cycle. |
| U | **The game** | Its own address space's worth of arithmetic, and `ecall`. |

The whole boundary between the OS and a game is one instruction: `ecall`. Nothing else crosses.

>"`ecall` means two different things here and it is worth saying out loud once. A game in U-mode
> calling `ecall` traps to us, and that is a syscall. Us calling `ecall` traps past us to the
> firmware, and that is an SBI call. Same instruction, opposite direction, decided entirely by
> who ran it. This is verified behaviour, not a reading of the spec."

- For the exception codes and how the trap is dispatched, [see also](../04_OS_VanadiumOS/Boot.md).
- For what a game binary looks like on the way in, [see also](../06_Game_layout/Executable_Layout.md).
- For the syscalls themselves, [see also](../05_SDK_HAL/_SDK_Overview.md).

## No Virtual Memory
There is one physical address space and no translation. This is a decision, not a stage we have
not reached yet.

- **We never wanted translation.** What the design asks for is explicit control of the layout and
  direct addressing for DMA. Paging gives neither; it hides both.
- **Determinism.** No page tables means no TLB, and no TLB means no TLB misses. Memory access
  costs what it costs, every frame, forever. A console holding 60 Hz on two screens would rather
  have a predictable cost than a lower average one.
- **More RAM does not need an MMU.** Capacity is a memory die question. Translation is a
  protection and address-space question. They are unrelated, and conflating them is the most
  common way this decision gets made wrong. [See also](Memory_Map.md).

## What We Gave Up
A game can corrupt the OS. There is no hardware standing between them.

This is a real cost and it is accepted on purpose. Physical memory protection lives in M-mode,
which belongs to OpenSBI under the [boot route we chose](../04_OS_VanadiumOS/Boot.md). We could
buy it back by taking M-mode, and that trade is written down where it belongs rather than
pretended away.

For v1 the cost is close to zero, because a console loading one game has nothing to protect it
*from*: the game **is** the system. A wild pointer takes down a machine that exists to run that
game. What matters is not surviving it, but explaining it — which is what
[Safety Modes](../04_OS_VanadiumOS/Safety_Modes.md) is for, and why the fatal trap path names
the cause and timestamps it instead of just halting.

>"Cartridge-era consoles had no memory protection at all and nobody experienced that as a defect.
> The failure mode a player understands is 'it crashed, I turned it off and on'. The failure mode
> nobody forgives is 'it crashed and told me nothing'."

## The Cores
Four U74 harts, [chosen for it](../03_Hardware/SoC_Choice.md). Two would have been enough; four
was upside.

Today hart 0 does everything and harts 1-3 are parked at `wfi` with no way to wake them. That is
correct for now and wrong to leave. Under our boot route we do not start them ourselves with
IPIs — the firmware holds them, and they come up through SBI's hart-start service.

>**OPEN** the core allocation is not decided. My recommendation, to be confirmed:
> **hart 0 for the OS, harts 1-3 for the game.** It matches what the lean-OS memory budget
> already implies, it is what the big consoles do (reserve a core for the system, the rest are
> the title's), and it means the "second processor for the OS" idea costs no second chip.

>**OPEN** whether the SDK exposes multiple cores to a game at all, or whether v1 hands a game a
> single hart and keeps the rest for later. This decides whether the SDK needs synchronisation
> primitives on day one, so it is worth answering before the SDK has users rather than after.

## Scheduling
With one game there is very little to schedule. This is worth stating because the absence is a
design position and not an omission: preemption exists to stop one task starving another, and
until there is more than one task, it is machinery without a job.

What the OS does need soon is not a scheduler but a **clock**, and it already has one: the
architectural counter is readable with no interrupt, no firmware call and no setup, which is why
logs can carry milliseconds-since-boot long before anything can be preempted. The timer
*interrupt* is a separate and later thing.

- For where that goes when it does become a real question, [see also](../04_OS_VanadiumOS/Scheduler.md).

## Loading a Game
>**TODO** this section is the next one to write, and it is currently the gap between "we can run
> U-mode code" and "we can run a game". The steps, cheapest first: run code linked into the
> kernel image itself (works today, needs nothing new); then load a separate binary from a known
> address; then a real filesystem and cartridge path. Nothing above requires the steps below it,
> which is worth exploiting. [See also](../06_Game_layout/Executable_Layout.md).

### Conclusion
>"Every line of this document is us spending complexity we did not have to spend, or refusing to.
> We refused paging, refused preemption, and refused protection, and each refusal has a date on
> which we might change our mind. None of them are today."
