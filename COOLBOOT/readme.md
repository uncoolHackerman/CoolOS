# Changelogs (dates in DD/MM/YYYY format)

## boot.bin v0.0.03 (07/12/2022)

- boot.bin will now check the drive parameters instead of trusting the ones already in the bios parameter block

## stage2.bin v0.0.1 (06/12/2022)

- fixed a bug where KERNEL_FILE would have to start with a double slash for files in the root directory.

## stage2.bin v0.0.09 (05/12/2022)

- options must now start with a ':' prefix

## stage2.bin v0.0.08 (04/12/2022)

- coolboot.sys must now have a valid signature to make sure there aren't any formatting issues when the syntax inevitably changes, valid signatures for each version will be placed in the docs directory
- added some functions to parse coolboot.sys
