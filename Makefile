BOCHS?=bochs

SRC=src
OUT_DIR=out
ASM_DIR=$(SRC)/asm
C_DIR=$(SRC)/c
LIB_C_DIR=$(C_DIR)/lib
SHELL_C_DIR=$(C_DIR)/shell
SHELL_C_APP_DIR=$(SHELL_C_DIR)/utilities
TOOLS_DIR=tools
BIN=bin

KSIZE=16

SHELL_C=$(SHELL_C_DIR)/shell.c
SHELL_OUT=$(OUT_DIR)/shell.o
SHELLL=$(BIN)/shell
LIB_ASM=$(ASM_DIR)/lib.asm
LIB_ASM_OUT=$(OUT_DIR)/lib_asm.o

KERNEL_C=$(C_DIR)/kernel.c
KERNEL_OUT=$(OUT_DIR)/kernel.o
KERNEL_IO=$(C_DIR)/io.c
KERNEL_IO_OUT=$(OUT_DIR)/io.o
KERNEL=$(OUT_DIR)/kernel
KERNEL_ASM=$(ASM_DIR)/kernel.asm
KERNEL_ASM_OUT=$(OUT_DIR)/kernel_asm.o

LIB_C=math string fileIO folderIO
LIB_C_OUT=$(patsubst %, $(OUT_DIR)/lib_%.o, $(LIB_C))

SHELL_C_APP=cat mv cp mkdir rm ln logo
SHELL_C_APP_OUT=$(patsubst %, $(OUT_DIR)/shell_%.o, $(SHELL_C_APP))

BOOTLOADER=$(OUT_DIR)/bootloader
BOOTLOADER_ASM=$(ASM_DIR)/bootloader.asm

SYS_IMG=$(OUT_DIR)/system.img
MAP_IMG=$(OUT_DIR)/map.img
SECTORS_IMG=$(OUT_DIR)/sectors.img
FILES_IMG=$(OUT_DIR)/files.img

# LOADFILE_CPP=$(TOOLS_DIR)/loadfile/loadfile.cpp
# LOADFILE=$(OUT_DIR)/loadfile

# $(LOADFILE): $(OUT_DIR)
# 	gcc -o $@ $(LOADFILE_CPP)

$(OUT_DIR):
	mkdir $(OUT_DIR)

$(BIN):
	mkdir $(BIN)

$(OUT_DIR)/lib_%.o: $(LIB_C_DIR)/%.c
	bcc -ansi -c -o $@ $<

$(OUT_DIR)/shell_%.o: $(SHELL_C_APP_DIR)/%.c
	bcc -ansi -c -o $@ $<

$(SHELL_OUT): $(SHELL_C)
	bcc -ansi -c -o $@ $<

$(LIB_ASM_OUT): $(LIB_ASM) $(OUT_DIR)
	nasm -f as86 $< -o $@ -I $(OUT_DIR)

$(SHELLL): $(SHELL_OUT) $(LIB_C_OUT) $(LIB_ASM_OUT)
	ld86 -o $@ -d $^
	python3 tools/loadfile/loadfile.py out/system.img $@

$(SHELL_C_APP): $(LIB_C_OUT) $(LIB_ASM_OUT)
	ld86 -o $(BIN)/$@ -d $(OUT_DIR)/shell_$@.o $^
	python3 tools/loadfile/loadfile.py out/system.img $(BIN)/$@

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

$(KERNEL_IO_OUT): $(KERNEL_IO) $(OUT_DIR)
	bcc -ansi -c -o $@ $<

$(KERNEL_ASM_OUT): $(KERNEL_ASM) $(OUT_DIR)
	nasm -f as86 $< -o $@ -I $(OUT_DIR)

$(KERNEL): $(KERNEL_OUT) $(KERNEL_IO_OUT) $(OUT_DIR)/lib_math.o $(OUT_DIR)/lib_string.o $(KERNEL_ASM_OUT)
	ld86 -o $@ -d $^

$(LANG):
	LANG=en-us.UTF-8

loadshell: $(BIN) $(OUT_DIR) $(SHELLL) $(SHELL_C_APP_OUT) $(SHELL_C_APP)

build: $(LANG) $(SYS_IMG) loadshell

run: build
	$(BOCHS) -f if2230.config

clean:
	rm -rf out/*
	rm -rf bin/*

.PHONY: build run clean
