TOOLS=tools
BOCHS ?= bochs
kernel_c=src/kernel.c
kernel_asm=src/kernel.asm
bootloader_asm=src/bootloader.asm

out=out

sys_img=$(out)/system.img
bootloader=$(out)/bootloader
kernel_o=$(out)/kernel.o
kernel_asm_o=$(out)/kernel_asm.o
kernel=$(out)/kernel

$(out):
	mkdir $(out)

$(sys_img): $(out) $(bootloader) $(kernel)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(bootloader) of=$@ bs=512 conv=notrunc count=1
	dd if=$(kernel) of=$@ bs=512 conv=notrunc seek=1

$(bootloader): $(bootloader_asm)
	nasm $< -o $@

$(kernel_o): $(kernel_c) $(out)
	bcc -ansi -c -o $@ $<

$(kernel_asm_o): $(kernel_asm) $(out)
	nasm -f as86 $< -o $@ -I $(out)

$(kernel): $(kernel_o) $(kernel_asm_o)
	ld86 -o $@ -d $^

build: $(sys_img)

run: build
	$(BOCHS) -f if2230.config

clean:
	rm -rf out/*

.PHONY: build run clean
