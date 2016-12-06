#ifndef FILEEX_H
#define FILEEX_H

#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

class FileEx
{
public:
	typedef enum
	{
		NotOpen = 0x0000,
		ReadOnly = 0x0001,
		WriteOnly = 0x0002,
		ReadWrite = ReadOnly | WriteOnly,
		Append = 0x0004,
		Truncate = 0x0008
	}eFileOpenMode;

	FileEx();
	FileEx(string filename);
	~FileEx();
	void SetFile(string filename);
	bool Open(int mode);
	string Read(long size);
	string ReadAll();
	unsigned long Write(string buf);
	unsigned long Write(const char *buf, long size);
	long Pos();
	int Seek(long offset);
	unsigned long Size();
	bool Exist();
	void Close();
private:
	FILE *m_fh;
	string m_filename;
};

#endif // FILEEX_H
