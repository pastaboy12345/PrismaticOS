ENTRY(_start)

SECTIONS
{
    . = 0x100000;  /* Kernel starts at 1MB */

    .text :
    {
        *(.text)
        *(.rodata*)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
        *(COMMON)
    }

    . = ALIGN(4096);
    . = . + 0x1000; /* Stack space */
    stack_top = .;
}
