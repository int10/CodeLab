#include "ext4_utils.h"

/* Function to read the primary superblock */
int read_sb(int fd, struct ext4_super_block *sb)
{
	__int64 ret;

	ret = _lseeki64(fd, 1024, SEEK_SET);
	if (ret < 0){
		fprintf(stderr, "failed to seek to superblock\n");
		return -1;
	}

	ret = _read(fd, sb, sizeof(*sb));
	if (ret < 0){
		fprintf(stderr, "failed to read superblock\n");
		return -1;
	}
	if (ret != sizeof(*sb)){
		fprintf(stderr, "failed to read all of superblock\n");
		return -1;
	}

	return 0;
}

int ext4_parse_sb_info(struct ext4_super_block *sb)
{
	if (sb->s_magic != EXT4_SUPER_MAGIC){
		fprintf(stderr, "superblock magic incorrect\n");
		return -1;
	}

	if ((sb->s_state & EXT4_VALID_FS) != EXT4_VALID_FS){
		fprintf(stderr, "filesystem state not valid\n");
		return -1;
	}

	return 0;
}

int ext4_info(const char *filename)
{
	int fd;
	struct ext4_super_block sb;
	unsigned __int64 blocksize;

	fd = _open(filename, O_RDONLY | O_BINARY);

	if (fd < 0){
		fprintf(stderr, "failed to open filesystem image\n");
		return -1;
	}

	if(read_sb(fd, &sb)){
		_close(fd);
		return -1;
	}

	_close(fd);

	if(ext4_parse_sb_info(&sb)){
		return -1;
	}

	blocksize = 1024 << sb.s_log_block_size;

	printf("info:total size:%llu\n", blocksize*sb.s_blocks_count_lo);
	printf("info:free size:%llu\n", blocksize*sb.s_free_blocks_count_lo);

	return 0;
}

