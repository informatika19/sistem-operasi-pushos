import sys

if len(sys.argv)<2:
    print("Masukkan path file yang benar")

try:
    f = open(sys.argv[1],"r")
except FileNotFoundError:
    print("Wrong file or file path")
    return

try:
    system = open("../system.img","r+")
except FileNotFoundError:
    print("Wrong file or file path")
    return

maps = []
system.seek(512,0)
for i in range(0,512):
    maps[i]=system.read(1)

files = []
system.seek(1024,0)
for i in range(0,512):
    files[i]=system.read(1)











