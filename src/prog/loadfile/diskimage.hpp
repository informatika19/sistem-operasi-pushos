#ifndef __DISKIMAGE_HPP__
#define __DISKIMAGE_HPP__

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>

using namespace std;

#define SECTOR_SIZE         512
#define ADDR_MAP            0x100 * SECTOR_SIZE
#define ADDR_FILES          0x101 * SECTOR_SIZE
#define ADDR_SECTORS        0x103 * SECTOR_SIZE

#define RANGE_MAP           256
#define RANGE_FILES         64
#define RANGE_SECTORS       32

#define ADDR_MAP_MAX        ADDR_MAP + RANGE_MAP * 16
#define ADDR_FILES_MAP      ADDR_FILES + RANGE_FILES * 16
#define ADDR_SECTORS_MAX    ADDR_SECTORS + RANGE_SECTORS * 16

class DiskImage {

    fstream diskimage;
    fstream map;
    fstream files;
    fstream sectors;

    public:
        DiskImage(char* filename)
        {
            diskimage.open(filename);

            map.open(filename);
            map.seekg(ADDR_MAP);
            map.seekp(ADDR_MAP);
            for(int i = 0; i < 10; i++)
            {
                map.put(0xFF);
            }
            map.seekp(ADDR_MAP);

            // flagMap(5);
            // flagMap(8);
            // map.seekp(ADDR_MAP);

            files.open(filename);
            files.seekg(ADDR_FILES);
            files.seekp(ADDR_FILES);

            sectors.open(filename);
            sectors.seekg(ADDR_SECTORS);
            sectors.seekp(ADDR_SECTORS);

            // flagSector(0, 0xDD, 0);
            // sectors.put(0xFF);
        };

        ~DiskImage()
        {
            map.close();
            files.close();
            sectors.close();
        }

        // Someone else pls do validation pls
        void addFile(char* inputFileName, string targetPath)
        {
        
            validateInputFile(inputFileName);
            int dirIdx = findDirectory(targetPath);
            int secIdx = findSector(0, RANGE_SECTORS);
            
            fstream inputFile;
            inputFile.open(inputFileName);

            inputFile.seekg(0, inputFile.end);
            streamsize inputSize = inputFile.tellg();
            inputFile.seekg(0, inputFile.beg);
            char* buffer = new char[inputSize];
            
            inputFile.read(buffer, inputSize);

            addToDirectory(dirIdx, secIdx, inputFileName);
            // cout << "TEST 1" << endl;
            writeSector(secIdx, buffer, inputSize);
            // cout << "TEST 2" << endl;
            
            delete[] buffer;
        }

        void addFolder(string folderName, string targetPath)
        {
            // validateInputFile(inputFolderName);
            int dirIdx = findDirectory(targetPath);
            // int mapIdx = findMap(0, RANGE_MAP);

            addFolderToDirectory(dirIdx, folderName);
        }

    private:
        void validateInputFile(char* inputFileName)
        {
            fstream inputFile;
            inputFile.open(inputFileName);
            inputFile.seekg(0, inputFile.end);
            if (inputFile.tellg() > 16 * SECTOR_SIZE)
            {
                cout << "ERROR" << 103;
                exit(EXIT_FAILURE);
            }

            string s(inputFileName);
            string delimit = "/";
            
            size_t pos=0;

            while ((pos = s.find(delimit)) != string::npos)
            {
                s.erase(0, pos + 1);
            }

            if (s.length() > 14)
            {
                cout << "ERROR" << 118;
                exit(EXIT_FAILURE);
            }
        }

        // Finds the directory of the specified directory
        int findDirectory(string pathString) 
        {
            vector<string> path;
            parsePath(pathString, path);

            if (pathString == "")
            {
                return 0xFF;
            }

            int parentIdx = 0xFF;
            for(int i = 0; i < path.size(); i++)
            {
                parentIdx = findFolderWithParent(path[i], parentIdx);
            }

            return parentIdx;
        }

        // Find folder with parent
        int findFolderWithParent(string folderName, int parentIdx)
        {
            return findFileInSector(0, RANGE_FILES, folderName, parentIdx);
        }

        bool folderWithParent(string folderName, int parentIdx, char* rowData)
        {
            bool isChild = ((unsigned char)rowData[0] == parentIdx);
            bool isFolder = ((unsigned char)rowData[1] >= 0xFF);

            bool correctFolderName = (folderName.compare(rowData+2) == 0);

            return (isChild && isFolder && correctFolderName);
        };

        // Finds available space on map
        int findMap(int begin, int end)
        {
            int i = begin;
            map.seekg(ADDR_MAP);
            bool found = false;
            char flag;
            // char* buffer = new char[1];
            while (i < end && !found)
            {
                map.seekg(ADDR_MAP + i);
                map.get(flag);
                found = (flag == 0x00);
                if (!found)
                {
                    i++;
                };
            }
            // cout << "Found available map at index " << i << endl;

            if (i >= end)
            {
                cout << "ERROR" << 179;
                exit(EXIT_FAILURE);
            }

            map.seekg(ADDR_MAP);
            // cout << "Found available sector at " << i << endl;

            // delete[] buffer;
            return i;
        }

        void flagMap(int idxMap)
        {
            map.seekp(ADDR_MAP + idxMap);
            map.put(0xFF);
        }

        void flagSector(int idxSector, int idxMap, int i)
        {
            sectors.seekp(ADDR_SECTORS + idxSector * 16 + i);
            sectors.put(idxMap);
            // sectors.write("GO FUCK YOURSELF", 16);
        }

        int findSector(int begin, int end)
        {
            int i = begin;
            sectors.seekg(ADDR_SECTORS);
            bool found = false;
            char empty;
            while (i < end && !found)
            {
                sectors.seekg(ADDR_SECTORS + i*16);
                sectors.get(empty);
                found = (empty == 0x00);
                // cout << buffer[0] << " THIS WHAT I GOT" << endl;
                if (!found)
                {
                    i++;
                };
            }

            if (i >= end)
            {
                cout << "ERROR" << 222;
                exit(EXIT_FAILURE);
            }

            sectors.seekg(ADDR_SECTORS);
            return i;
        }

        // Writes data into sector. sectorIdx is result of findMap
        void writeSector(int sectorIdx, char* buffer, int size)
        {
            char* emptySector = new char[512];

            int availableSector;
            int remainingSize = size;
            cout << "Writing sector table index " << sectorIdx << " at address " << hex << ADDR_SECTORS + sectorIdx * 16  << endl;

            int i;
            int sectionsNeeded = div(size, SECTOR_SIZE).quot + 1;
            cout << "Taking " << sectionsNeeded << " sections" << endl;


            for( i = 0; i < sectionsNeeded; i++)
            {
                availableSector = findMap(1, RANGE_MAP);

                cout << "Flagging map table at address " << hex << ADDR_MAP + availableSector << endl;
                flagMap(availableSector);

                cout << "Assigning sector " << hex << availableSector << " to table sector col " << i << " with sectorIdx " << sectorIdx << endl;
                flagSector(sectorIdx, availableSector, i);
                
                cout << "Writing to available sector " << availableSector << " at address " << hex << SECTOR_SIZE * availableSector << endl;
                diskimage.seekp(SECTOR_SIZE * availableSector);
                diskimage.write(emptySector, 512);
                diskimage.seekp(SECTOR_SIZE * availableSector);

                if (remainingSize > 512)
                {
                    diskimage.write(buffer+i*512, 512);
                    remainingSize -= 512;
                } else {
                    cout << "File remainder size " << remainingSize << endl;
                    diskimage.write(buffer, remainingSize);
                    remainingSize = 0;
                }
                
            }
        }

        void addToDirectory(int parentIdx, int sectorIdx, char* fileName)
        {
            int dirIdx = findAvailableDirectorySlot(0, RANGE_FILES);
            files.seekp(ADDR_FILES + dirIdx * 16);
            
            char* buffer = new char[16];
            buffer[0] = parentIdx;
            buffer[1] = sectorIdx;

            int i = 2;
            int k = 0;

            // cout << "TEST 3 " << endl;
            string s(fileName);
            string delimit = "/";
            
            size_t pos=0;

            while ((pos = s.find(delimit)) != string::npos)
            {
                s.erase(0, pos + 1);
            }

            for (k = 0; k < s.length() && k < 13; k++)
            {
                buffer[i] = s[k];
                i++;
            }

            buffer[i] = 0x00;

            cout << "Writing to directory index " << dirIdx << " at address " << hex << ADDR_FILES + dirIdx * 16 << " with contents "; 
            for(int j = 0; j < i; j++)
            {
                cout << buffer[j];
            }

            cout << " pointing to sector table row " << hex << sectorIdx << endl;
            files.write(buffer, i);
            files.seekp(ADDR_FILES);

            delete[] buffer;
        }

        void addFolderToDirectory(int parentIdx, string fileName)
        {
            int dirIdx = findAvailableDirectorySlot(0, RANGE_FILES);
            files.seekp(ADDR_FILES + dirIdx * 16);
            
            char* buffer = new char[16];
            buffer[0] = parentIdx;
            buffer[1] = 0xFF;

            int i = 2;
            int k = 0;

            string s(fileName);
            string delimit = "/";
            
            size_t pos=0;

            while ((pos = s.find(delimit)) != string::npos)
            {
                s.erase(0, pos + 1);
            }

            for (k = 0; k < s.length() && k < 13; k++)
            {
                buffer[i] = s[k];
                i++;
            }

            buffer[i] = 0x00;

            cout << "Writing to directory index " << dirIdx << " at address " << hex << ADDR_FILES + dirIdx * 16 << " with contents "; 
            for(int j = 0; j < i; j++)
            {
                cout << buffer[j];
            }

            // cout << " pointing to sector table row " << hex << sectorIdx << endl;
            files.write(buffer, i);
            files.seekp(ADDR_FILES);

            delete[] buffer;
        }

        // Find something in sector, return idx
        int findFileInSector (int begin, int end, string folderName, int parentIdx) //  bool (*query)(char*)
        {
            int i = begin;
            bool found = false;
            char* buffer = new char[17];
            while (i < end && !found)
            {
                files.get(buffer, 17);
                found = folderWithParent(folderName, parentIdx, buffer);
                if (!found) i++;
            }

            if (i >= end)
            {
                cout << "ERROR" << 320;
                exit(EXIT_FAILURE);
            }

            files.seekg(ADDR_FILES);
            delete[] buffer;
            return i;
        }

        int findAvailableDirectorySlot(int begin, int end)
        {
            int i = begin;
            bool found = false;
            char* buffer = new char[16];
            while (i < end && !found)
            {
                files.seekg(ADDR_FILES + i * 16);
                files.get(buffer, 16);
                found = (buffer[2] == 0x00);
                if (!found) i++;
            }

            if (i >= end)
            {
                cout << "ERROR" << 343;
                exit(EXIT_FAILURE);
            }

            files.seekg(ADDR_FILES);
            delete[] buffer;
            return i;
        }

        // Parse path
        void parsePath(string pathString, vector<string> &path)
        {
            stringstream ss(pathString);
            string s;

            while(getline(ss, s, '/'))
            {
                path.push_back(s);
            }
        }
};

#endif
