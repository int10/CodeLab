#ifndef XATTR_FROM_DEBUGFS
#define XATTR_FROM_DEBUGFS

#include <stdio.h>
#include "ext2fs.h"

#include <string>


void internal_dump_inode_extra(FILE *out,
                      //const char *prefix EXT2FS_ATTR((unused)),
                      //ext2_ino_t inode_num EXT2FS_ATTR((unused)),
                      //struct ext2_inode_large *inode)
                      const EXT2_INODE *inode,
                      uint16_t inode_size);

void xattr_fetch_selinux_label(const EXT2_INODE *inode, uint16_t inode_size, std::string* out);

void xattr_fetch_capibilities(const EXT2_INODE *inode, uint16_t inode_size, uint64_t* out);

//void xattr_fetch_capibilities(const EXT2_INODE *inode, uint16_t inode_size, std::string* out);

#endif // XATTR_FROM_DEBUGFS

