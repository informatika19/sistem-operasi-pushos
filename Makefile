TOOLS=tools
BOCHS?=bochs

OUT_DIR=out
ASM_DIR=src/asm
C_DIR=src/c
LIB_C_DIR=src/c/lib

KERNEL_C=$(C_DIR)/kernel.c
KERNEL_OUT=$(OUT_DIR)/kernel.o
KERNEL=$(OUT_DIR)/kernel
KERNEL_ASM=$(ASM_DIR)/kernel.asm
KERNEL_ASM_OUT=$(OUT_DIR)/kernel_asm.o

LIB_C = math
LIB_C_OUT=$(patsubst %, $(OUT_DIR)/lib_%.o, $(LIB_C))
# LIB_C_ASM=$()/lib.asm
# LIB_C_ASM_OUT=$(OUT_DIR)/lib_asm.o

BOOTLOADER=$(OUT_DIR)/bootloader
BOOTLOADER_ASM=$(ASM_DIR)/bootloader.asm

SYS_IMG=$(OUT_DIR)/system.img

$(OUT_DIR)/lib_%.o: $(LIB_C_DIR)/%.c
	bcc -ansi -c -o $@ $<

$(OUT_DIR):
	mkdir $(OUT_DIR)

$(SYS_IMG): $(OUT_DIR) $(BOOTLOADER) $(KERNEL)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BOOTLOADER) of=$@ bs=512 conv=notrunc count=1
	dd if=$(KERNEL) of=$@ bs=512 conv=notrunc seek=1

$(BOOTLOADER): $(BOOTLOADER_ASM)
	nasm $< -o $@

$(KERNEL_OUT): $(KERNEL_C) $(OUT_DIR)
	bcc -ansi -c -o $@ $<

$(KERNEL_ASM_OUT): $(KERNEL_ASM) $(OUT_DIR)
	nasm -f as86 $< -o $@ -I $(OUT_DIR)

$(KERNEL): $(KERNEL_OUT) $(LIB_C_OUT) $(KERNEL_ASM_OUT)
	ld86 -o $@ -d $^

build: $(SYS_IMG)

run: build
	$(BOCHS) -f if2230.config

clean:
	rm -rf out/*

.PHONY: build run clean
