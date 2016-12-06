#include "FileEx.h"

FileEx::FileEx()
{
	m_filename = "";
	m_fh = NULL;
}

FileEx::FileEx(string filename)
{
	m_filename = filename;
	m_fh = NULL;
}

FileEx::~FileEx()
{
	Close();
}

void FileEx::SetFile(string filename)
{
	if(m_fh) {
		return ;
	}
	m_filename = filename;
}

bool FileEx::Open(int mode)
{
	if(m_fh) {
		return false;
	}
	string strmode;
	if(mode & ReadOnly) {
		if((mode & Append) && (mode & WriteOnly)) {
			strmode = "ab+";
		} else if(mode & WriteOnly) {
			strmode = "rb+";
		} else {
			strmode = "rb";
		}
	} else if(mode & WriteOnly) {
		if(mode & Append) {
			strmode = "ab+";
		} else if (mode & Truncate) {
			strmode = "wb";
		} else {
			strmode = "rb+";
		}
	} else {
		return false;
	}

	m_fh = fopen(m_filename.data(), strmode.data());
	if(!m_fh)
	{
		perror("fopen failed!");
		return false;
	}

	return true;
}

string FileEx::Read(long size)
{
	if(m_fh) {
		char * buf;
		buf = new char[size];
		size = fread(buf, 1, size, m_fh);
		string result(buf, size);
		delete []buf;
		return result;
	} else {
		return "";
	}
}

string FileEx::ReadAll()
{
	if(m_fh) {
		unsigned long size = this->Size();
		return Read(size);
	} else {
		return "";
	}
}

unsigned long FileEx::Write(string buf)
{
	if(m_fh) {
		return fwrite(buf.data(), 1, buf.size(), m_fh);
	} else {
		return 0;
	}
}

unsigned long FileEx::Write(const char *buf, long size)
{
	if(m_fh) {
		return fwrite(buf, 1, size, m_fh);
	} else {
		return 0;
	}
		
}

long FileEx::Pos()
{
	if(m_fh){
		return ftell(m_fh);
	} else {
		return -1;
	}
}

int FileEx::Seek(long offset)
{
	if(m_fh) {
		return fseek(m_fh, offset, SEEK_SET);
	} else {
		return 0;
	}
	
}

unsigned long FileEx::Size()
{
	struct stat statbuff;
	if(stat(m_filename.data(), &statbuff) < 0) {
		return 0;
	} else {
		return statbuff.st_size;
	}
}

bool FileEx::Exist()
{
	if(-1 == access(m_filename.data(), 0)){
		return false;
	} else {
		return true;
	}
}

void FileEx::Close()
{
	if(m_fh) {
		fclose(m_fh);
		m_fh = NULL;
	}
}

