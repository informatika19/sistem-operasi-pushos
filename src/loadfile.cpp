
#include <filesystem>
#include <string>
#include <fstream>
#include "diskimage.hpp"

#define SECTOR_SIZE 512
#define IMAGE_NAME argv[1]
#define INPUT_NAME argv[2]
#define DIRECTORY_NAME argv[3]

using namespace std;

int main(int argc, char** argv)
{
    /*
    loadfile.exe <system.img> <file> <path>
    argv[1] = System disk image
    argv[2] = Input file path
    argv[3] = Directory for file inside the system disk image
    */

    DiskImage system(IMAGE_NAME);
    validateInput(INPUT_NAME);
    system.addFile(INPUT_NAME, DIRECTORY_NAME);

}

// W I P - - - C O M I N G - - - S O O N
void validateInput(string inputFileName)
{
    /* Open the file */
    // Check size
    // 512 map, 1024 files, 512 sectors
    if (filesystem::file_size(inputFileName) > 16 * SECTOR_SIZE)
    {
        throw "File too large";
    }

    /* Input validation */
    // Parse and validate input
    if (inputFileName.length() > 14)
    {
        throw "File name too long";
    }

    // Check directory
    

    // Check map

    // Find area in map large enough for file
}