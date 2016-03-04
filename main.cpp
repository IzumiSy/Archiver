/* ----------------------------------------
 *
 *  SIMPLE ARCHIVER
 *
 *  File: main.cpp
 *  Version: 0.1
 *  Author: IzumiSy <beetle-noise@gmx.com>
 *  Create: 2011/9/21
 *
 * ------------------------------------- */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
using namespace std;

#include "types.h"
#include "misc.h"
#include "usage.h"

#define MAX_SIZE    4 * 1024 * 1024
#define MAX_FILES   100
#define MAX_HEADSIZ 8

#define ARCH_NAME   "archive.out"
#define INDEX_NAME  "index.txt"

static const char headerSignature[] = "SaRc";

vector<struct FILEDATA> parseInputFiles(vector<string> inputFilePaths)
{
    int i;
    int filesLen = inputFilePaths.size();
    size_t fileSize, current;
    string filenameBuffer;
    ifstream in;

    struct FILEDATA base;
    vector<struct FILEDATA> fileDataArray;

    current = 0;
    for (i = 0;i < filesLen;i++) {
        filenameBuffer = inputFilePaths.at(i);
        in.open(filenameBuffer, ios::in | ios::binary);
        if (in.fail()) {
            throw (new string("File open error : "))->append(filenameBuffer);
        }
        fileSize = in.seekg(0, ios::end).tellg();
        in.seekg(0, ios::beg);
        base.filename.assign(filenameBuffer);
        base.fsize = fileSize;
        base.add += fileSize;
        current += base.fsize;
        fileDataArray.push_back(base);
        in.close();
    }

    return fileDataArray;
}

// Detects "-o" option that specifies output file name
// and input files as arguments.
struct OPTIONS parseExecArguments(int argc, char *argv[])
{
    int i;
    struct OPTIONS options;
    string argumentBuffer;
    string outputFileName;

    options.outputFileName = ARCH_NAME;

    for (i = 1;i < argc;i++) {
        argumentBuffer = argv[i];
        if (argumentBuffer.find_first_of("-o", 0) == 0) {
            options.outputFileName = argumentBuffer.substr(2);
            argv[i] = NULL;
        } else {
            options.inputFilePaths.push_back(argumentBuffer);
        }
    }

    return options;
}

void writeArchiveFile(string outputFileName, vector<struct FILEDATA> inputFiles)
{
    int i, inputFileAmount = inputFiles.size();
    ofstream out;
    UCHAR *dataBuffer;

    // Make a header and write it to an archived file.
    // A default file name is "archive.out"
    out.open(outputFileName.c_str(), ios::binary | ios::out | ios::trunc);
    if (out.fail()) {
        throw ("File open error : Archive");
    }

    if (!(dataBuffer = (UCHAR *)malloc(MAX_SIZE))) {
        throw ("Out of memory : Archive");
    }

    for (i = 0;i < 4;i++) {
        dataBuffer[i] = headerSignature[i];
    }
    put32(dataBuffer, inputFileAmount - 1);
    out << dataBuffer;

    // TODO
    // Write contents of input files into the archive file
    //

    free(dataBuffer);
    out.close();
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        putUsage();
    }

    struct OPTIONS options;
    vector<struct FILEDATA> fileData;

    options = parseExecArguments(argc, argv);
    if (options.inputFilePaths.size() <= 0) {
        cout << "No input files spcified" << endl;
        putUsage();
    }

    try {
        fileData = parseInputFiles(options.inputFilePaths);
        if (fileData.empty()) {
            throw ("Unexpected error");
        }
        writeArchiveFile(options.outputFileName, fileData);
    } catch (string reason) {
        cout << reason << endl;
    }

    return 0;
}
