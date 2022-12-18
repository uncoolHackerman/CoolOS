# makefile 23/11/2022
# written by Gabriel Jickells

ASM=/usr/local/i686-elf-gcc/bin/i686-elf-as
EMULATOR=qemu-system-i386
CC32=/usr/local/i686-elf-gcc/bin/i686-elf-gcc
LD32=/usr/local/i686-elf-gcc/bin/i686-elf-gcc

CFLAGS32=-c -O2 -ffreestanding -nostdlib
LDFLAGS32=-nostdlib
CLIBS32=-lgcc

COOLBOOTSRCDIR=src/COOLBOOT/src
SRCDIR=src
BINDIR=bin

# end the branch string with an underscore unless it is empty
VERSION=0.0.14
PLATFORM=x86
DISK=CoolOS_v$(VERSION)_$(PLATFORM).img

default: always bootloader kernel
	dd if=/dev/zero of=$(BINDIR)/$(DISK) bs=512 count=2880
	dd if=$(BINDIR)/boot.bin of=$(BINDIR)/$(DISK) conv=notrunc
	mmd -i $(BINDIR)/$(DISK) user
	mmd -i $(BINDIR)/$(DISK) src
	mmd -i $(BINDIR)/$(DISK) coolos
	mmd -i $(BINDIR)/$(DISK) coolos/system
	mmd -i $(BINDIR)/$(DISK) src/kernel
	mcopy -i $(BINDIR)/$(DISK) notes.txt "::/user/notes.txt"
	mcopy -i $(BINDIR)/$(DISK) $(COOLBOOTSRCDIR)/*.* "::/src/"
	mcopy -i $(BINDIR)/$(DISK) $(BINDIR)/stage2.bin "::/stage2.bin"
	mcopy -i $(BINDIR)/$(DISK) $(COOLBOOTSRCDIR)/stage2/*.* "::/src/"
	mcopy -i $(BINDIR)/$(DISK) $(SRCDIR)/kernel/*.* "::/src/kernel/"
	mcopy -i $(BINDIR)/$(DISK) $(BINDIR)/*.bin "::/CoolOS/system"
	mcopy -i $(BINDIR)/$(DISK) $(SRCDIR)/coolboot.sys "::/coolboot.sys"
	rm $(BINDIR)/*.bin
	rm $(BINDIR)/*.o

bootloader:
	$(ASM) $(COOLBOOTSRCDIR)/boot.s -o $(BINDIR)/boot.o
	$(LD32) $(LDFLAGS32) -T $(COOLBOOTSRCDIR)/boot.ld -Wl,-Map=$(BINDIR)/boot.map $(BINDIR)/boot.o -o $(BINDIR)/boot.bin
	$(ASM) $(COOLBOOTSRCDIR)/stage2/stage2.s -o $(BINDIR)/stage2.o
	$(CC32) $(CFLAGS32) $(COOLBOOTSRCDIR)/stage2/stage2.c -o $(BINDIR)/stage2a.o
	$(LD32) $(LDFLAGS32) -T $(COOLBOOTSRCDIR)/stage2/stage2.ld -Wl,-Map=$(BINDIR)/stage2.map $(BINDIR)/stage2.o $(BINDIR)/stage2a.o -o $(BINDIR)/stage2.bin $(CLIBS32)

kernel:
	$(CC32) $(CFLAGS32) $(SRCDIR)/kernel/kernel.c -o $(BINDIR)/kernel.o
	nasm -f elf -o $(BINDIR)/wrappers.o $(SRCDIR)/kernel/wrappers.asm
	$(LD32) $(LDFLAGS32) -T kernel.ld -Wl,-Map=$(BINDIR)/kernel.map $(BINDIR)/kernel.o $(BINDIR)/wrappers.o -o $(BINDIR)/kernel.bin

always:
	mkdir --parents $(BINDIR)
	touch $(BINDIR)/a.a
	rm $(BINDIR)/*.*

run:
	$(EMULATOR) -m 1 -fda $(BINDIR)/$(DISK)

install_coolboot:
	mkdir -p $(COOLBOOTSRCDIR)
	rm -rf $(COOLBOOTSRCDIR)
	git clone https://github.com/uncoolHackerman/COOLBOOT.git $(COOLBOOTSRCDIR)
	rm -rf $(COOLBOOTSRCDIR)/.git
	#rm $(COOLBOOTSRCDIR)/makefile