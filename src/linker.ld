OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
  /* 0x80000000 is where the machine-level firmware lives; S-mode payloads
     start at 0x80200000 by convention. This has to match wherever the
     previous stage actually loads us. See docs/04_OS_VanadiumOS/Boot.md */
  RAM (rwx) : ORIGIN = 0x80200000, LENGTH = 128M
}

SECTIONS
{
  .text : {
    . = ALIGN(8);
    PROVIDE(_code_start = .);
    *(.text.init)
    *(.text .text.*)
    . = ALIGN(8);
    PROVIDE(_code_end = .);
  } > RAM

  .rodata : {
    . = ALIGN(8);
    *(.rodata .rodata.*)
  } > RAM

  .data : {
    . = ALIGN(8);
    *(.data .data.*)
  } > RAM

  .bss : {
    . = ALIGN(8);
    PROVIDE(_bss_start = .);
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(8);
    PROVIDE(_bss_end = .);
  } > RAM

  .stack (NOLOAD) : {
    . = ALIGN(16);
    . += 0x1000;
    _stack_top = .;
  } > RAM

  _end = .;
}
