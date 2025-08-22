CROSS=i686-elf
CC=$(CROSS)-gcc
AS=$(CROSS)-as
LD=$(CROSS)-gcc
QEMU=qemu-system-i386

ISO_DIR=isodir
ISO_BOOT=$(ISO_DIR)/boot
ISO_GRUB=$(ISO_DIR)/boot/grub
ISO_IMAGE=$(ISO_DIR)/myos.iso

BUILD_DIR=build
C_SRC=kernel.c
C_OBJS=$(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRC))
AS_SRC=boot.s
AS_OBJS=$(patsubst %.s, $(BUILD_DIR)/%.o, $(AS_SRC))
OBJS=$(C_OBJS) $(AS_OBJS)
LINKER_LD=linker.ld
KERNEL_BIN=$(BUILD_DIR)/myos.bin

CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS=-T $(LINKER_LD) -ffreestanding -O2 -nostdlib -lgcc

all: $(KERNEL_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR):
	mkdir -p $(ISO_BOOT) $(ISO_GRUB)

$(AS_OBJS): $(AS_SRC) | $(BUILD_DIR)
	$(AS) $< -o $@

$(C_OBJS): $(C_SRC) | $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(KERNEL_BIN): $(OBJS) $(LINKER_LD)
	$(LD) $(LDFLAGS) $(OBJS) -o $(KERNEL_BIN)

prepare-iso: $(KERNEL_BIN) | $(ISO_DIR)
	cp $(KERNEL_BIN) $(ISO_BOOT)/myos.bin
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
