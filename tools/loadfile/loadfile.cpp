
// #include <filesystem>
#include <string>
#include <fstream>
#include "diskimage.hpp"

#define SECTOR_SIZE 512
#define IMAGE_NAME argv[1]
#define INPUT_NAME argv[2]
#define DIRECTORY_NAME argv[3]

// loadfile.exe <system.img> -f <foldername> <path>
#define FOLDER_NAME argv[3]
#define FOLDER_PATH argv[4]

using namespace std;

// W I P - - - C O M I N G - - - S O O N
void validateInput(string inputFileName)
{

}

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
    if (argc == 3) // loadfile <system.img> <file>
    {
        cout << "HE" << endl;
        system.addFile(INPUT_NAME, "");
    } else if (argc == 5)
    {
        system.addFolder(FOLDER_NAME, FOLDER_PATH);
    } else // loadfile <system.img> <file / -f> <path>
    { 
        string s(INPUT_NAME);
        if (s == "-f")
        {
            system.addFolder(FOLDER_NAME, "");
        } else {
            system.addFile(INPUT_NAME, DIRECTORY_NAME);
        }
    }
}

