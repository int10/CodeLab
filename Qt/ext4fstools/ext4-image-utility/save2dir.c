#include <io.h>
#include "sparse_format.h"
#include "sparse_file.h"
#include "ext2fs.h"

#ifndef	NULL
#ifdef	__cplusplus
#define	NULL 0
#else
#define	NULL ((void*)0)
#endif
#endif

extern	int read_all(int fd, void *buf, size_t len);

int InFile;
struct sparse_file *SparseFile = NULL;
sparse_header_t		SparseHeader;
//chunk_header_t		ChunkHeader;
EXT2_SUPER_BLOCK	SuperBlock;
EXT2_GROUP_DESC		GroupDesc;
EXT2_INODE			Inode;
__int64	InodeOffset;
EXT2_DIR_ENTRY		Entry;
EXT4_EXTENT			Extent;
EXT4_EXTENT_IDX		ExtentIdx;
EXT4_EXTENT_HEADER	ExtentHeader;

void Get_SparseHeader(sparse_header_t *S_Header)
{
	_lseek(InFile, 0, SEEK_SET);
	read_all(InFile, S_Header, sizeof(sparse_header_t));
}

void Count_TrueOffset(__int64 sparse_offset, chunk_header_t *c_header ,__int64 *img_offset ,__int64 *chunk_left)
{
	sparse_header_t s_header;
	__int64 bytes;
	int chunks;
	Get_SparseHeader(&s_header);
	*img_offset = sizeof(sparse_header_t);
	*chunk_left = 0;
	chunks = 0;
	while(sparse_offset)
	{
		chunks ++;
		if(chunks > s_header.total_chunks)
			break;
		_lseek(InFile, *img_offset, SEEK_SET);
		read_all(InFile, c_header, sizeof(chunk_header_t));
		bytes = c_header->chunk_sz * s_header.blk_sz;
		if(sparse_offset > bytes)
		{
			switch(c_header->chunk_type)
			{
			case CHUNK_TYPE_RAW:
			case CHUNK_TYPE_FILL:
			case CHUNK_TYPE_DONT_CARE:
				*img_offset += c_header->total_sz;
				sparse_offset -= bytes;
				break;
			case CHUNK_TYPE_CRC32:
				printf("CHUNK_TYPE_CRC32: do not handle!\n");
				exit(1);
				break;
			}
		}
		else
		{
			switch(c_header->chunk_type)
			{
			case CHUNK_TYPE_RAW:
				*img_offset += sizeof(chunk_header_t);
				*img_offset += sparse_offset;
				if(sparse_offset == bytes)
				{
					_lseek(InFile, *img_offset, SEEK_SET);
					read_all(InFile, c_header, sizeof(chunk_header_t));
					*img_offset += sizeof(chunk_header_t);
					*chunk_left = c_header->total_sz - sizeof(chunk_header_t);
				}
				else
					*chunk_left = bytes - sparse_offset;
				sparse_offset = 0;
				break;
			case CHUNK_TYPE_FILL:	//?????????????? fill值未确定
				*img_offset += sizeof(chunk_header_t);
				*img_offset += sparse_offset;
				if(sparse_offset == bytes)
				{
					_lseek(InFile, *img_offset, SEEK_SET);
					read_all(InFile, c_header, sizeof(chunk_header_t));
					*img_offset += sizeof(chunk_header_t);
					*chunk_left = c_header->total_sz - sizeof(chunk_header_t);
				}
				else
					*chunk_left = 0;
				sparse_offset = 0;
				break;
			case CHUNK_TYPE_DONT_CARE:
				printf("CHUNK_TYPE_DONT_CARE: do not handle!\n");
				exit(1);
				break;
			case CHUNK_TYPE_CRC32:
				printf("CHUNK_TYPE_CRC32: do not handle!\n");
				exit(1);
				break;
			}
		}
	}
}

void Read_ImgFile(__int64 sparse_offset,char *buf,__int64 size)
{
	__int64 offset,chunk_left;
	chunk_header_t c_header;
	Count_TrueOffset(sparse_offset,&c_header,&offset,&chunk_left);
	_lseek(InFile, offset, SEEK_SET);
	read_all(InFile, buf, size);
}
//-----------------------------------------------------------
void Get_SuperBlock(EXT2_SUPER_BLOCK *super_block)
{
//	__int64 offset;
//	offset = 1024;
//	offset += sizeof(sparse_header_t);
//	offset += sizeof(chunk_header_t);
//	if(1024 == SparseHeader.blk_sz)
//		offset += sizeof(chunk_header_t);
//	_lseek(InFile, offset, SEEK_SET);
//	read_all(InFile, &SuperBlock, sizeof(EXT2_SUPER_BLOCK));
//	printf("super block size: %d\n", sizeof(EXT2_SUPER_BLOCK));

	__int64 offset,chunk_left;
	chunk_header_t c_header;
	Count_TrueOffset(0x400,&c_header,&offset,&chunk_left);
	_lseek(InFile, offset, SEEK_SET);
	read_all(InFile, super_block, sizeof(EXT2_SUPER_BLOCK));
}

void Get_GroupDesc(EXT2_GROUP_DESC *group_desc)
{
//	__int64 offset, tmp;
//	offset = 0;
//	if(1024 == SparseHeader.blk_sz)
//	{
//		offset += 1024;
//		offset += sizeof(EXT2_SUPER_BLOCK);
//	}
//	else
//		offset += SparseHeader.blk_sz;
//	offset += sizeof(sparse_header_t);
//	tmp = offset;
//	while(1)
//	{
//		offset += sizeof(chunk_header_t);
//		if(tmp < SparseHeader.blk_sz)
//			break;
//		tmp -= SparseHeader.blk_sz;
//	}
//	_lseek(InFile, offset, SEEK_SET);
//	read_all(InFile, &GroupDesc, sizeof(EXT2_GROUP_DESC));
//	printf("GroupDesc: %d\n", sizeof(EXT2_GROUP_DESC));

	__int64 offset,chunk_left;
	__int64 addr;
	chunk_header_t c_header;
	addr = 0x400;
	Count_TrueOffset(0x400,&c_header,&offset,&chunk_left);
	addr += chunk_left;
	Count_TrueOffset(addr,&c_header,&offset,&chunk_left);
	_lseek(InFile, offset, SEEK_SET);
	read_all(InFile, group_desc, sizeof(EXT2_GROUP_DESC));
}

void Get_Inode(EXT2_INODE *inode, int index)
{
	sparse_header_t s_header;
	EXT2_SUPER_BLOCK super_block;
	EXT2_GROUP_DESC group_desc;
	__int64 addr,offset,chunk_left;
	chunk_header_t c_header;
	if(index < 2)
	{
		printf("Get_Inode() index error.\n");
		exit(1);
	}
	Get_SparseHeader(&s_header);
	Get_SuperBlock(&super_block);
	Get_GroupDesc(&group_desc);
	addr = group_desc.bg_inode_table * s_header.blk_sz;
	addr += (super_block.s_inode_size * (index-1));
	Count_TrueOffset(addr,&c_header,&offset,&chunk_left);
	_lseek(InFile, offset, SEEK_SET);
	read_all(InFile, inode, sizeof(EXT2_INODE));
}

//void Read_Dir(EXT2DIRENT *dirent)
//{
//	;
//}

typedef struct{
	char *path_name;
	EXT2_INODE *inode;
}DIR_INFO;

void Copy_Folder(DIR_INFO *info)
{
	EXT2_DIR_ENTRY *dirbuf, *dirnext;
	__int64 addr;
	EXT4_EXTENT_HEADER	header;
	EXT4_EXTENT			event;
	char *path_name_next;
	EXT2_INODE *inode_next;
	path_name_next = (char *) calloc(1,1024);
	inode_next = (EXT2_INODE *) calloc(1,sizeof(EXT2_INODE));
	memcpy(path_name_next,info->path_name,strlen(info->path_name)+1);

	memcpy(&header,info->inode->i_block,sizeof(EXT4_EXTENT_HEADER));
	memcpy(&event,(char *)info->inode->i_block + sizeof(EXT4_EXTENT_HEADER), sizeof(EXT4_EXTENT));
	addr = (__int64)event.ee_start_lo;
	addr |= (((__int64)event.ee_start_hi << 31)<<1);
	addr *= SparseHeader.blk_sz;
	dirbuf = calloc(1,SparseHeader.blk_sz);
	Read_ImgFile(addr,dirbuf,SparseHeader.blk_sz);
	dirnext = dirbuf;

	free(dirbuf);
	free(inode_next);
	free(path_name_next);
}
//==========================================================
void Save2Dir(char *path_name)
{
	DIR_INFO dir_info;

	__int64 kk,mm;
	chunk_header_t c_header;
	Count_TrueOffset(0x618e94,&c_header,&kk,&mm);	//test

	Get_SparseHeader(&SparseHeader);
	Get_SuperBlock(&SuperBlock);
	Get_GroupDesc(&GroupDesc);
	Get_Inode(&Inode,2);
	//------------------------------
	dir_info.inode = &Inode;
	dir_info.path_name = path_name;
	Copy_Folder(&dir_info);

	printf("InFile = %d\n", InFile);
	printf("XXXXXXXXXXXXX: %s\n", path_name);


	//------------------------------
	sparse_file_destroy(SparseFile);
	SparseFile = NULL;
	_close(InFile);
}

