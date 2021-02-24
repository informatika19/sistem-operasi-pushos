# Penjelasan Milestone 1 Tugas IF2230 Sistem Operasi
## Cara kerja interrupt pada sistem operasi
Interrupt adalah sinyal yang dikirimkan ke CPU oleh suatu entitas pada komputer sehingga CPU menghentikan seluruh aktivitasnya dan melakukan suatu eksekusi tertentu. Terdapat tiga jenis interrupt yaitu :
### *Hardware interrupt*
>*Hardware interrupt* dikirmkan oleh hardware eksternal sebagai sinyal penanda bahwa kini hardware tersebut membutuhkan perhatian OS. Umumnya interrupt ini dihasilkan oleh I/O *device* seperti *keyboard* ketika menerima ketikan dari pengguna. 
### *Software interrupt*
>*software interrupt* dikirimkan oleh suatu *software* ketika *software* tersebut ingin melakukan *system call* yaitu suatu proses yang bertujuan untuk melakukan aksi pada *hardware* komputer dengan bantuan OS.
### *Traps*
>*traps* merupakan suatu *interrupt* yang dihasilkan oleh CPU itu sendiri ketika menemui kondisi *error* seperti *division by zero*.

Untuk dapat melakukan *interrupt* pada sistem operasi, maka digunakanlah fungsi `interrupt` dari `kernel.asm`. Fungsi `interrupt` memiliki 4 parameter yaitu `int number`, `int AX`, `int BX`, `int CX`, `int DX`. Parameter `number` diisi dengan nomor *Interrupt Request*(IRQ) dalam hexadecimal, pada milestone 1 ini kami menggunakan *interrupt* 10 dan *interrupt* 16. Kemudian, parameter `AX`, `BX`, `CX`, `DX` diisi dengan suatu nilai hexadecimal yang ingin dimasukkan ke dalam register terkait agar fungsi dalam interrupt yang terpanggil sesuai. 

## Cara kerja kode pada kernel.asm
Berikut ini adalah fungsi-fungsi yang terdapat dalam `kernel.c` dengan penjelasannya.
### makeInterrupt21
> Fungsi ini digunakan untuk melakukan *system call* pada sistem.
### printString
> Fungsi ini digunakan untuk mencetak suatu string ke dalam layar. Parameter dari fungsi ini adalah sebuah string yang akan dicetak. Kemudian untuk mencetak string, dilakukan iterasi per karakter hingga ditemukan *null terminator* untuk dilakukan pencetakan karakter oleh *interrupt* 10 dengan `AH` berisi 0Eh dan `AL` berisi karakter yang ingin ditulis.
### readString
> Fungsi ini digunakan untuk membaca string dari input pengguna melalui *keyboard*. Parameter dari fungsi ini adalah pointer alamat penyimpanan string. Kami membatasi panjangnya string yang dapat dibaca sepanjang 16 karakter sehingga pembacaan karakter dari input pengguna dilakukan hingga diterima input `\r` atau panjang string sudah mencapai 16 karakter. Pembacaan karakter dari *keyboard* pengguna dilakukan dengan *interrupt* 16 dengan AX bernilai 0.
### clear
> Fungsi ini digunakan untuk mengosongkan buffer penyimpanan string dengan cara mengeset tiap elemen array menjadi 0.  
### lineLogo
> Fungsi ini digunakan untuk mencetak 1 baris ASCII art logo dengan menggunakan fungsi `putInMemory` untuk mencetak tiap karakter.
### logo
> Fungsi ini digunakan untuk memanggil fungsi lineLogo untuk seluruh baris ASCII art logo.
### main
> Pada `main`, kami pertama-tama menyiapkan buffer character sepanjang 16 karakter (panjang string maksimal yang kami atur), kemudian kami memanggil `interrupt 10` untuk berpindah ke mode grafis dan mengosongkan layar. Setelah itu, kami memanggil fungsi `printLogo` untuk mencetak logo ASCII pada layat. Lalu, kami memanggil fungsi `readString` untuk membaca input dari pengguna dan menyimpannya pada buffer yang sudah kami siapkan sebelumnya. Kemudian, kami memanggil fungsi `printstring` untuk mencetak string yang tersimpan dalam buffer. Terakhir, kami memanggil fungsi `clear` untuk mengosongkan kembali isi buffer.