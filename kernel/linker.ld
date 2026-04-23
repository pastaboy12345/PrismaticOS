ENTRY(_start)

PHDRS
{
    text PT_LOAD FILEHDR PHDRS;
    data PT_LOAD;
    bss PT_LOAD;
}

SECTIONS
{
    . = 1M; /* Kernel starts at 1MB */

    .text : 
    {
        *(.text)
        *(.rodata*)
    } :text

    .data :
    {
        *(.data)
    } :data

    .bss :
    {
        *(.bss)
        *(COMMON)
    } :bss

    . = ALIGN(4096);
    . = . + 0x1000; /* Stack space */
    stack_top = .;
}
