#!/usr/bin/env python

import sys
import os
import math

SSIZE=512
ENCODING="ascii"

if len(sys.argv) < 3:
  print(f'Usage: {sys.argv[0]} <system.img> <file to insert> [insertpath]')
  exit(1)

print(sys.argv)

syst = sys.argv[1]
file = sys.argv[2]
path = f'/{file}'
if len(sys.argv) > 3:
  path = sys.argv[3]
fsize = os.stat(file).st_size

# check file size limit
if fsize > 16*SSIZE:
  print(f'File size cannot exceed 8KiB!')
  exit(2)

file = open(file, 'rb')
sysi = open(syst, 'r+b')

sysi.seek(0x100*SSIZE, 0)
mapSection = sysi.read(SSIZE)

mapEmpty = [i for i,s in enumerate(mapSection) if s != 0xFF]
writeSectorCount = math.ceil(fsize/SSIZE)

# check map
if len(mapEmpty) < writeSectorCount:
  print(f'Map is full')

sysi.seek(0x101*SSIZE, 0)
fileSection = sysi.read(SSIZE*2)

fullPath = [x for x in path.split('/') if x != '.' and x != ""]
pathHierarchy = fullPath[:-1] if len(fullPath) > 1 else []
filename = fullPath[-1]

# check filename length 
if len(filename) > 14:
  print(f'Filename can\'t be longer than 14 characters in fs')
  exit(4)

# check folder hierarchy
currentParent = 0xFF

for p in pathHierarchy:
  if p == '..':
    currentParent = fileSection[currentParent]
  if len(p) > 14:
    print(f'Folder name can\'t be longer than 14 characters in fs')
    exit(4)
  found = False
  for i in range(0x40):
    offset = i<<4
    if fileSection[offset+2:offset+16].decode('ascii').rstrip('\x00') == p and fileSection[offset] == currentParent:
      currentParent = i
      found = True
      break
  if not found:
    # make folder
    for i in range(0x40):
      offset = i<<4
      if fileSection[offset:offset+16] == bytes([0 for _ in range(16)]):
        fileSection = fileSection[:offset] + bytes([currentParent, 0xFF]) + bytearray(p, ENCODING) + bytes([0 for _ in range(14-len(p))]) + fileSection[offset+16:]
        currentParent = i
        break

# check existence
for i in range(0x40):
  offset = i<<4
  if fileSection[offset] == currentParent and fileSection[offset+2:offset+16].decode().rstrip('\x00') == filename:
    print(f'File exists in root fs')
    exit(3)

# write file
# search for empty sector in sector table

sysi.seek(0x103*SSIZE, 0)
sectorTable = sysi.read(SSIZE)

sector = -1
for i in range(0x20):
  offset = i << 4
  if sectorTable[offset:offset+16] == bytes([0 for _ in range(16)]):
    sector = i
    break

if sector == -1:
  print(f'Sector table is full')
  exit(5)

# write file metadata (parent, sectorid, name)
for i in range(0x40):
  offset = i << 4
  if fileSection[offset:offset+16] == bytes([0 for _ in range(16)]):
    fileSection = fileSection[:offset] + bytes([currentParent, sector]) + bytearray(filename, ENCODING) + bytes([0 for _ in range(14-len(filename))]) + fileSection[offset+16:]
    break

sysi.seek(0x101*SSIZE, 0)
sysi.write(fileSection)
file.seek(0, 0)

for i in range(writeSectorCount):
  s = mapEmpty.pop(0)
  if s > 0xFF: exit(6)
  sysi.seek(s*SSIZE, 0)
  sysi.write(file.read(SSIZE))
  sysi.seek(0x100*SSIZE+s, 0)
  sysi.write(bytes([0xFF]))
  sysi.seek(0x103*SSIZE+(sector<<4)+i, 0)
  sysi.write(bytes([s]))

file.close()
sysi.close()