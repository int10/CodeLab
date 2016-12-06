#ifndef LZMA_H
#define LZMA_H

#include <QObject>
#include "lzmalib/LzmaLib.h"
#include <QFile>

class Lzma : public QObject
{
	Q_OBJECT
public:
	explicit Lzma(QObject *parent = 0);
	//下面这四个函数要一对一对地用，比如用Compress压缩数据，就要用UnCompress来解压，不能用RestoreBlockData来解压。。
	static int Compress(quint8 *dest, size_t  *destLen, const quint8 *src, size_t  srcLen);	//压缩函数，dest为压缩后的数据，destLen为压缩后长度，其它两个为压缩前的信息
	static int UnCompress(quint8 *dest, size_t  *destLen, const quint8 *src, size_t  *srcLen);//解压函数，dest为解压后的数据，destLen为解压后长度，其它两个为解压前的信息
	static bool SaveBlockData(QFile *file,quint8 *dest, size_t  *destLen, const quint8 *src, size_t  srcLen);//压缩并保存压缩后的数据在file里。函数说明参见Compress。
	static bool RestoreBlockData(QFile *file,quint8 *dest, size_t  *destLen, const quint8 *src, size_t  *srcLen);//读取file里的数据并解压。。函数说明参见UnCompress。
	~Lzma();
signals:
	
public slots:
	
};

#endif // LZMA_H
