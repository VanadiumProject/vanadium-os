>"An error code is useless if it doesn't tell you where to look, and a log is useless if you can't read it when everything else is broken."
## Why a Shared Format?
This isn't an OS-only concern. The same error code format is used by the System Firmware
(pre-kernel, running from QSPI) and by VanadiumOS itself once the kernel is up. If the format
changed between boot stages, the debug port would need two parsers instead of one, and a
modder reading logs after a bad RAM mod wouldn't be able to tell which stage failed without
already knowing the answer.
- For more on when each stage is active, [see also](Boot.md).
- For where the log itself physically lives, [see also](../03_Hardware/PCB.md).
## Format
Every error code is 32 bits, split into three fields:
| Field | Bits | Purpose |
|---|---|---|
| Module/Type | 8 | Which subsystem raised it (CPU, MEM, PCI, INPUT...) |
| Instance | 8 | Which *instance* of that module (Controller 0 vs 1, PCIe lane 0 vs 1...) |
| Code | 16 | The specific condition, unique per module |
>"Why not just one big code? Because a controller doesn't care that its twin exists, and the SoC
> can throw more than 255 distinct problems on its own. Splitting instance from code means two
> identical peripherals never fight over the same error space, and a single module never runs out
> of room."
## Severity
Appended as a single-letter suffix in documentation and logs:
- **I** - Information: nothing is wrong, just a note.
- **W** - Warning: still working, but something is off.
- **E** - Error: a subsystem failed, system may continue degraded.
- **F** - Fatal: system cannot continue safely.
- **X** - Special/Unexpected: Easteregg codes.
## Timestamping
Logs use milliseconds since boot, not Unix time. There's no need for a battery-backed RTC read
just to know a controller stopped responding 4 seconds after power-on, and session logs are
expected to be read and cleared per boot anyway.
## Appendix Convention
In the printed/manual appendix, entries are indexed both by code and by module name, so a
reader who already has the code from the debug port doesn't need to hunt through a chapter
list, and a reader browsing the manual by subject still finds it under the module chapter:
>`0x02XX (MEM) 0x0159 - Insufficient Memory found - F`

>Recommended actions (look at the PCB documentation for guidance):
>- Verify memory solder joints;
>- Check memory orientation on the PCB;
>- Check for shorts around the memory bus;
>
>If the problem persists:
>- Pray;
>- E-mail us what you did;
>
>If after all that, it still doesn't work or you didn't find the problem:
>- Send us the console

>`0x02XX (MEM) 0x015A - Over-sufficient Memory found - X`

>Recommended actions:
>- E-mail us how much RAM you added;
## On Easter Eggs
Not every code needs to be reachable through normal failure. Some exist as a reward for people
who go looking:
>`0x03XX (CPU) 0x0F99 - Wrong CPU, how did we get here? - X`

>"how did we get here?" is part of the name, not a comment. If you can legitimately trigger this
> on real hardware, that's the whole point.

>`0x04XX (PCI) 0xAAAA - Detected another Vanadium Console connected, that's new - X`

Device ID `0x0000` is reserved on the PCIe expansion bus specifically so this can't be spoofed by
editing a string in firmware; a verification hash is required to confirm it's actually another
Vanadium Console, not just a device pretending to be one.

### Conclusion

>"If you're reading this appendix by choice and not because something broke, you're exactly who
> it was written for."
