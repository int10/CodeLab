#ifndef SAVEDIR_H
#define SAVEDIR_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <io.h>
#include "sparse_format.h"
#include "ext2fs.h"
#include "fcntl.h"
#include "exit_code.h"
#include "ext2read.h"
#include "xattr_from_debugfs.h"
#include "ext4_utils.h"
#include <assert.h>
#include <QDebug>
#include <QTextCodec>

extern	"C"	int read_all(int fd, void *buf, size_t len);


#define		MUL_BLOCK	50000


typedef union{
	EXT2_INODE  inode;
	char		block [4096]; //hack for thing like xattr after normal inode
}MY_EXT2_INODE;


class SaveDir : public QObject
{
	Q_OBJECT
public:
//	explicit SaveDir(QObject *parent = 0);
	SaveDir(QString &imgfile,QString &outdir,
			const QString &fs_config_path,
			const QString &selinux_context_path);
	~SaveDir();

public:
	void Explode();
	void Recursion(MY_EXT2_INODE *my_inode,QString path,const QString &my_path_in_image,bool fs_config,bool selinux_context);
	void CountSize(MY_EXT2_INODE *my_inode,QString path,const QString &my_path_in_image,bool fs_config,bool selinux_context);
	static void Ext4_Info(QString ext4file);

signals:

public slots:

private:
	int Count_TrueOffset_(u64 sparse_offset, chunk_header_t *c_header ,u64 *img_offset ,u64 *chunk_left,u32 *fill_val);
	int Count_TrueOffset(u64 sparse_offset, chunk_header_t *c_header ,u64 *img_offset ,u64 *chunk_left,u32 *fill_val);
	void Read_ImgFile_(u64 sparse_offset,char *buf,u64 size);
	void Read_ImgFile(u64 sparse_offset,char *buf,u64 size);
	void Get_SparseHeader();
	void Get_SuperBlock();
	void Get_GroupDesc();
	void Get_Inode_(MY_EXT2_INODE *my_inode, int index);
	void Get_Inode(MY_EXT2_INODE *my_inode, int index);
	int Read_Dir(EXT2DIRENT *dirent,u64 offset,MY_EXT2_INODE *my_inode,QString &name);
	void Write_Out(MY_EXT2_INODE *my_inode,QString name);

private:
	lloff_t extent_binarysearch(EXT4_EXTENT_HEADER *header, lloff_t lbn, bool isallocated);
	lloff_t extent_to_logical(EXT2_INODE *ino, lloff_t lbn);
	uint32_t fileblock_to_logical(EXT2_INODE *ino, uint32_t lbn);
	int ext2_readblock(lloff_t blocknum, void *buffer);
	int ext2_readblock2(lloff_t blocknum, void *buffer, u64 *readblock);
	int  ReadBlock_ImgFile(EXT2_INODE *ino, lloff_t lbn, void *buf, u64 *readblock);

private:
	void Fs_Config(const QString &image_path,const MY_EXT2_INODE *my_inode);
	void Selinux_Context(const QString &image_path, const MY_EXT2_INODE *my_inode);
private:
	//for fs_config
	QString m_filepath_fs_config;
	QFile *m_qfile_fs_config;
	QTextStream *m_qtext_stream_fs_config;
	//for capibilities
	QString m_filepath_selinux_context;
	QFile *m_qfile_selinux_context;
	QTextStream *m_qtext_stream_selinux_context;

private:
	int m_in;
	sparse_header_t m_s_header;
	ext4_super_block m_super_block;
	EXT2_GROUP_DESC m_group_desc;
	QString m_outdir;
	QTextCodec *m_codec;

private:
	unsigned int m_size;
	unsigned int m_size_total;
	unsigned int m_percentage;
	char *m_buffer;
};

#endif // SAVEDIR_H

