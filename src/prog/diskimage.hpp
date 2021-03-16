#ifndef __DISKIMAGE_HPP__
#define __DISKIMAGE_HPP__

#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

#define SECTOR_SIZE         512
#define ADDR_MAP            0x100 * SECTOR_SIZE
#define ADDR_FILES          0x101 * SECTOR_SIZE
#define ADDR_SECTORS        0x103 * SECTOR_SIZE

#define RANGE_MAP           32
#define RANGE_FILES         64
#define RANGE_SECTORS       32

#define ADDR_MAP_MAX        ADDR_MAP + RANGE_MAP * 16
#define ADDR_FILES_MAP      ADDR_FILES + RANGE_FILES * 16
#define ADDR_SECTORS_MAX    ADDR_SECTORS + RANGE_SECTORS * SECTOR_SIZE

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

            files.open(filename);
            files.seekg(ADDR_FILES);
            files.seekp(ADDR_FILES);

            sectors.open(filename);
            sectors.seekg(ADDR_SECTORS);
            sectors.seekp(ADDR_SECTORS);
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
            int mapIdx = findMap(0, RANGE_MAP);
            
            fstream inputFile;
            inputFile.open(inputFileName);

            inputFile.seekg(0, inputFile.end);
            streamsize inputSize = inputFile.tellg();
            inputFile.seekg(0, inputFile.beg);
            char* buffer = new char[inputSize];
            
            inputFile.read(buffer, inputSize);

            addToDirectory(dirIdx, mapIdx, inputFileName);
            writeSector(mapIdx, buffer, inputSize);
            
            delete[] buffer;
        }

    private:
        void validateInputFile(char* inputFileName)
        {
            fstream inputFile;
            inputFile.open(inputFileName);
            inputFile.seekg(0, inputFile.end);
            if (inputFile.tellg() > 16 * SECTOR_SIZE)
            {
                throw "File too large";
            }

            string s(inputFileName);
            if (s.length() > 14)
            {
                throw "File name too long";
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
            bool isChild = (rowData[0] == parentIdx);
            bool isFolder = (rowData[1] == 0xFF);
            bool correctFolderName = (folderName.compare(2, 14, rowData));

            return (isChild && isFolder && correctFolderName);
        };

        // Finds available space on map
        int findMap(int begin, int end)
        {
            int i = begin;
            map.seekg(ADDR_MAP);
            bool found = false;
            char* buffer = new char[16];
            while (i < end && !found)
            {
                map.seekg(ADDR_MAP + i * 16);
                map.get(buffer, 16);
                found = (buffer[0] == 0x00);
                if (!found)
                {
                    i++;
                };
            }

            if (i >= end)
            {
                throw "No space available";
            }

            map.seekg(ADDR_MAP);
            delete[] buffer;
            return i;
        }

        // Writes data into sector. sectorIdx is result of findMap
        void writeSector(int sectorIdx, char* buffer, int size)
        {
            sectors.seekp(ADDR_SECTORS + sectorIdx * 16 * SECTOR_SIZE);
            sectors.write(buffer, size);
            sectors.seekp(ADDR_SECTORS);

            char* mark;
            mark[0] == 0xFF;

            char* freemark;
            freemark[0] == 0x00;

            map.seekp(ADDR_MAP + sectorIdx * 16);
            cout << "Starting to write on map at address " << hex << ADDR_MAP + sectorIdx * 16 << endl;

            map.put(0xFF);
            // for(int i = 0; i < 16; i++)
            // {
            //     // if (i > floor(size / SECTOR_SIZE))
            //     // {
            //     //     map.write(freemark, 1);
            //     // } else {
            //     //     map.write(mark, 1);
            //     // }
                
            //     cout << "HUHI" << endl;
            // }

            cout << "Writing sector index " << sectorIdx << " at address " << hex << ADDR_SECTORS + sectorIdx * 16 * SECTOR_SIZE << endl;
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
            while(fileName[k] != 0x00 && i < 15)
            {
                buffer[i] = fileName[k];
                i++;
                k++;
            }

            buffer[i] = 0x00;

            cout << "Writing to directory index " << dirIdx << " at address " << hex << ADDR_FILES + dirIdx * 16 << " with contents "; 
            for(int j = 0; j < i; j++)
            {
                cout << buffer[j];
            }

            cout << " pointing to sector " << hex << sectorIdx << endl;
            files.write(buffer, i);
            files.seekp(ADDR_FILES);

            delete[] buffer;
        }

        // Find something in sector, return idx
        int findFileInSector (int begin, int end, string folderName, int parentIdx) //  bool (*query)(char*)
        {
            int i = begin;
            bool found = false;
            char* buffer = new char[16];
            while (i < end && !found)
            {
                files.get(buffer, 16);
                found = folderWithParent(folderName, parentIdx, buffer);
                if (!found) i++;
            }

            if (i >= end)
            {
                throw "Directory not found";
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
                files.get(buffer, 16);
                found = (buffer[2] == 0x00);
                if (!found) i++;
            }

            if (i >= end)
            {
                throw "No directory space available";
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