# Changelogs (dates in DD/MM/YYYY format)

## COOLBOOT v0.0.13 (13/12/2022)

- changed the assembly files from nasm syntax to AT&T syntax in order to fit in better with the i686-elf-gcc toolchain

## stage2.bin v0.0.12 (08/12/2022)

- made some huge fixes to the coolboot.sys parser although I think the parser needs a proper re-implementation

## stage2.bin v0.0.11 (08/12/2022)

- if the kernel returns success, then the post-kernel environment will not clear the screen and will simply return to the assembly wrapper to halt the cpu. I plan to make the kernel success code specifiable in coolboot.sys

## boot.bin v0.0.03 (07/12/2022)

- boot.bin will now check the drive parameters instead of trusting the ones already in the bios parameter block

## stage2.bin v0.0.1 (06/12/2022)

- fixed a bug where KERNEL_FILE would have to start with a double slash for files in the root directory.

## stage2.bin v0.0.09 (05/12/2022)

- options must now start with a ':' prefix

## stage2.bin v0.0.08 (04/12/2022)

- coolboot.sys must now have a valid signature to make sure there aren't any formatting issues when the syntax inevitably changes, valid signatures for each version will be placed in the docs directory
- added some functions to parse coolboot.sys
