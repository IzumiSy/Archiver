#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned long ULONG;
typedef unsigned char UCHAR;

struct FILEDATA
{
    std::string filename;
    ULONG fsize;
    ULONG add;
};

struct OPTIONS
{
    std::string outputFileName;
    std::vector<std::string> inputFilePaths;
};

#endif
