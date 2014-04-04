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

#define MAX_SIZE	4 * 1024 * 1024
#define MAX_FILES	100
#define MAX_HEADSIZ	8

#define ARCH_NAME	"archive.out"
#define INDEX_NAME	"index.txt"

static const char sign[] = "SaRc";

typedef unsigned long ULONG;
typedef unsigned char UCHAR;

struct FILEDATA
{
	string	filename;
	ULONG	fsize;
	ULONG	add;
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
	puts(" usage: >archiver file1 file2 ...");
	exit(0);
} 

void createIndexFile(void)
{
    ofstream fp;

    fp.open(INDEX_NAME, ios::out | ios::trunc);

    // Do something

    fp.close();
}

int main(int argc, char *argv[])
{
	int i, files;
	size_t fsize, current;
	string err0, optbuf, outputname;
	UCHAR *buf;
	
	ostringstream osstream;
	ifstream in;
	ofstream out, indexfile;
	vector<string> errs;
	vector<struct FILEDATA> filedats;
	struct FILEDATA base;
	
	outputname = ARCH_NAME;
	current = 0;
	
	if (argc < 2) {
		putUsage();
	}

    // Names it if there is a specified name of the achieve file.
    // And set a file name string to "outputname"
	for (i = 1;i < argc;i++) {
		optbuf = argv[i];
		if (optbuf.find_first_of("-o", 0) == 0) {
			outputname = optbuf.substr(2);
			cout << "output: " << outputname << endl;
			argv[i] = 0;
		}
	}

    try {

        // Parses a command line and gets size and name of each file to add.
        // And stores them to the file list.
	    for (i = 1;i < argc;i++) {
		    if (argv[i] != NULL) {
			    in.open(argv[i], ios::in | ios::binary);
			    if (in.fail()) {
				    throw (new string("File open error : "))->append(argv[i]);
			    }
    		    fsize = in.seekg(0, ios::end).tellg();
    		    in.seekg(0, ios::beg);
    		    base.filename.assign(argv[i]);
    			base.fsize = fsize;
    			base.add += fsize;
    			current += base.fsize;
    			filedats.push_back(base);
    			in.close();
    		}
    	}

		if (filedats.empty()) {
            throw ("Unexpected error");   
        }
        
        // Display name of files to add
		for (i = 0;i < (signed)filedats.size();i++) {
			cout << filedats.at(i).filename << endl;
		}
        cout << " -> " << outputname << endl;
		
        // Make a header and write it to an archived file.
        // A default file name is "archive.out"
		out.open(outputname.c_str(), ios::binary | ios::out | ios::trunc);
		if (out.fail()) 
			throw ("File open error : Archive");
		
		buf = (UCHAR *)malloc(MAX_SIZE);
		if (!buf) 
			throw ("Out of memory : Archive");
		
		// Makes a file header and write it.
		// sign: A File Signature
		// files: The number of files which is archived.
		for (i = 0;i < 4;i++) { 
			buf[i] = sign[i];
		}
		files = argc - 1;
		put32(buf, files);
		
		out << buf;

		free(buf);
		out.close();
		
	} catch (vector<string> reason) {
		for (i = 0;i < (int)reason.size();i++) {
			cout << reason.at(i) << endl;
		}		
	} catch (string reason) {
		cout << reason << endl;
	}
}
