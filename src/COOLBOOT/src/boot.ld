OUTPUT_FORMAT("binary")
phys = 0x00007C00;

SECTIONS
{
    . = phys;
    .text : {__text_start = .; *(.text)}
}