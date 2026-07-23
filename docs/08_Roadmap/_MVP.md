>"The MVP is not the small version of the console. It is the smallest thing that is still, without doubt, a console."

## What Counts As MVP
A person powers the device on, a game loads, the game draws to the screen, the person
presses a button and the game responds, and the progress survives being unplugged. Remove
any one of those five and it is no longer a console -- it is a demo.

Everything below is ordered by **unblocking**: each phase exists because the previous one
made it possible, not for the sake of tidiness.

- For the boundary games see, [see also](../05_SDK_HAL/_SDK_Overview.md).
- For the execution model underneath, [see also](../02_Architecture/Execution_Model.md).

## Where We Already Are
This does not start from zero. Already working and verified in the emulator:
- S-mode boot through the manufacturer route, [see also](../04_OS_VanadiumOS/Boot.md).
- A trap path that dispatches U-mode syscalls and names fatal faults.
- `kprintf`, and time-since-boot from the architectural counter.
- The kernel's freestanding memory routines (`kstring`), which the filesystem will reuse.

The MVP is the distance between that and "you can play it".

## Phase 1 - Run A Game
The milestone where picolibc becomes testable and a U-mode program really runs.
- `WRITE` moves from a single character to `(fd, buf, len)`.
- `EXIT` is implemented (only declared today).
- `SBRK` and a heap region, so `malloc` can exist.
- U-mode stack and heap reserved in the memory map, [see also](../02_Architecture/Memory_Map.md).
- Cheapest loader first: a game linked into the image, entered by `sret`. Then ELF.

>"At the end of this phase a 'game' prints, allocates and exits. No screen, no filesystem,
> no cartridge. It does not look like a console yet, but it is the first time third-party
> code runs on our floor."

## Phase 2 - See And Control
This is where it becomes a real console, because it is the first thing a player recognizes.
- A video driver that fills a framebuffer. It is the project's biggest gap and has no design
  yet -- the document comes before the code.
- Real input, beyond the placeholder that returns a test variable today.

>**TODO** video and input have no design document yet. Write the design before opening the
> code, or we end up hostage to decisions made in a hurry.

## Phase 3 - Don't Lose The Progress
On a console the player pulls the plug. That is normal use, not an edge case.
- A block device (virtio-blk in the emulator) and FatFs on top, [see also](../04_OS_VanadiumOS/FS_Saves.md).
- Saves in two alternating slots with a sequence number and a checksum. Filesystem journaling
  does not save a half-written save; atomicity has to live in the application.
- FAT32 for the one property nothing else gives: the player reads their own saves on a PC.

## What The MVP Deliberately Does NOT Have
Stated so no one mistakes absence for oversight:
- No audio. A mute console is still playable; a blind one is not.
- No cartridge loading -- it loads from whatever the emulator offers.
- No multiple games, no menu, no shell. It loads one, runs one.
- No memory protection. The game *is* the system; a stray pointer takes down a machine that
  exists to run that game, and [Safety Modes](../04_OS_VanadiumOS/Safety_Modes.md) explains
  the crash rather than preventing it.

### Conclusion
>"If Phase 3 closes and a game loads, draws, responds and saves, the rest of the roadmap is
> polish. This line right here is the border between a project and a product."
