#include "savedir.h"

//SaveDir::SaveDir(QObject *parent) :
//	QObject(parent)
//{
//}

SaveDir::SaveDir(QString &imgfile,QString &outdir,const QString &fs_config_path,const QString &selinux_context_path)
{
	m_in = _open(imgfile.toAscii(), O_RDONLY | O_BINARY);
	if (m_in < 0)
	{
		fprintf(stderr, "Cannot open input file %s\n", imgfile);
		exit(EXIT_CODE_SIMG2IMG_INTPUT_FILE_OPEN_ERROR);
	}

	m_codec = QTextCodec::codecForName("utf-8");

	Get_SparseHeader();
	Get_SuperBlock();
	Get_GroupDesc();

	m_outdir = outdir;
	m_filepath_fs_config = fs_config_path;
	m_filepath_selinux_context = selinux_context_path;
	m_buffer = new char[m_s_header.blk_sz * MUL_BLOCK];


	u64 blocksize;
	blocksize = 1024 << m_super_block.s_log_block_size;
	printf("info:total size:%llu\n", blocksize*m_super_block.s_blocks_count_lo);
	printf("info:free size:%llu\n\n", blocksize*m_super_block.s_free_blocks_count_lo);
	printf("info:total size(KB):%llu\n", blocksize*m_super_block.s_blocks_count_lo/1024);
	printf("info:free size(KB):%llu\n", blocksize*m_super_block.s_free_blocks_count_lo/1024);
}

SaveDir::~SaveDir()
{
	delete [] m_buffer;
	_close(m_in);
}


void SaveDir::Ext4_Info(QString ext4file)
{
	int in;
	in = _open(ext4file.toAscii(), O_RDONLY | O_BINARY);
	if (in < 0)
	{
		fprintf(stderr, "Cannot open input file %s\n", ext4file);
		exit(EXIT_CODE_SIMG2IMG_INTPUT_FILE_OPEN_ERROR);
	}
	ext4_super_block super_block;
	_lseeki64(in, 0x400, SEEK_SET);
	read_all(in, &super_block, sizeof(ext4_super_block));
	u64 blocksize;
	blocksize = 1024 << super_block.s_log_block_size;
	printf("info:total size:%llu\n", blocksize*super_block.s_blocks_count_lo);
	printf("info:free size:%llu\n\n", blocksize*super_block.s_free_blocks_count_lo);
	printf("info:total size(KB):%llu\n", blocksize*super_block.s_blocks_count_lo/1024);
	printf("info:free size(KB):%llu\n", blocksize*super_block.s_free_blocks_count_lo/1024);
	_close(in);
}

//=======================================================
int SaveDir::Count_TrueOffset_(u64 sparse_offset, chunk_header_t *c_header ,u64 *img_offset ,u64 *chunk_left,u32 *fill_val)
{
	return(Count_TrueOffset(sparse_offset, c_header ,img_offset ,chunk_left,fill_val));
}

inline int SaveDir::Count_TrueOffset(u64 sparse_offset, chunk_header_t *c_header ,u64 *img_offset ,u64 *chunk_left,u32 *fill_val)
{
	u64 bytes;
	int chunks;
	*img_offset = sizeof(sparse_header_t);
	*chunk_left = 0;
	chunks = 0;
	int sign;
	sign = 1;
	while(sign)
	{
		chunks ++;
		if(chunks > m_s_header.total_chunks)
			return 1;	//超出img文件范围
		_lseeki64(m_in, *img_offset, SEEK_SET);
		read_all(m_in, c_header, sizeof(chunk_header_t));
		bytes = c_header->chunk_sz * m_s_header.blk_sz;
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
				if(sparse_offset < bytes)
				{
					*chunk_left = bytes - sparse_offset;
					sign = 0;
				}
				sparse_offset = 0;
				break;
			case CHUNK_TYPE_FILL:	//?????????????? fill值未确定
				if(sparse_offset == bytes)
					*img_offset += c_header->total_sz;
				else
				{
					*chunk_left = bytes - sparse_offset;
					_lseeki64(m_in, *img_offset + sizeof(chunk_header_t), SEEK_SET);
					read_all(m_in, fill_val, sizeof(u32));
					sign = 0;
				}
				sparse_offset = 0;
				break;
			case CHUNK_TYPE_DONT_CARE:
				if(sparse_offset == bytes)
					*img_offset += c_header->total_sz;
				else
				{
					*chunk_left = bytes - sparse_offset;
					*fill_val = 0;
					sign = 0;
				}
				sparse_offset = 0;
				break;
			case CHUNK_TYPE_CRC32:	//原工具代码里也没作处理
				printf("CHUNK_TYPE_CRC32: do not handle!\n");
				exit(1);
				break;
			}
		}
	}
	return 0;
}


void SaveDir::Read_ImgFile_(u64 sparse_offset,char *buf,u64 size)
{
	Read_ImgFile(sparse_offset,buf,size);
}

inline void SaveDir::Read_ImgFile(u64 sparse_offset, char *buf, u64 size)
{
	u64 offset,chunk_left;
	chunk_header_t c_header;
	u32 fill_val;
	while(1)
	{
		if(Count_TrueOffset(sparse_offset,&c_header,&offset,&chunk_left,&fill_val))
			break;
		_lseeki64(m_in, offset, SEEK_SET);
		if(CHUNK_TYPE_RAW == c_header.chunk_type)
		{
			if(chunk_left >= size)
			{
				read_all(m_in, buf, size);
				break;
			}
			else
			{
				read_all(m_in, buf, chunk_left);
				buf += chunk_left;
				sparse_offset += chunk_left;
				size -= chunk_left;
			}
		}
		else if(CHUNK_TYPE_FILL == c_header.chunk_type || \
				CHUNK_TYPE_DONT_CARE == c_header.chunk_type)
		{
			if(chunk_left >= size)
			{
				memset(buf,fill_val,size);
				break;
			}
			else
			{
				memset(buf,fill_val,chunk_left);
				buf += chunk_left;
				sparse_offset += chunk_left;
				size -= chunk_left;
			}
		}
		else
			break;
	}
}

//---------------------------------------------------------------------------------
lloff_t SaveDir::extent_binarysearch(EXT4_EXTENT_HEADER *header, lloff_t lbn, bool isallocated)
{
	EXT4_EXTENT *extent;
	EXT4_EXTENT_IDX *index;
	EXT4_EXTENT_HEADER *child;
	lloff_t physical_block = 0;
	lloff_t block;

	if(header->eh_magic != EXT4_EXT_MAGIC)
	{
		LOG("Invalid magic in Extent Header: %X\n", header->eh_magic);
		return 0;
	}
	extent = EXT_FIRST_EXTENT(header);
	//    LOG("HEADER: magic %x Entries: %d depth %d\n", header->eh_magic, header->eh_entries, header->eh_depth);
	if(header->eh_depth == 0)
	{
		for(int i = 0; i < header->eh_entries; i++)
		{
			//          LOG("EXTENT: Block: %d Length: %d LBN: %d\n", extent->ee_block, extent->ee_len, lbn);
			if((lbn >= extent->ee_block) &&
			   (lbn < (extent->ee_block + extent->ee_len)))
			{
				physical_block = ext_to_block(extent) + lbn;
				physical_block = physical_block - (lloff_t)extent->ee_block;
				if(isallocated)
					delete [] header;
				//                LOG("Physical Block: %d\n", physical_block);
				return physical_block;
			}
			extent++; // Pointer increment by size of Extent.
		}
		return 0;
	}

	index = EXT_FIRST_INDEX(header);
	for(int i = 0; i < header->eh_entries; i++)
	{
		//        LOG("INDEX: Block: %d Leaf: %d \n", index->ei_block, index->ei_leaf_lo);
		if(lbn >= index->ei_block)
		{
			child = (EXT4_EXTENT_HEADER *) new char [m_s_header.blk_sz];
			block = idx_to_block(index);
			ext2_readblock(block, (void *) child);

			return extent_binarysearch(child, lbn, true);
		}
	}

	// We reach here if we do not find the key
	if(isallocated)
		delete [] header;

	return physical_block;
}

lloff_t SaveDir::extent_to_logical(EXT2_INODE *ino, lloff_t lbn)
{
	lloff_t block;
	struct ext4_extent_header *header;

	header = get_ext4_header(ino);
	block = extent_binarysearch(header, lbn, false);

	return block;
}

uint32_t SaveDir::fileblock_to_logical(EXT2_INODE *ino, uint32_t lbn)
{
	uint32_t block, indlast, dindlast;
	uint32_t tmpblk, sz;
	uint32_t *indbuffer;
	uint32_t *dindbuffer;
	uint32_t *tindbuffer;

	if(lbn < EXT2_NDIR_BLOCKS)
	{
		return ino->i_block[lbn];
	}

	sz = m_s_header.blk_sz / sizeof(uint32_t);
	indlast = sz + EXT2_NDIR_BLOCKS;
	indbuffer = new uint32_t [sz];
	if((lbn >= EXT2_NDIR_BLOCKS) && (lbn < indlast))
	{
		block = ino->i_block[EXT2_IND_BLOCK];
		ext2_readblock(block, indbuffer);
		lbn -= EXT2_NDIR_BLOCKS;
		block = indbuffer[lbn];
		delete [] indbuffer;
		return block;
	}

	dindlast = (sz * sz) + indlast;
	dindbuffer = new uint32_t [sz];
	if((lbn >= indlast) && (lbn < dindlast))
	{
		block = ino->i_block[EXT2_DIND_BLOCK];
		ext2_readblock(block, dindbuffer);

		tmpblk = lbn - indlast;
		block = dindbuffer[tmpblk/sz];
		ext2_readblock(block, indbuffer);

		lbn = tmpblk % sz;
		block = indbuffer[lbn];

		delete [] dindbuffer;
		delete [] indbuffer;
		return block;
	}

	tindbuffer = new uint32_t [sz];
	if(lbn >= dindlast)
	{
		block = ino->i_block[EXT2_TIND_BLOCK];
		ext2_readblock(block, tindbuffer);

		tmpblk = lbn - dindlast;
		block = tindbuffer[tmpblk/(sz * sz)];
		ext2_readblock(block, dindbuffer);

		block = tmpblk / sz;
		lbn = tmpblk % sz;
		block = dindbuffer[block];
		ext2_readblock(block, indbuffer);
		block = indbuffer[lbn];

		delete [] tindbuffer;
		delete [] dindbuffer;
		delete [] indbuffer;

		return block;
	}

	// We should not reach here
	return 0;
}

//读取一个block数据
int SaveDir::ext2_readblock(lloff_t blocknum, void *buffer)
{
	chunk_header_t c_header;
	u32 fill_val;
	u64 tmp_offset;
	u64 img_offset;
	u64 blocks = 0;
	int chunks = 0;
	img_offset = sizeof(sparse_header_t);
	int sign = 1;
	while(sign)
	{
		chunks ++;
		if(chunks > m_s_header.total_chunks)
			return -1;	//超出img文件范围
		_lseeki64(m_in, img_offset, SEEK_SET);
		read_all(m_in, &c_header, sizeof(chunk_header_t));
		switch(c_header.chunk_type)
		{
		case CHUNK_TYPE_RAW:
			if((blocknum >= blocks) &&
				(blocknum < (c_header.chunk_sz + blocks)))
			{
				tmp_offset = (blocknum - blocks) * m_s_header.blk_sz;
				_lseeki64(m_in, tmp_offset, SEEK_CUR);
				read_all(m_in, buffer, m_s_header.blk_sz);
				sign = 0;
			}
			else
			{
				blocks += c_header.chunk_sz;
				img_offset += c_header.total_sz;
			}
			break;
		case CHUNK_TYPE_FILL:
			if((blocknum >= blocks) &&
				(blocknum < (c_header.chunk_sz + blocks)))
			{
				tmp_offset = sizeof(chunk_header_t);
				_lseeki64(m_in, tmp_offset, SEEK_CUR);
				read_all(m_in, &fill_val, sizeof(fill_val));
				memset(buffer,fill_val,m_s_header.blk_sz);
				sign = 0;
			}
			else
			{
				blocks += c_header.chunk_sz;
				img_offset += c_header.total_sz;
			}
			break;
		case CHUNK_TYPE_DONT_CARE:
			if((blocknum >= blocks) &&
				(blocknum < (c_header.chunk_sz + blocks)))
			{
				fill_val = 0;
				memset(buffer,fill_val,m_s_header.blk_sz);
				sign = 0;
			}
			else
			{
				blocks += c_header.chunk_sz;
				img_offset += c_header.total_sz;
			}
			break;
		case CHUNK_TYPE_CRC32:
			printf("CHUNK_TYPE_CRC32: do not handle!\n");
			return -1;
		}
	}
	return 0;
}

//读取n个block数据，n在某个chunk范围内
int SaveDir::ext2_readblock2(lloff_t blocknum, void *buffer, u64 *readblock)
{
	chunk_header_t c_header;
	u32 fill_val;
	u64 tmp_offset;
	u64 img_offset;
	u64 blocks = 0;
	u64 tmp;
	int chunks = 0;
	img_offset = sizeof(sparse_header_t);
	int sign = 1;
	while(sign)
	{
		chunks ++;
		if(chunks > m_s_header.total_chunks)
			return -1;	//超出img文件范围
		_lseeki64(m_in, img_offset, SEEK_SET);
		read_all(m_in, &c_header, sizeof(chunk_header_t));
		switch(c_header.chunk_type)
		{
		case CHUNK_TYPE_RAW:
			if((blocknum >= blocks) &&
				(blocknum < (c_header.chunk_sz + blocks)))
			{
				tmp = c_header.chunk_sz + blocks;
				tmp -= blocknum;
				*readblock = (tmp > MUL_BLOCK)?MUL_BLOCK:tmp;
				tmp = *readblock;

				tmp_offset = (blocknum - blocks) * m_s_header.blk_sz;
				_lseeki64(m_in, tmp_offset, SEEK_CUR);
				read_all(m_in, buffer, tmp * m_s_header.blk_sz);
				sign = 0;
			}
			else
			{
				blocks += c_header.chunk_sz;
				img_offset += c_header.total_sz;
			}
			break;
		case CHUNK_TYPE_FILL:
			if((blocknum >= blocks) &&
				(blocknum < (c_header.chunk_sz + blocks)))
			{
				tmp = c_header.chunk_sz + blocks;
				tmp -= blocknum;
				*readblock = (tmp > MUL_BLOCK)?MUL_BLOCK:tmp;
				tmp = *readblock;

				tmp_offset = sizeof(chunk_header_t);
				_lseeki64(m_in, tmp_offset, SEEK_CUR);
				read_all(m_in, &fill_val, sizeof(fill_val));
				memset(buffer,fill_val, tmp * m_s_header.blk_sz);
				sign = 0;
			}
			else
			{
				blocks += c_header.chunk_sz;
				img_offset += c_header.total_sz;
			}
			break;
		case CHUNK_TYPE_DONT_CARE:
			if((blocknum >= blocks) &&
				(blocknum < (c_header.chunk_sz + blocks)))
			{
				tmp = c_header.chunk_sz + blocks;
				tmp -= blocknum;
				*readblock = (tmp > MUL_BLOCK)?MUL_BLOCK:tmp;
				tmp = *readblock;

				fill_val = 0;
				memset(buffer,fill_val, tmp * m_s_header.blk_sz);
				sign = 0;
			}
			else
			{
				blocks += c_header.chunk_sz;
				img_offset += c_header.total_sz;
			}
			break;
		case CHUNK_TYPE_CRC32:
			printf("CHUNK_TYPE_CRC32: do not handle!\n");
			return -1;
		}
	}
	return 0;
}

int SaveDir::ReadBlock_ImgFile(EXT2_INODE *ino, lloff_t lbn, void *buf, u64 *readblock)
{
	lloff_t block;

	if(INODE_HAS_EXTENT(ino))
		block = extent_to_logical(ino, lbn);
	else
		block = fileblock_to_logical(ino, lbn);

	if(block == 0)
		return -1;

	return ext2_readblock2(block, buf, readblock);
}

//---------------------------------------------------------------------------------
void SaveDir::Get_SparseHeader()
{
	_lseeki64(m_in, 0, SEEK_SET);
	read_all(m_in, &m_s_header, sizeof(sparse_header_t));
}

void SaveDir::Get_SuperBlock()
{
	u64 offset,chunk_left;
	chunk_header_t c_header;
	u32 fill_val;
	Count_TrueOffset_(0x400,&c_header,&offset,&chunk_left,&fill_val);
	_lseeki64(m_in, offset, SEEK_SET);
	read_all(m_in, &m_super_block, sizeof(ext4_super_block));
}

void SaveDir::Get_GroupDesc()
{
	u64 offset,chunk_left;
	u64 addr;
	chunk_header_t c_header;
	u32 fill_val;
	addr = sizeof(m_super_block) / m_s_header.blk_sz;
	addr += 1;
	addr *= m_s_header.blk_sz;
	Count_TrueOffset_(addr,&c_header,&offset,&chunk_left,&fill_val);
	_lseeki64(m_in, offset, SEEK_SET);
	read_all(m_in, &m_group_desc, sizeof(EXT2_GROUP_DESC));
}


void SaveDir::Get_Inode_(MY_EXT2_INODE *my_inode, int index)
{
	Get_Inode(my_inode, index);
}

inline void SaveDir::Get_Inode(MY_EXT2_INODE *my_inode, int index)
{
	u64 addr;
	if(index < 2)	//前10个inode是被ext2文件系统保留的，其中第2个inode是根目录，第11个inode是lost+found目录
	{//有出现index==0的情况，不能当作异常退出处理
//		printf("Get_Inode() index error.\n");
//		exit(1);
		addr = m_group_desc.bg_inode_table * m_s_header.blk_sz;
		addr += (m_super_block.s_inode_size * 0);
		Read_ImgFile(addr,(char *)&my_inode->inode, sizeof(MY_EXT2_INODE));
	}
	else
	{
		addr = m_group_desc.bg_inode_table * m_s_header.blk_sz;
		addr += (m_super_block.s_inode_size * (index-1));
		Read_ImgFile(addr,(char *)&my_inode->inode, sizeof(MY_EXT2_INODE));
	}
}

//=======================================================
void SaveDir::Explode()
{
	MY_EXT2_INODE *my_inode;
	my_inode = new MY_EXT2_INODE;
	Get_Inode_(my_inode,2);
	//---------------------------------
	bool fs_config = false;

	bool selinux_context = false;

	QString image_path = ""; //this is the name of the root of the file system aka EMPTY
	m_qfile_fs_config = NULL;
	m_qtext_stream_fs_config = NULL;
	m_qfile_selinux_context = NULL;
	m_qtext_stream_selinux_context = NULL;

	if(m_filepath_fs_config != "")
	{
		this->m_qfile_fs_config = new QFile(m_filepath_fs_config);

		this->m_qfile_fs_config->open(QFile::ReadWrite | QFile::Text | QIODevice::Truncate);

		this->m_qtext_stream_fs_config = new QTextStream( this->m_qfile_fs_config);
		//modify 20150924
		this->m_qtext_stream_fs_config->setCodec("utf-8");

		fs_config = true;
	}

	if(m_filepath_selinux_context != "")
	{
		this->m_qfile_selinux_context = new QFile(m_filepath_selinux_context);

		this->m_qfile_selinux_context->open(QFile::ReadWrite | QFile::Text | QIODevice::Truncate);

		this->m_qtext_stream_selinux_context = new QTextStream( this->m_qfile_selinux_context);
		//modify 20150924
		this->m_qtext_stream_selinux_context->setCodec("utf-8");

		selinux_context = true;
	}
	//---------------------------------
	m_size = 0;
	m_size_total = 0;
	m_percentage = 0;
	CountSize(my_inode,m_outdir,image_path,fs_config,selinux_context);	//计算全部文件总大小
	Recursion(my_inode,m_outdir,image_path,fs_config,selinux_context);
	if(0 == m_percentage)
	{
		m_percentage = 100;
		printf("%d%%\n",m_percentage);
	}

	if(m_qtext_stream_selinux_context)	delete	m_qtext_stream_selinux_context;
	if(m_qfile_selinux_context)	delete	m_qfile_selinux_context;
	if(m_qtext_stream_fs_config)	delete	m_qtext_stream_fs_config;
	if(m_qfile_fs_config)	delete	m_qfile_fs_config;
	delete my_inode;
}

void SaveDir::Recursion(MY_EXT2_INODE *my_inode,QString path,const QString &my_path_in_image,bool fs_config,bool selinux_context)
{
	QDir dest_dir;
	QFileInfo fileinfo(path);
	if(fileinfo.isDir() && fileinfo.exists() && QFile::exists(path) && QDir(path).exists()){	//以防相同文件夹名
		QString pathname = fileinfo.path();
		QString filename = fileinfo.fileName();
		for(int i=2; i<256; i++){
			path = pathname + "/" + filename + ".samemark" + QString::number(i);
			QFileInfo fi(path);
			if(!fi.isDir() && !fi.exists() && !QFile::exists(path) && !QDir(path).exists()) break;
		}
	}
	dest_dir.mkpath(path);

	MY_EXT2_INODE *my_inode_next;
	my_inode_next = new MY_EXT2_INODE;

	EXT4_EXTENT_HEADER	*header;
	EXT4_EXTENT			*event;
	u64 block;

	header = (EXT4_EXTENT_HEADER *)my_inode->inode.i_block;
	event = (EXT4_EXTENT *)((char *)header + sizeof(EXT4_EXTENT_HEADER));
	block = (u64)event->ee_start_lo;
	block |= ((u64) event->ee_start_hi << 31) << 1;

	u64 sparse_offset;
	sparse_offset = m_s_header.blk_sz * block;;

	EXT2DIRENT	*dirent;
	dirent = new EXT2DIRENT;
	dirent->dirbuf = NULL;
	dirent->next = NULL;
	dirent->read_bytes = 0;
	dirent->next_block = 0;
	QString	name;
	int sign = 1;
	int ret;
	while(1)
	{
		ret = Read_Dir(dirent,sparse_offset,my_inode,name);
		if(sign)
		{
			sign = 0;
			if(fs_config)
				Fs_Config(my_path_in_image,my_inode);		//fs_config.txt
			if(selinux_context)
				Selinux_Context(my_path_in_image, my_inode);	//selinux_context.txt
		}
		if(ret)		//fs_config.txt，为处理空目录
			break;
		QString my_path_in_image_and_mychild = my_path_in_image;
		my_path_in_image_and_mychild.append(QString::fromAscii("/"));
		my_path_in_image_and_mychild.append(name);

		QString new_path = path;
		new_path.append(QString::fromAscii("/"));
		new_path.append(name);
		Get_Inode(my_inode_next,dirent->next->inode);
		if(EXT2_S_ISDIR(my_inode_next->inode.i_mode))
		{
			Recursion(my_inode_next,new_path,my_path_in_image_and_mychild,fs_config,selinux_context);
		}
		else if(EXT2_S_ISREG(my_inode_next->inode.i_mode))
		{
			m_size += my_inode_next->inode.i_size;
			unsigned int kk;
			kk = 100*((double)m_size/(double)m_size_total);
			if(kk != m_percentage)
			{
				m_percentage = kk;
				printf("%d%%\n",m_percentage);
			}

			QFileInfo fileinfo(new_path);
			if(fileinfo.exists()){	//以防相同文件名
				QString pathname = fileinfo.path();
				QString filename = fileinfo.fileName();
				for(int i=2; i<256; i++){
					new_path = pathname + "/" + filename + ".samemark" + QString::number(i);
					QFileInfo fi(new_path);
					if(!fi.exists()) break;
				}
			}
			Write_Out(my_inode_next,new_path);
			if(fs_config)
				Fs_Config(my_path_in_image_and_mychild,my_inode_next);		//fs_config.txt
			if(selinux_context)
				Selinux_Context(my_path_in_image_and_mychild, my_inode_next);	//selinux_context.txt
		}
		else if(EXT2_S_ISLINK(my_inode_next->inode.i_mode))
		{
			unsigned int len = my_inode_next->inode.i_size;
			if(len + 1 <= sizeof(my_inode_next->inode.i_block))
			{
				QString link((char*)(my_inode_next->inode.i_block));
				unlink(new_path.toAscii().data());
				if(!CreateSymbolicLink(
								new_path.toStdWString().data(),
								link.toStdWString().data(), 0))
					printf("EXT2_S_ISLINK: error!\n");
				if(fs_config)
					Fs_Config(my_path_in_image_and_mychild,my_inode_next);		//fs_config.txt
				if(selinux_context)
					Selinux_Context(my_path_in_image_and_mychild, my_inode_next);	//selinux_context.txt
			}
			else
			{
				//we do not consider this situation now.
				assert(0);
			}
		}
		else
		{
			printf("do not handle.\n");
		}
	}

	if(dirent->dirbuf)
		delete [] dirent->dirbuf;
	delete dirent;
	delete my_inode_next;
}

void SaveDir::CountSize(MY_EXT2_INODE *my_inode,QString path,const QString &my_path_in_image,bool fs_config,bool selinux_context)
{
	MY_EXT2_INODE *my_inode_next;
	my_inode_next = new MY_EXT2_INODE;

	EXT4_EXTENT_HEADER	*header;
	EXT4_EXTENT			*event;
	u64 block;

	header = (EXT4_EXTENT_HEADER *)my_inode->inode.i_block;
	event = (EXT4_EXTENT *)((char *)header + sizeof(EXT4_EXTENT_HEADER));
	block = (u64)event->ee_start_lo;
	block |= ((u64) event->ee_start_hi << 31) << 1;

	u64 sparse_offset;
	sparse_offset = m_s_header.blk_sz * block;;

	EXT2DIRENT	*dirent;
	dirent = new EXT2DIRENT;
	dirent->dirbuf = NULL;
	dirent->next = NULL;
	dirent->read_bytes = 0;
	dirent->next_block = 0;
	QString	name;
	int ret;
	while(1)
	{
		ret = Read_Dir(dirent,sparse_offset,my_inode,name);
		if(ret)		//fs_config.txt，为处理空目录
			break;
		QString my_path_in_image_and_mychild = my_path_in_image;
		my_path_in_image_and_mychild.append(QString::fromAscii("/"));
		my_path_in_image_and_mychild.append(name);

		QString new_path = path;
		new_path.append(QString::fromAscii("/"));
		new_path.append(name);
		Get_Inode(my_inode_next,dirent->next->inode);
		if(EXT2_S_ISDIR(my_inode_next->inode.i_mode))
			CountSize(my_inode_next,new_path,my_path_in_image_and_mychild,fs_config,selinux_context);
		else if(EXT2_S_ISREG(my_inode_next->inode.i_mode))
			m_size_total += my_inode_next->inode.i_size;
	}
	if(dirent->dirbuf)
		delete [] dirent->dirbuf;
	delete dirent;
	delete my_inode_next;
}

inline int SaveDir::Read_Dir(EXT2DIRENT *dirent,u64 offset,MY_EXT2_INODE *my_inode,QString &name)
{
	string filename;
	char *pos;

	if(!dirent)
		return -1;
	if(!dirent->dirbuf)
	{
		dirent->dirbuf = (EXT2_DIR_ENTRY *) new char[m_s_header.blk_sz];
		if(!dirent->dirbuf)
			return -1;
		Read_ImgFile(offset,(char *)dirent->dirbuf,m_s_header.blk_sz);

		dirent->next_block++;
	}

	again:
	if(!dirent->next)
		dirent->next = dirent->dirbuf;
	else
	{
		pos = (char *) dirent->next;
		dirent->next = (EXT2_DIR_ENTRY *)(pos + dirent->next->rec_len);
		if(IS_BUFFER_END(dirent->next, dirent->dirbuf, m_s_header.blk_sz))
		{
			dirent->next = NULL;
			if(dirent->read_bytes < my_inode->inode.i_size)
			{
				offset += (m_s_header.blk_sz * dirent->next_block);
				Read_ImgFile(offset,(char *)dirent->dirbuf,m_s_header.blk_sz);

				dirent->next_block++;
				goto again;
			}
			return -1;
		}
	}

	dirent->read_bytes += dirent->next->rec_len;
	filename.assign(dirent->next->name, dirent->next->name_len);
	if((filename.compare(".") == 0) ||
	   (filename.compare("..") == 0))
        goto again;
	name.clear();
	name.append(m_codec->toUnicode(filename.c_str()));

	return 0;
}

inline void SaveDir::Write_Out(MY_EXT2_INODE *my_inode,QString name)
{
	QFile *filetosave = new QFile(name);
	if (!filetosave->open(QIODevice::ReadWrite | QIODevice::Truncate))
	{
		LOG("Error creating file %s.\n", name);
		return;
	}

	u64 readblock;	//读取数据的block数; m_buffer大小内读取chunk余下的所有block数据
	u64 i,j;
	u64 blocks,blkindex;
	int extra;
	int ret;
	blocks = my_inode->inode.i_size/m_s_header.blk_sz;
	for(blkindex=0; blkindex<blocks; )
	{
		ret = ReadBlock_ImgFile(&(my_inode->inode), blkindex, m_buffer, &readblock);
		if(ret < 0)
		{
			printf("ReadBlock_ImgFile() fail.\n");
			filetosave->close();
			delete filetosave;
			exit(1);
		}
		i = blocks - blkindex;
		j = (i > readblock)?readblock:i;
		blkindex += j;
		filetosave->write(m_buffer, j * m_s_header.blk_sz);
	}

	extra = my_inode->inode.i_size % m_s_header.blk_sz;
	if(extra)
	{
		ret = ReadBlock_ImgFile(&(my_inode->inode), blkindex, m_buffer, &readblock);
		if(ret < 0)
		{
			printf("ReadBlock_ImgFile() fail.\n");
			filetosave->close();
			delete filetosave;
			exit(1);
		}
		filetosave->write(m_buffer,extra);
	}

	filetosave->close();
	delete filetosave;
}


//=========================================================================
extern	QString g_MountPoint;
#define DELIM_CHAR "\t"
//#define DELIM_CHAR " "

inline void SaveDir::Fs_Config(const QString &image_path, const MY_EXT2_INODE *my_inode)
{
	//*this->m_qtext_stream_fs_config << g_MountPoint << image_path << " ";

	if(image_path == "")
	{
		QString root_path ="/";

		*this->m_qtext_stream_fs_config << g_MountPoint << root_path << DELIM_CHAR;
	}
	else
	{
		*this->m_qtext_stream_fs_config << g_MountPoint << image_path << DELIM_CHAR;;
	}

	if (EXT2_S_ISDIR(my_inode->inode.i_mode))
	{
		//*this->m_qtext_stream << "d" << "\t";

		//return true;
	}
	else if (EXT2_S_ISREG(my_inode->inode.i_mode))
	{
		//*this->m_qtext_stream << "r" << "\t";
		//return true;
	}
	else if (EXT2_S_ISLINK(my_inode->inode.i_mode))
	{
		//*this->m_qtext_stream << "l" << "\t";
		//return true;
	}
	else
	{
		return;
	}

	//uint32_t	i_mtime;		/* Modification time */

	if(0)
	{
		*this->m_qtext_stream_fs_config << my_inode->inode.i_mtime << "\t";
	}

	//*this->m_qtext_stream << file->inode.i_ctime << "\t";

	//*this->m_qtext_stream << file->inode.i_atime << "\t";

	//*this->m_qtext_stream << file->inode.i_dtime << "\t";

	//uint16_t	i_uid;		/* Low 16 bits of Owner Uid */

	this->m_qtext_stream_fs_config->setIntegerBase(10);

	*this->m_qtext_stream_fs_config << my_inode->inode.i_uid << DELIM_CHAR;

	//uint16_t	i_gid;		/* Low 16 bits of Group Id */
	*this->m_qtext_stream_fs_config << my_inode->inode.i_gid << DELIM_CHAR;

	this->m_qtext_stream_fs_config->setIntegerBase(8);

	//uint16_t	i_mode;		/* File mode */
	*this->m_qtext_stream_fs_config << "0" << (my_inode->inode.i_mode & 0x1FF) << DELIM_CHAR;

//	char buffer[256];

	uint16_t inode_size = m_super_block.s_inode_size;	//file->partition->inode_size;

	uint64_t capability = 0UL;

	//std::string capibility = "0";

	xattr_fetch_capibilities(
		&my_inode->inode,
		inode_size,
		&capability);
//	xattr_fetch_capibilities(
//		&(file->inode),
//		inode_size,
//		&capability);

	//QString qcapibility = capibility.c_str();

	char buf[256] = {0};

	sprintf(buf, "%llu", capability);

	*this->m_qtext_stream_fs_config << "capabilities=" << buf << endl;
}


inline void SaveDir::Selinux_Context(const QString &image_path, const MY_EXT2_INODE *my_inode)
{
//    char buffer[256];

	uint16_t inode_size = m_super_block.s_inode_size;

	std::string s = "";

	xattr_fetch_selinux_label(
		&my_inode->inode,
		inode_size,
		&s);

	if(s != "")
	{
		//QString path = g_MountPoint + image_path;
		QString path = image_path;

		if(path == "")
		{
			path = "/";
		}

		path = g_MountPoint + path;

		//对path进行转义

		//这个要放在第一个哦
		path.replace("\\","\\\\");

		//我们没有支持全部

		//https://msdn.microsoft.com/zh-cn/library/ae5bf541%28v=vs.90%29.aspx

		path.replace(QString("+"), QString("\\+"));
		path.replace("-","\\-");
		path.replace("^","\\^");
		path.replace("$","\\$");
		path.replace("*","\\*");
		path.replace("?","\\?");
		path.replace("{","\\{");
		path.replace("}","\\}");
		path.replace(".","\\.");
		path.replace("(","\\(");
		path.replace(")","\\)");
		path.replace(":","\\:");
		path.replace("=","\\=");
		path.replace("!","\\!");
		path.replace("|","\\|");
		path.replace("[","\\[");
		path.replace("]","\\]");

		//*this->m_qtext_stream_selinux_context << "/system" << image_path << " ";

		*this->m_qtext_stream_selinux_context << path << DELIM_CHAR;

		*this->m_qtext_stream_selinux_context << s.c_str() << endl;
	}

	return;
}

