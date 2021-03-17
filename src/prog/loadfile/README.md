# Cara Menggunakan Program loadfile

Program loadfile akan memasukkan file pilihan ke root.

1. _Compile_ `loadfile.cpp` dengan command berikut ini.
`$ g++ -o loadfile loadfile.cpp`
2. 
a. Load file
Buka terminal dan buka binary file dengan tambahan argumen: lokasi `system.img`dengan command berikut ini.
`$ ./loadfile ../../../out/system.img <path file yang ingin dimasukkan>`
Catatan : asumsi file yang ingin dimasukkan terletak di folder yang sama dengan `loadfile.cpp`.

b. Load folder
Buka terminal dan buka binary file dengan tambahan argumen: lokasi `system.img`, flag -f, nama folder, dan lokasi tujuan penyimpanan folder dengan command berikut ini.
`$ ./loadfile ../../../out/system.img -f <nama folder> <lokasi penyimpanan folder pada OS>`


___
