# ============================================================
# RainOS Build System
# Multiboot2 + GRUB (BIOS + UEFI) + QEMU
#
# Requirements (Ubuntu/Debian):
#   sudo apt install build-essential nasm grub-pc-bin grub-common \
#        mtools xorriso qemu-system ovmf
#
# Build ISO:
#   make
#
# Run (BIOS):
#   make run
#
# Run (UEFI firmware):
#   make run-uefi
#
# Run (UEFI + KVM acceleration):
#   make run-cpu
#
# Clean:
#   make clean
# ============================================================

PROJECT      := rainos
BUILD_DIR    := build
ISO_DIR      := $(BUILD_DIR)/isodir
KERNEL_BIN   := $(BUILD_DIR)/$(PROJECT).bin
ISO_FILE     := $(BUILD_DIR)/$(PROJECT).iso

CC           := gcc
LD           := ld
AS           := nasm

GRUBMKRESCUE := grub2-mkrescue
ifeq (, $(shell which grub2-mkrescue 2>/dev/null))
GRUBMKRESCUE := grub-mkrescue
endif

# ---------------- Compiler / Linker ----------------

CFLAGS := -m64 -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
          -nostdlib -Wall -Wextra -Werror -O2 \
          -I./src -I./src/kernel -I./src/fb -I./src/drivers

LDFLAGS := -m elf_x86_64 -T linker64.ld -nostdlib

# ---------------- Sources ----------------

C_SOURCES := \
    src/kernel/kmain.c \
    src/kernel/idt.c \
    src/kernel/pic.c \
    src/fb/framebuffer.c \
    src/drivers/keyboard.c \
    src/drivers/mouse.c \
    src/drivers/rtc.c \
    src/fb/font.c \
    src/modules/window.c \
    src/modules/welcome_app.c \
    src/icons/generic_app.c \
    src/icons/desktop.c \
    src/desktop/home.c \
	src/modules/patches_app.c \
	src/modules/showcase_app.c \
	src/modules/settings_app.c

ASM_SOURCES := \
    src/boot/boot64.asm \
    src/kernel/isr.asm \
    src/kernel/idt_flush.asm

C_OBJECTS   := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst %.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
OBJECTS     := $(C_OBJECTS) $(ASM_OBJECTS)

EFI_CODE    ?= /usr/share/OVMF/OVMF_CODE_4M.fd
EFI_VARS    ?= /usr/share/OVMF/OVMF_VARS_4M.fd

.PHONY: all clean iso run run-uefi run-cpu dirs

all: iso

# ---------------- Directories ----------------

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/src/boot
	mkdir -p $(BUILD_DIR)/src/kernel
	mkdir -p $(BUILD_DIR)/src/fb
	mkdir -p $(BUILD_DIR)/src/drivers
	mkdir -p $(BUILD_DIR)/src/modules
	mkdir -p $(BUILD_DIR)/src/icons
	mkdir -p $(BUILD_DIR)/src/desktop
	mkdir -p $(ISO_DIR)/boot/grub

# ---------------- Compilation ----------------

$(BUILD_DIR)/%.o: %.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.asm | dirs
	$(AS) -f elf64 $< -o $@

$(KERNEL_BIN): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

# ---------------- ISO Generation ----------------

iso: clean $(KERNEL_BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/$(PROJECT).bin
	cp src/boot/grub/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg

	mkdir -p $(ISO_DIR)/boot/grub/themes/rain
	cp -r src/boot/grub/themes/rain/* $(ISO_DIR)/boot/grub/themes/rain/

	$(GRUBMKRESCUE) -o $(ISO_FILE) $(ISO_DIR)

# ---------------- QEMU Targets ----------------

# WARNING! ALL EXCEPT 'run-cpu' IS DEPRECATED AND DOESN'T RUN AT ACTUAL KERNEL!
run: iso
	qemu-system-i386 \
		-cdrom $(ISO_FILE) \
		-m 4G -cpu max -smp 2 \
		-vga std \
		-display sdl,gl=on \
		-name "RainOS BIOS"

run-uefi: iso
	qemu-system-x86_64 \
		-drive if=pflash,format=raw,readonly=on,file=$(EFI_CODE) \
		-drive if=pflash,format=raw,file=$(EFI_VARS) \
		-cdrom $(ISO_FILE) \
		-m 4G -cpu max -smp 2 \
		-display sdl,gl=on \
		-name "RainOS UEFI"

run-cpu: iso
	qemu-system-x86_64 \
		-enable-kvm \
		-machine q35 \
		-cpu host \
		-drive if=pflash,format=raw,readonly=on,file=$(EFI_CODE) \
		-cdrom $(ISO_FILE) \
		-m 12G \
		-display sdl,gl=on \
		-name "RainOS UEFI KVM"

# ---------------- Cleanup ----------------

clean:
	rm -rf $(BUILD_DIR)