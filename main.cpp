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

/* ----------------------------- 
 * 
 *	A stricture of a file data
 *	(72byte)
 * 
 * -------------------------- */
struct FILEDATA
{
	string	filename;
	ULONG	fsize;
	ULONG	add;
};

/* ---------------------------
 * 
 *	     Writing header. 
 * 
 * ------------------------ */
int get32(const UCHAR *p)
{
	return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

/* ---------------------------
 * 
 *	     Reading header.
 * 
 * ------------------------ */
void put32(UCHAR *p, int i)
{
	p[0] =  i        & 0xff;
	p[1] = (i >>  8) & 0xff;
	p[2] = (i >> 16) & 0xff;
	p[3] = (i >> 24) & 0xff;
}

/* ---------------------------
 * 
 *	       Put usage.
 * 
 * ------------------------ */
void usage(void)
{
	puts("sarchive [ Simple Archiver version 0.1 ]");
	puts("Copyright (C) 2011 JsZ <beetle-noise@gmx.com>");
	puts(" usage: >sarchive file1 file2 ...");
	exit(0);
}

/* ---------------------------
 * 
 *	          Main
 * 
 * ------------------------ */
int main(int argc, char *argv[])
{
	int i, var0, files;
	time_t now;
	size_t fsize, size, current;
	string err0, optbuf, outputname;
	UCHAR *buf;
	char str0[64];
	bool nonindex;
	
	ostringstream osstream;
	ifstream in;
	ofstream out, indexfile;
	vector<string> errs;
	vector<struct FILEDATA> filedats;
	struct FILEDATA base;
	struct tm *date;
	
	outputname = ARCH_NAME;
	current = 0;
	nonindex = false;
	
	/*
	 *	Put usage.
	 */
	if (argc < 2) {
		usage();
	}

	/*
	 *  Names it if there is a specified name of the achieve file.
	 *  And set a file name string to "outputname"
	 */
	for (i = 1;i < argc;i++) {
		optbuf = argv[i];
		if (optbuf.find_first_of("-o", 0) == 0) {
			outputname = optbuf.substr(2);
			cout << "output: " << outputname << endl;
			argv[i] = 0;
		}
	}

	/*
	 * 	Parses a command line and Pushes each data to a file list.
	 *	If some errors occur, store them into container. 
	 */
	for (i = 1;i < argc;i++) {
		if (argv[i] != NULL) {
			in.open(argv[i], ios::in | ios::binary);
			if (in.fail()) {
				errs.push_back((new string("File open error : "))->append(argv[i]));
				continue;
			}
			
			/* Gets file size. */
			fsize = in.seekg(0, ios::end).tellg();
			in.seekg(0, ios::beg);
			
			/*
			 *	Sets file data.
			 *	Pushs it to the file list.
			 */
			base.filename.assign(argv[i]);
			base.fsize = fsize;
			base.add += fsize;
			current += base.fsize;
			filedats.push_back(base);
			
			in.close();
		}
	}

	/*
	 *	Creates a index file.
	 */
	indexfile.open(INDEX_NAME, ios::out | ios::trunc);
	if (indexfile.fail()) {
		errs.push_back((new string("Index file is not created : "))->append(INDEX_NAME));
	}
	time(&now);
	date = localtime(&now);
	indexfile << "Archived file index (";
	sprintf(str0, "Date: %04d/%02d/%02d %02d:%02d",
			date->tm_year + 1900, date->tm_mon + 1,
			date->tm_mday, date->tm_hour, date->tm_min);
	indexfile << str0 << ")" << endl;
	indexfile << "Output: " << outputname << endl;

	size = filedats.size();
	indexfile << endl << "[ " << size << " files are contented ]" << endl;
	for (i = 0;i < (int)size;i++) {
		osstream << filedats.at(i).filename << " (" << filedats.at(i).fsize << ")";
		indexfile << osstream.str() << endl;
		osstream.str("");
	}

	indexfile.close();

	try {

		/*
		 *	Put errors and exit if there are.
		 */
		if (!errs.empty())
			throw errs;

		/*
		 *	Display parsed datas.
		 */
		if (!filedats.empty()) {
			var0 = filedats.size();
			cout << "contents:";
			for (i = 0;i < var0;i++) {
				cout << " " << filedats.at(i).filename;
			}
			cout << endl;
		}
		
		/*
		 *	Make a header and write it to an archived file.
		 *  A default file name is "archive.out"
		 */
		out.open(outputname.c_str(), ios::binary | ios::out | ios::trunc);
		if (out.fail()) 
			throw ("File open error : Archive");
		
		buf = (UCHAR *)malloc(MAX_SIZE);
		if (!buf) 
			throw ("Out of memory : Archive");
		
		/*
		 *  Makes a file header and write it.
		 *	sign: A File Signature
		 *	files: The number of files which is archived.
		 */
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
