#include "lzma.h"
#include "lzmalib/LzmaLib.h"
#include <QDebug>

#define HANDLE_COMPRESS_EOF_ERROR
Lzma::Lzma(QObject *parent) :
	QObject(parent)
{

}
Lzma::~Lzma()
{

}
int Lzma::Compress(quint8 *dest, size_t  *destLen, const quint8 *src, size_t  srcLen)
{

	//dest buf context order:
	//original size (sizeof(size_t))
	//outPropsSize size (sizeof(size_t))
	//outProps size (sizeof(outProps))
	
	quint8 outProps[5],*p;
	size_t outPropsSize = 5;
	int result;
	int save_buf_size = sizeof(outProps)+(sizeof(size_t)*2);
	*destLen = srcLen;		//这个函数应该是有bug，如果把*destLen设成0的话，会出错，网上人认为，把这个值设到跟srcLen一样，是比较靠谱的。。
	//在前面预留一些空间，用来放srcLen,outProps和outPropsSize;
	result = LzmaCompress(dest+save_buf_size, destLen, src, srcLen, outProps, &outPropsSize, 9, (1<<24), 3, 0, 2, 32, 2);
	p = dest;
	*((size_t *)p) = srcLen;
	p+=sizeof(size_t);
	*((size_t *)p) = outPropsSize;
	p+=sizeof(size_t);
	for(int i = 0;i<sizeof(outProps);i++)
	{
		*(p+i)=outProps[i];
	}
	*destLen+=save_buf_size;

	return result;
}

int Lzma::UnCompress(quint8 *dest, size_t  *destLen, const quint8 *src, size_t  *srcLen)
{
	quint8 outProps[5],*p;
	size_t outPropsSize;
	int result;
	int save_buf_size = sizeof(outProps)+(sizeof(size_t)*2);
	//读取放在buf前面的srcLen,outProps和outPropsSize;
	p = (quint8 *)src;

	*destLen =  *((size_t *)p) ;  //这个函数应该是有bug，如果把*destLen设成0的话，会出错，把它设成数据压缩前的长度，就不会出错，这个应该靠谱的。。
	p+=sizeof(size_t);
	outPropsSize = *((size_t *)p) ;
	p+=sizeof(size_t);
	for(int i = 0;i<sizeof(outProps);i++)
	{
		outProps[i] = *(p+i);
	}
	*srcLen-=save_buf_size;
	
	result = LzmaUncompress(dest, destLen,  src+=save_buf_size,  srcLen, outProps, outPropsSize);
	return result;
}


bool Lzma::SaveBlockData(QFile *file,quint8 *dest, size_t  *destLen, const quint8 *src, size_t  srcLen)
{
	qint64 result_size,result_buf;
	size_t size;
#ifdef HANDLE_COMPRESS_EOF_ERROR
	bool isCompress ;
	int result  = Lzma::Compress(dest, destLen, src, srcLen);
	if(result ==SZ_OK)
	{
		isCompress = true;
		if((result_size = file->write((const char *)&isCompress,sizeof(bool))) == -1)		//write isCompress
			return false;
		size = *destLen;
		if((result_size = file->write((const char *)&size,sizeof(size_t))) == -1)		//write block size;
			return false;
		if((result_buf=file->write((const char *)dest, *destLen)) == -1)			//write block data;
			return false;
		return true;

	}
	else if(result == SZ_ERROR_OUTPUT_EOF)
	{
		isCompress = false;
		if((result_size = file->write((const char *)&isCompress,sizeof(bool))) == -1)		//write isCompress
			return false;
		size = srcLen;
		if((result_size = file->write((const char *)&size,sizeof(size_t))) == -1)		//write block size;
			return false;
		if((result_buf=file->write((const char *)src, srcLen)) == -1)			//write block data;
			return false;
		return true;
	}
	else
		return false;
#else
	if(Lzma::Compress(dest, destLen, src, srcLen)!=SZ_OK)
	{
		qDebug()<<"Compress error";
		return false;

	}

	size = *destLen;
	if((result_size = file->write((const char *)&size,sizeof(size_t))) == -1)		//write block size;
		return false;
		
	if((result_buf=file->write((const char *)dest, *destLen)) == -1)			//write block data;
		return false;
	return true;
#endif
}

bool Lzma::RestoreBlockData(QFile *file,quint8 *dest, size_t  *destLen, const quint8 *src, size_t  *srcLen)
{

	size_t block_size;
	qint64 result;
#ifdef HANDLE_COMPRESS_EOF_ERROR

	bool isCompress;
	result = file->read((char *)&isCompress, sizeof(bool));//get
	if(result==-1||result<sizeof(bool))
		return  false;
	
	result = file->read((char *)&block_size, sizeof(size_t));//get block size;

	if(result==-1||result<sizeof(size_t))
		return  false;

	if(isCompress == true)
	{

		result = file->read((char *)src,block_size);
		if(result==-1||result <block_size)
			return false;
		*srcLen = block_size;
		if((UnCompress(dest, destLen, (const quint8 *) src, srcLen))!=SZ_OK)
		{
			qDebug()<<"UnCompress error";
			return false;
		}
		return true;	

		
	}
	else
	{
		result = file->read((char *)dest,block_size);
		if(result==-1||result <block_size)
			return false;
		*destLen = block_size;
		return true;
	}
	return false;


	
#else
	result = file->read((char *)&block_size, sizeof(size_t));//get block size;

	if(result==-1||result<sizeof(size_t))
		return  false;

	result = file->read((char *)src,block_size);
	if(result==-1||result <block_size)
		return false;
	*srcLen = block_size;
	if((UnCompress(dest, destLen, (const quint8 *) src, srcLen))!=SZ_OK)
	{
		qDebug()<<"UnCompress error";
		return false;
	}
	return true;	
#endif
}





