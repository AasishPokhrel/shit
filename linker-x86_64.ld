/* I LIKE POOP */

OUTPUT_FORMAT(elf64-x86-64)

ENTRY(_start)

KERNEL_BASE = 0xffffffff80000000;
KERNEL_PHYS_LOAD_ADDRESS = 0x100000;

PAGE_SIZE = 4K;

PHDRS
{
    phdr_limine_shit          PT_LOAD FLAGS(4);
    phdr_text_shit            PT_LOAD FLAGS(5);
    phdr_rodata_shit          PT_LOAD FLAGS(4);
    phdr_data_shit            PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = KERNEL_BASE;

    .limine_shit ALIGN(PAGE_SIZE) : AT(KERNEL_PHYS_LOAD_ADDRESS) {
        KEEP(*(.limine_requests))
    } :phdr_limine_shit

    . = ALIGN(PAGE_SIZE);

    .text : {
        *(.text .text.*)
    } :phdr_text_shit

    . = ALIGN(PAGE_SIZE);

    .rodata : {
        *(.rodata .rodata.*)
    } :phdr_rodata_shit

    . = ALIGN(PAGE_SIZE);

    .data : {
        *(.data .data.*)
    } :phdr_data_shit

    . = ALIGN(PAGE_SIZE);

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :phdr_data_shit

    . = ALIGN(PAGE_SIZE);
    _kernel_end = .;

    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
        *(.comment)
    }
}