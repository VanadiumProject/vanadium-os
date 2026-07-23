>"Everything here is a decision we already made and deliberately deferred. This file is where the 'not now' answers wait, so nobody mistakes them for 'no'."

## The Rule For This File
Nothing goes here that is merely a nice idea. Every entry is something we hit during design,
weighed, and pushed past 1.0 on purpose -- with a reason on record and a date on which we
might change our mind. If it is not a deferred decision, it does not belong here.

- For the constraints these were weighed against, [see also](../00_Vision_Philosophy/_Manifest.md).

## A Boot Chain That Is Entirely Ours
1.0 boots through the manufacturer route and runs in S-mode under OpenSBI, which owns
M-mode. [Boot](../04_OS_VanadiumOS/Boot.md) records the alternative -- taking M-mode
ourselves -- as a **Not-Now, not a No**. The adopted stages are open source; the plan, if the
day comes, is to fork a stable version rather than start from nothing, the way the PS4 built
on FreeBSD 9.

>"Owning M-mode buys real things: the CLINT directly, and PMP for memory protection without
> paging. It also costs everything else, because doing it during bring-up delays the timer,
> the scheduler, the graphics and the games. So it waits, and it waits from a position of
> strength -- a console that already boots."

## Memory Protection Between OS And Game
The unlock the item above enables. With M-mode ours, **PMP** puts region-based protection
between the kernel and a U-mode game with no page tables and no TLB jitter. 1.0 does without
it because it loads one game and that game *is* the system. This matters the moment more than
one thing shares the machine -- see below. [See also](../02_Architecture/Execution_Model.md).

## More Than One Game At A Time
1.0 loads one game and runs it. Menus, a resident system UI, switching titles without a power
cycle -- all of it needs the machine to hold more than one thing safely, which is why it is
downstream of both preemption (1.0) and protection (above). The core-allocation groundwork
from 1.0 is the first step; this is where it pays off.

## The Expansion Connector
The flagship idea the hardware was chosen to allow, [see also](../03_Hardware/Expansion_Connector.md):
PCIe over M.2 Key E, with the second screen as the first-class accessory. Deferred because an
expansion bus with nothing proven to expand is speculation, and because the OS has to identify
and trust a device before it can safely hand it to a game.

## Homebrew Firmware On Accessories
The far end of the sovereignty promise: a user builds firmware for their own accessory, and
the OS recognizes it and lets them use it. A dedicated keyboard, or -- if someone finds a
reason -- stranger things. This is a platform commitment, not a console feature, and it only
makes sense once the expansion path above is real.

## The SDK Grows Up
Tracked here because it lives in [its own repository](../05_SDK_HAL/_SDK_Overview.md) and moves
on its own clock:
- More of the C library surface, as real games ask for it.
- A console API above libc -- the equivalent of what libctru is to newlib on the 3DS -- so a
  game is not talking in raw syscalls.
- Multiple cores exposed to a game, if 1.0's allocation decision left that door open, with the
  synchronization primitives that implies.

## The Dream That Sets The Ceiling
Not a plan, a direction: the point at which someone builds something on Vanadium that surprises
us, because the platform got out of the way. Every deferred item above is chosen so that day
gets closer, not just so 1.0 ships.

### Conclusion
>"A roadmap that ends at 1.0 describes a product. A roadmap that keeps going describes a
> platform. We are writing the second kind on purpose, and this file is the proof we meant it."
