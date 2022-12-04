ENTRY(main)
OUTPUT_FORMAT("binary")
phys = 0x00020000;

SECTIONS
{
    . = phys;
    .entry : {__entry_start = .; *(.entry)}
    .text : {__text_start = .; *(.text)}
    .data : {__data_start = .; *(.data)}
    .rodata : {__data_start = .; *(.rodata)}
    .bss : {__bss_start = .; *(.bss)}
    __end = .;
}