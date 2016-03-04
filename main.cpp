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

#define MAX_SIZE    4 * 1024 * 1024
#define MAX_FILES   100
#define MAX_HEADSIZ 8

#define ARCH_NAME   "archive.out"
#define INDEX_NAME  "index.txt"

static const char sign[] = "SaRc";

typedef unsigned long ULONG;
typedef unsigned char UCHAR;

struct FILEDATA
{
    string filename;
    ULONG  fsize;
    ULONG  add;
};

struct OPTIONS
{
    string outputFileName;
    vector<string> inputFilePaths;
};

int get32(const UCHAR *p)
{
    return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

void put32(UCHAR *p, int i)
{
    p[0] =  i        & 0xff;
    p[1] = (i >>  8) & 0xff;
    p[2] = (i >> 16) & 0xff;
    p[3] = (i >> 24) & 0xff;
}

void putUsage(void)
{
    puts("[ Simple Archiver version 0.1 ]");
    puts("Copyright (C) 2011 IzumiSy <beetle-noise@gmx.com>");
    puts(" usage: archiver [ -o ] file1 file2 ...");
    exit(0);
}

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
    int i, files;
    int inputFileAmount = inputFiles.size();
    ofstream out;
    UCHAR *buf;

    // Make a header and write it to an archived file.
    // A default file name is "archive.out"
    out.open(outputFileName.c_str(), ios::binary | ios::out | ios::trunc);
    if (out.fail()) {
        throw ("File open error : Archive");
    }

    if (!(buf = (UCHAR *)malloc(MAX_SIZE))) {
        throw ("Out of memory : Archive");
    }

    // Makes a file header and write it.
    // sign: A File Signature
    // files: The number of files which is archived.
    for (i = 0;i < 4;i++) {
        buf[i] = sign[i];
    }
    files = inputFileAmount - 1;
    put32(buf, files);

    out << buf;

    free(buf);
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
