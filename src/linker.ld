OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
  RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
  .text : {
    PROVIDE(_code_start = .);
    *(.text.init)
    *(.text .text.*)
    PROVIDE(_code_end = .);
  } > RAM

  .rodata : {
    *(.rodata .rodata.*)
  } > RAM

  .data : {
    *(.data .data.*)
  } > RAM

  .bss : {
    PROVIDE(_bss_start = .);
    *(.bss .bss.*)
    *(COMMON)
    PROVIDE(_bss_end = .);
  } > RAM

  .stack (NOLOAD) : {
    . = ALIGN(16);
    . += 0x1000;
    _stack_top = .;
  } > RAM

  _end = .;
}
