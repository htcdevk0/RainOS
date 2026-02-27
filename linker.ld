ENTRY(_start)

SECTIONS
{
  . = 1M;

  .text : {
    *(.multiboot2)
    *(.text*)
  }

  .rodata : { *(.rodata*) }
  .data   : { *(.data*) }

  .bss : {
    *(COMMON)
    *(.bss*)
  }

  _end = .;
}