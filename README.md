# Milestone 1 Tugas Besar IF2230 Sistem Operasi
## Cara menjalankan OS
1. Pastikan seluruh dependensi telah ter-install di komputer.
2. Buat disk image dengan memasukkan command `dd if=/dev/zero of=system.img bs=512 count=2880` ke dalam terminal.
3. Kompilasi bootloader dengan command `nasm bootloader.asm -o bootloader`
4. Masukkan bootloader ke dalam disk image dengan command `dd if=bootloader of=system.img bs=512 count=1 conv=notrunc`
5. Buka terminal dan pindah ke folder `sistem-operasi-pushos`.
6. Masukkan command `make run clean` ke dalam terminal.
7. Tunggu hingga window Bochs terbuka, apabila window Bochs sudah terbuka maka OS sudah dapat digunakan.
## Alternatif Cara menjalankan OS
Apabila tidak berhasil menggunakan `Makefile`, ganti lah langkah 6 dengan memasukkan command berikut:
>`bcc -ansi -c -o kernel.o kernel.c`  
>`nasm -f as86 kernel.asm -o kernel_asm.o`  
>`ld86 -o kernel -d kernel.o kernel_asm.o`  
>`dd if=kernel of=system.img bs=512 conv=notrunc seek=1`  
## Dependensi
* Netwide Assembler
* Bruce C Compiler
* Id86
* Bochs
## File Markdown MILESTONE1
[MILESTONE 1](./MILESTONE1.md)
## Anggota Kelompok PushOS
* 13519116 Jeane Mikha Erwansyah
* 13519131 Hera Shafira
* 13519139 I Gede Govindabhakta
