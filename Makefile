CROSS=i686-elf
CC=$(CROSS)-gcc
AS=$(CROSS)-as
LD=$(CROSS)-gcc
QEMU=qemu-system-i386

ISO_DIR=isodir
ISO_BOOT=$(ISO_DIR)/boot
ISO_GRUB=$(ISO_BOOT)/grub
ISO_IMAGE=$(ISO_DIR)/krazsh.iso

SRC_DIR=src
BUILD_DIR=build
C_SRC=kernel.c terminal.c keyboard.c std.c
AS_SRC=boot.s
OBJS=$(addprefix $(BUILD_DIR)/, $(C_SRC:.c=.o) $(AS_SRC:.s=.o))

LINKER_LD=linker.ld
KERNEL_FILENAME=krazsh.bin
KERNEL_BIN=$(BUILD_DIR)/$(KERNEL_FILENAME)

OO=-O2
CFLAGS=-std=gnu99 -ffreestanding $(OO) -Wall -Wextra
LDFLAGS=-T $(LINKER_LD) -ffreestanding $(OO) -nostdlib -lgcc

all: $(KERNEL_BIN)

$(KERNEL_BIN): $(OBJS) $(LINKER_LD)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s | $(BUILD_DIR)
	$(AS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR):
	mkdir -p $(ISO_BOOT) $(ISO_GRUB)

prepare-iso: $(KERNEL_BIN) | $(ISO_DIR)
	cp $(KERNEL_BIN) $(ISO_BOOT)/$(KERNEL_FILENAME)
	cp grub.cfg $(ISO_GRUB)/grub.cfg

iso: prepare-iso
	grub-mkrescue -o $(ISO_IMAGE) $(ISO_DIR)

run: $(KERNEL_BIN)
	$(QEMU) -kernel $(KERNEL_BIN)

run-iso: $(ISO_IMAGE)
	$(QEMU) -cdrom $(ISO_IMAGE)

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)

clean-build:
	rm -rf $(BUILD_DIR)

clean-iso:
	rm -rf $(ISO_DIR)

.PHONY: all prepare-iso iso run run-iso clean clean-build clean-iso
