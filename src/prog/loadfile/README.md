# Cara Menggunakan Program loadfile

Program loadfile akan memasukkan file pilihan ke root.

1. _Compile_ `loadfile.cpp` dengan GCC.
2. Buka terminal dan buka binary file dengan tambahan argumen: lokasi `system.img`.

```sh
S g++ -o loadfile loadfile.cpp
$ ./loadfile ../../../out/system.img <file yang ingin dimasukkan>
```
___
