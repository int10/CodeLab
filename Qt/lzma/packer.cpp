#include "packer.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include "lzma.h"
//#define NOT_COMPRESS_FILE	//ONLY FOR DEBUG
Packer::Packer(QObject *parent) :
	QObject(parent)
{
	m_fileTab.tab_size=0;
	m_fileTab.file_item_list.clear();
	m_srcBuf = new quint8 [PACKAGE_PER_PAGE_SIZE];
	m_destBuf = new quint8 [PACKAGE_PER_PAGE_SIZE*2];
}


Packer::~Packer()
{
	delete [] m_srcBuf;
	delete [] m_destBuf;
}

PACKER_ERROR_CODE Packer:: PackPath(QString pathName,QString outFileName)
{
	size_t size;
	m_fileTab.tab_size = 0;
	m_fileTab.file_item_list.clear();
	
	m_fileTab.tab_size+=sizeof(size_t);		//for tab size self.
	m_base_path = pathName;

	m_file.setFileName(outFileName);


	if((m_file.open(QIODevice::ReadOnly)))
	{
		if(CheckPackerMark()==true)
		{
			m_file.close();
			qDebug()<<"this file have been packed";
			return PACKER_PACKED;
		}
	
	}
	m_file.close();
		

	if(!(m_file.open(QIODevice::Append) ))
	{
		qDebug()<<"Can't open output file";
		return PACKER_NO_OUTPUT_FILE;
	}
	

	
	m_mark.original_size = m_file.size();
	strcpy_s(m_mark.packer_id,10,LT_PACKER_ID);
	
	GetPathInfo(pathName);

	foreach(FILE_ITEM_INFO info,m_fileTab.file_item_list)
	{
		qDebug()<<info.entry_size<<info.file_size<<info.is_dir<<info.path_name;
	}

	size = FileTab2BlockBuf(m_srcBuf);

	m_srcLen = size;

	if(Lzma::SaveBlockData(&m_file, m_destBuf, &m_destLen, (const quint8 *)m_srcBuf, m_srcLen) == false)
	{
		qDebug()<<"Can't save Data";
		m_file.close();
		return PACKER_DATA_ERROR;
	}
	
	PackContent(&m_file,&m_fileTab);

	AppendPackerMark();

	m_file.close();

	qDebug()<<"-----------------------------------------------------------------";

//	m_fileTab.tab_size = 0;
//	m_fileTab.file_item_list.clear();

//	BlockBuf2FileTab(m_srcBuf);

//	foreach(FILE_ITEM_INFO info,m_fileTab.file_item_list)
//	{
//		qDebug()<<info.entry_size<<info.file_size<<info.is_dir<<info.path_name;
//	}



	return PACKER_NO_ERROR;
}



PACKER_ERROR_CODE Packer::UnPackFile(QString destPathName,QString fileName)
{

	size_t size;
	size_t block_size;
	m_fileTab.tab_size = 0;
	m_fileTab.file_item_list.clear();
	m_base_path = destPathName;
	m_file.setFileName(fileName);
	if(!(m_file.open(QIODevice::ReadOnly) ))
	{
		qDebug()<<"Can't open input file";
		return PACKER_NO_INPUT_FILE;
	}

	if(CheckPackerMark() == false)
	{
		qDebug()<<"Can't find the mark..";
		return PACKER_NO_MARK;
	}
	m_file.seek(m_mark.original_size);


	if(Lzma::RestoreBlockData(&m_file, m_destBuf, &m_destLen, (const quint8 *) m_srcBuf, &m_srcLen) == false)
	{
		qDebug()<<"read block data error";
		m_file.close();
		return PACKER_DATA_ERROR;
	}

	BlockBuf2FileTab(m_destBuf);
//	foreach(FILE_ITEM_INFO info,m_fileTab.file_item_list)
//	{
//		qDebug()<<info.entry_size<<info.file_size<<info.is_dir<<info.path_name;
//	}


	UnPackContent(&m_file, &m_fileTab);


	m_file.close();

	return PACKER_NO_ERROR;
	
}



int Packer::GetPathInfo(QString path)
{

	QDir dir(path);

	dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot|QDir::Hidden);
	dir.setSorting(QDir::DirsFirst);

	QFileInfoList list = dir.entryInfoList();
	if(list.size() == 0)
	{
		return 0;
	}
	foreach(QFileInfo fileinfo,list)
	{
		FILE_ITEM_INFO file_item;
		file_item.file_size = fileinfo.size();
		file_item.is_dir = fileinfo.isDir();
		file_item.path_name = fileinfo.absoluteFilePath();
		file_item.path_name.remove(0,m_base_path.size()+1);		//only save the relate path
		file_item.entry_size = (sizeof(size_t))*2+sizeof(bool)+(file_item.path_name.size()+1);
		m_fileTab.file_item_list.append(file_item);
		m_fileTab.tab_size += file_item.entry_size;
		if(fileinfo.isDir())
		{
			GetPathInfo(m_base_path+"/"+file_item.path_name);
		}
	}
	return 0;

}

size_t Packer::FileTab2BlockBuf(quint8 *buf)
{
	//assume block buf is bigger than file tab size
	quint8 *p = buf;
	//tab size;
	*((size_t *)p) = m_fileTab.tab_size;
	p+= sizeof(size_t);


	//file list 2 buf
	foreach(FILE_ITEM_INFO file_item,m_fileTab.file_item_list)
	{
		p+=FileInfo2Buf(p, &file_item);
	}
	return p-buf;
		
}


void Packer::BlockBuf2FileTab(quint8 *buf)
{
	quint8 *p = buf;
	m_fileTab.tab_size = *((size_t *)p);
	p+= sizeof(size_t);
	while((size_t)(p-buf)<m_fileTab.tab_size)
	{

		FILE_ITEM_INFO file_item;
		p+=Buf2FileInfo(p,&file_item);
		m_fileTab.file_item_list.append(file_item);

		
	}
}

size_t Packer::FileInfo2Buf(quint8 *buf,FILE_ITEM_INFO *file_item)
{
	quint8 *p = buf;


	*((size_t *)p) = file_item->entry_size;
	p+= sizeof(size_t);
	*((size_t *)p) = file_item->file_size;
	p+= sizeof(size_t);
	*((bool *)p) = file_item->is_dir;
	p+=sizeof(bool);
	strcpy_s((char *)p,255,(const char *)file_item->path_name.toLatin1().data());

	//p+=(file_item->path_name.size()+1);
	p=buf+file_item->entry_size;
	return p-buf;
}


size_t Packer::Buf2FileInfo(quint8 *buf,FILE_ITEM_INFO *file_item)
{
	quint8 *p = buf;
	file_item->entry_size = *((size_t *)p);
	p+= sizeof(size_t);
	file_item->file_size =*((size_t *)p);
	p+= sizeof(size_t);
	file_item->is_dir = *((bool *)p);
	p+=sizeof(bool);
	QString temp((char *)p);

	file_item->path_name= temp;
	return file_item->entry_size;

}

int Packer::PackContent(QFile *outFile,FILE_TAB_INFO * fileTab)
{
	FILE_ITEM_INFO file_item;
	QFile cur_file;
	
	size_t buf_left = PACKAGE_PER_PAGE_SIZE,read_size;
	quint8 *p;
	p = m_srcBuf;
	foreach(file_item,fileTab->file_item_list)
	{
		QString cur_path = m_base_path+"/"+file_item.path_name;
		if(!file_item.is_dir)
		{
			qDebug()<<"pack "<<m_base_path+"/"+file_item.path_name;
			cur_file.setFileName(m_base_path+"/"+file_item.path_name);
			cur_file.open(QIODevice::ReadOnly);
			while(1)
			{
				read_size = cur_file.read((char *)p, buf_left);
				buf_left -= read_size;
				p+=read_size;
				if(buf_left == 0)			//buf full...compress and save it to file
				{
					buf_left = PACKAGE_PER_PAGE_SIZE;
					p = m_srcBuf;


					if(Lzma::SaveBlockData(outFile, m_destBuf, &m_destLen, m_srcBuf, PACKAGE_PER_PAGE_SIZE) == false)
					{
						qDebug()<<"Can't save content data";
					}

				}
				else		//buf not full , get next file....
					break;
					
			}
			cur_file.close();
		}
	}
	//save the last data

	if(Lzma::SaveBlockData(outFile, m_destBuf, &m_destLen, m_srcBuf, PACKAGE_PER_PAGE_SIZE-buf_left) == false)
	{
		qDebug()<<"Can't save content data";
		return 0;
	}
	
	return 0;
}


int Packer::UnPackContent(QFile * inFile, FILE_TAB_INFO * fileTab)
{
	FILE_ITEM_INFO file_item;
	QFile cur_file;
	QDir cur_dir;
	

	size_t buf_left = 0,write_size,file_left;
	quint8 *p;
	p = m_destBuf;



	if(Lzma::RestoreBlockData(inFile, m_destBuf, &m_destLen, (const quint8 *) m_srcBuf, &m_srcLen) == false)
	{
		qDebug()<<"read content data error";
		return 0;
	}

	buf_left = m_destLen;

	//creat root dir
	cur_dir.setPath(m_base_path);
	if(!cur_dir.exists())
		cur_dir.mkpath(m_base_path);

	//for(int i = 0;i<(fileTab->file_item_list.count());i++)
	foreach(file_item,fileTab->file_item_list)
	{
//		file_item = fileTab->file_item_list.at(i);

		QString cur_path = m_base_path+"/"+file_item.path_name;
		if(file_item.is_dir)
		{
			cur_dir.mkpath(cur_path);
		}
		else
		{
			qDebug()<<"unpack "<<m_base_path+"/"+file_item.path_name;
			cur_file.setFileName(m_base_path+"/"+file_item.path_name);
			cur_file.open(QIODevice::WriteOnly);
			file_left = file_item.file_size;
			while(1)
			{
				write_size = cur_file.write((char *)p, qMin(file_left,buf_left));

				buf_left-=write_size;
				p+=write_size;
				file_left -= write_size;
				//if((i+1) == fileTab->file_item_list.count()&&file_left == 0)			//last file is unpacked..
				if(file_left == 0)			//write one file end...get next file
				{
					break;
				}
//				qDebug()<<"write size = "<<write_size<<"buf_left="<<buf_left<<"file_left="<<file_left;
				if(buf_left ==0)			//out of buf data ,read next block
				{

					p = m_destBuf;
					if(Lzma::RestoreBlockData(inFile, m_destBuf, &m_destLen, (const quint8 *) m_srcBuf, &m_srcLen) == false)
					{
						qDebug()<<"read content data error";
						return 0;
					}
					buf_left = m_destLen;
				}
				else		//buf still have data ,write next file......
				{
					break;
				}
			}
			cur_file.close();
		}
	}

	return 0;
}

bool Packer::AppendPackerMark()
{
	size_t result ;
	result = m_file.write((char *)&m_mark,sizeof(PACKER_MARK));
	if(result == -1||result <sizeof(PACKER_MARK))
		return false;
	return true;
}

bool Packer::CheckPackerMark()
{
	size_t result;

	if(m_file.size()<sizeof(PACKER_MARK))
		return false;
	
	m_file.seek(m_file.size()-sizeof(PACKER_MARK));
	result = m_file.read((char *) &m_mark, sizeof(PACKER_MARK));
	
	if(result == -1||result<sizeof(PACKER_MARK))
	{
		return false;
	}
	if(strcmp(m_mark.packer_id,LT_PACKER_ID) != 0)
	{
		return false;
	}
	return true;
}


