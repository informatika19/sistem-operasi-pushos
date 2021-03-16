# Cara Menggunakan Program loadfile

1. _Compile_ `loadfile.cpp` dengan GCC.
2. Buka terminal dan buka binary file dengan dua tambahan argumen: lokasi `system.img` dan lokasi file yang ingin dimasukkan.

```sh
S g++ -o loadfile loadfile.cpp
$ ./loadfile ../../../out/system.img <file yang ingin dimasukkan>
```
___
