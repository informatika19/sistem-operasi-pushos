BOCHS?=bochs

SRC=src
OUT_DIR=out
ASM_DIR=$(SRC)/asm
C_DIR=$(SRC)/c
LIB_C_DIR=$(C_DIR)/lib
SHELL_C_DIR=$(C_DIR)/shell
SHELL_C_APP_DIR=$(SHELL_C_DIR)/utilities
# BOOT_LOGO_DIR=$(SRC)/other

KSIZE=50

KERNEL_C=$(C_DIR)/kernel.c
KERNEL_OUT=$(OUT_DIR)/kernel.o
KERNEL=$(OUT_DIR)/kernel
KERNEL_ASM=$(ASM_DIR)/kernel.asm
KERNEL_ASM_OUT=$(OUT_DIR)/kernel_asm.o
# BOOT_LOGO_OUT=$(OUT_DIR)/logo.bin

LIB_C=math string logo fileIO folderIO
LIB_C_OUT=$(patsubst %, $(OUT_DIR)/lib_%.o, $(LIB_C))

SHELL_C_APP=mv cp mkdir rm cat ln
SHELL_C_APP_OUT=$(patsubst %, $(OUT_DIR)/shell_%.o, $(SHELL_C_APP))

SHELL_C=shell
SHELL_C_OUT=$(patsubst %, $(OUT_DIR)/shell.o, $(SHELL_C))

BOOTLOADER=$(OUT_DIR)/bootloader
BOOTLOADER_ASM=$(ASM_DIR)/bootloader.asm

SYS_IMG=$(OUT_DIR)/system.img
MAP_IMG=$(OUT_DIR)/map.img
SECTORS_IMG=$(OUT_DIR)/sectors.img
FILES_IMG=$(OUT_DIR)/files.img

$(OUT_DIR):
	mkdir $(OUT_DIR)

$(OUT_DIR)/lib_%.o: $(LIB_C_DIR)/%.c
	bcc -ansi -c -o $@ $<

$(OUT_DIR)/shell_%.o: $(SHELL_C_APP_DIR)/%.c
	bcc -ansi -c -o $@ $<

$(OUT_DIR)/shell.o: $(SHELL_C_DIR)/$(SHELL_C).c
	bcc -ansi -c -o $@ $<

# $(BOOT_LOGO_OUT): $(BOOT_LOGO_DIR)/logo.png
# 	python3 $(BOOT_LOGO_DIR)/image2bin.py $< $@

$(MAP_IMG):
	dd if=/dev/zero of=$@ bs=512 count=1
	python3 -c "import sys; sys.stdout.buffer.write(b'\xFF'*$(KSIZE))" | dd of=$@ bs=512 count=$(KSIZE) conv=notrunc

$(FILES_IMG):
	dd if=/dev/zero of=$@ bs=512 count=2

$(SECTORS_IMG):
	dd if=/dev/zero of=$@ bs=512 count=1

$(SYS_IMG): $(OUT_DIR) $(BOOTLOADER) $(KERNEL) $(MAP_IMG) $(SECTORS_IMG) $(FILES_IMG)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BOOTLOADER) of=$@ bs=512 conv=notrunc count=1
	dd if=$(KERNEL) of=$@ bs=512 conv=notrunc seek=1
	dd if=$(MAP_IMG) of=$@ bs=512 count=1 seek=256
	dd if=$(FILES_IMG) of=$@ bs=512 count=2 seek=257
	dd if=$(SECTORS_IMG) of=$@ bs=512 count=1 seek=259

$(BOOTLOADER): $(BOOTLOADER_ASM)
	nasm $< -o $@

$(KERNEL_OUT): $(KERNEL_C) $(OUT_DIR)
	bcc -ansi -c -o $@ $<

$(KERNEL_ASM_OUT): $(KERNEL_ASM) $(OUT_DIR) # $(BOOT_LOGO_OUT)
	nasm -f as86 $< -o $@ -I $(OUT_DIR)

$(KERNEL): $(KERNEL_OUT) $(LIB_C_OUT) $(SHELL_C_APP_OUT) $(SHELL_C_OUT) $(KERNEL_ASM_OUT)
	ld86 -o $@ -d $^

$(LANG):
	LANG=en-us.UTF-8

build: $(LANG) $(SYS_IMG)

run: build
	$(BOCHS) -f if2230.config

clean:
	rm -rf out/*

.PHONY: build run clean
