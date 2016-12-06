#ifndef PACKER_H
#define PACKER_H

#include <QObject>
#include <QFile>
#define PACKAGE_PER_PAGE_SIZE 		(100*1024*1024)
//#define PACKAGE_PER_PAGE_SIZE 		(1024*1024)

typedef enum
{
	PACKER_NO_ERROR,
	PACKER_NO_INPUT_FILE,
	PACKER_NO_MARK,
	PACKER_DATA_ERROR,
	PACKER_PACKED,
	PACKER_NO_OUTPUT_FILE,
}PACKER_ERROR_CODE;


#define LT_PACKER_ID				"LT_Packer"

typedef struct
{
	size_t size;
	quint8 * block_buf;
}BLOCK_INFO;

typedef struct
{
size_t entry_size;			//this struct size ... the size of path_name is path_name.size();
size_t file_size;
bool is_dir;
QString path_name;
}FILE_ITEM_INFO;

typedef struct
{
	size_t tab_size;
	QList <FILE_ITEM_INFO> file_item_list;
}FILE_TAB_INFO;

typedef struct
{
	char packer_id[10];
	size_t original_size;
}PACKER_MARK ;



class Packer : public QObject
{
	Q_OBJECT
public:
	explicit Packer(QObject *parent = 0);
	~Packer();
	PACKER_ERROR_CODE PackPath(QString path,QString outFileName);	//pack files in this path to a file...
	PACKER_ERROR_CODE UnPackFile(QString destPathName,QString fileName);
private:
	int GetPathInfo(QString path);			//get path info(include sub dir) to file tab info..
	size_t FileTab2BlockBuf(quint8 *buf);	//m_fileTab info to block buf..
	void BlockBuf2FileTab(quint8 *buf);		//get file tab info from block,and place at m_fileTab;
	size_t Buf2FileInfo(quint8 *buf,FILE_ITEM_INFO *file_item);
	size_t FileInfo2Buf(quint8 *buf,FILE_ITEM_INFO *file_item);
	int PackContent(QFile *outFile,FILE_TAB_INFO * fileTab);
	int UnPackContent(QFile *inFile,FILE_TAB_INFO * fileTab);

	bool AppendPackerMark();
	bool CheckPackerMark();
	

	FILE_TAB_INFO m_fileTab;		
	quint8 *m_srcBuf,*m_destBuf;
	size_t m_srcLen,m_destLen;
	QString m_base_path;
	QFile m_file;
	PACKER_MARK m_mark;
	
signals:
	
public slots:
};

#endif // PACKER_H
