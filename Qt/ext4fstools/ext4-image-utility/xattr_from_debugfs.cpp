#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "xattr_from_debugfs.h"

#include <iostream>
#include <string>
#include <assert.h>

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;

struct ext2_ext_attr_header {
    __u32	h_magic;	/* magic number for identification */
    __u32	h_refcount;	/* reference count */
    __u32	h_blocks;	/* number of disk blocks used */
    __u32	h_hash;		/* hash value of all attributes */
    __u32	h_reserved[4];	/* zero right now */
};

struct ext2_ext_attr_entry {
    __u8	e_name_len;	/* length of name */
    __u8	e_name_index;	/* attribute name index */
    __u16	e_value_offs;	/* offset in disk block of value */
    __u32	e_value_block;	/* disk block attribute is stored on (n/i) */
    __u32	e_value_size;	/* size of attribute value */
    __u32	e_hash;		/* hash value of name and value */
#if 0
    char	e_name[0];	/* attribute name */
#endif
};

#define EXT2_EXT_ATTR_MAGIC		0xEA020000
#define EXT2_EXT_ATTR_PAD_BITS		2
#define EXT2_EXT_ATTR_PAD		((unsigned) 1<<EXT2_EXT_ATTR_PAD_BITS)
#define EXT2_EXT_ATTR_ROUND		(EXT2_EXT_ATTR_PAD-1)
#define EXT2_EXT_ATTR_LEN(name_len) \
    (((name_len) + EXT2_EXT_ATTR_ROUND + \
    sizeof(struct ext2_ext_attr_entry)) & ~EXT2_EXT_ATTR_ROUND)
#define EXT2_EXT_ATTR_NEXT(entry) \
    ( (struct ext2_ext_attr_entry *)( \
      (char *)(entry) + EXT2_EXT_ATTR_LEN((entry)->e_name_len)) )
#define EXT2_EXT_ATTR_SIZE(size) \
    (((size) + EXT2_EXT_ATTR_ROUND) & ~EXT2_EXT_ATTR_ROUND)
#define EXT2_EXT_IS_LAST_ENTRY(entry) (*((__u32 *)(entry)) == 0UL)
#define EXT2_EXT_ATTR_NAME(entry) \
    (((char *) (entry)) + sizeof(struct ext2_ext_attr_entry))
#define EXT2_XATTR_LEN(name_len) \
    (((name_len) + EXT2_EXT_ATTR_ROUND + \
    sizeof(struct ext2_xattr_entry)) & ~EXT2_EXT_ATTR_ROUND)
#define EXT2_XATTR_SIZE(size) \
    (((size) + EXT2_EXT_ATTR_ROUND) & ~EXT2_EXT_ATTR_ROUND)

char isprint (unsigned char c)
{
    if ( c >= 0x20 && c <= 0x7e )
        return 1;
    return 0;
}


static void dump_xattr_string(FILE *out, char* from, const char *str, int len)
{
    int i;

    fprintf(stderr, "\ndump_xattr_string start from %s\n", from);

    for (i = 0; i < len; i++)
    {
        fprintf(stderr, " *** %02x ", (unsigned char)str[i]);
    }

    fprintf(stderr, "\ndump_xattr_string end from %s\n", from);
}

static void dump_xattr_string_2(FILE *out, const char *str, int len)
{
    int printable = 0;
    int i;

    /* check: is string "printable enough?" */
    for (i = 0; i < len; i++)
    {
        fprintf(stderr, " *** %02x ", (unsigned char)str[i]);

        if (isprint(str[i]))
        {
            printable++;
        }
    }

    if (printable <= len*7/8)
        printable = 0;

    for (i = 0; i < len; i++)
        if (printable)
            fprintf(out, isprint(str[i]) ? "%c" : "\\%03o",
                (unsigned char)str[i]);
        else
            fprintf(out, "%02x ", (unsigned char)str[i]);
}


void xattr_fetch_selinux_label(const EXT2_INODE *inode, uint16_t inode_size, std::string* out)
{
	struct ext2_ext_attr_entry *entry;
    __u32 *magic;
    char *start, *end;
    unsigned int storage_size;

    if (inode->i_extra_isize > inode_size - EXT2_GOOD_OLD_INODE_SIZE)
    {
        fprintf(stderr, "invalid inode->i_extra_isize (%u)\n",
                inode->i_extra_isize);
        return;
    }

    //storage_size = EXT2_INODE_SIZE(current_fs->super) -
    storage_size = (unsigned int)inode_size -
            (unsigned int)EXT2_GOOD_OLD_INODE_SIZE -
            (unsigned int)inode->i_extra_isize;

    magic = (__u32 *)((char *)inode + EXT2_GOOD_OLD_INODE_SIZE +
            inode->i_extra_isize);

    if (*magic == EXT2_EXT_ATTR_MAGIC)
    {
        end = (char *) inode + inode_size;

        start = (char *) magic + sizeof(__u32);

        entry = (struct ext2_ext_attr_entry *) start;

        while (!EXT2_EXT_IS_LAST_ENTRY(entry))
        {
            struct ext2_ext_attr_entry *next = EXT2_EXT_ATTR_NEXT(entry);

            if (entry->e_value_size > storage_size || (char *) next >= end)
            {
                //fprintf(out, "invalid EA entry in inode\n");
                return;
            }

            //fprintf(out, "  ");

            //dump_xattr_string(out, EXT2_EXT_ATTR_NAME(entry), entry->e_name_len);

			std::string name = EXT2_EXT_ATTR_NAME(entry);

			if (name == "selinux")
			{
				*out = start + entry->e_value_offs;
			}

            //fprintf(out, " = \"");

            //dump_xattr_string(out, start + entry->e_value_offs, entry->e_value_size);

            //fprintf(out, "\" (%u)\n", entry->e_value_size);

            entry = next;
        }
    }
}

#pragma pack(1)

struct vfs_cap_data {

    uint32_t magic_etc;            /* Little endian */

    struct {

        uint32_t permitted;    /* Little endian */
        uint32_t inheritable;  /* Little endian */

    } data[2];

 };

#pragma pack()


#define VFS_CAP_REVISION_2      0x02000000

#define VFS_CAP_REVISION        VFS_CAP_REVISION_2

#define VFS_CAP_FLAGS_EFFECTIVE 0x000001

int make_vfs_cap_data(uint64_t capabilities)
{
    if (capabilities == 0)
    {
        return 0;
    }

    struct vfs_cap_data cap_data;

    memset(&cap_data, 0, sizeof(cap_data));

    cap_data.magic_etc = VFS_CAP_REVISION | VFS_CAP_FLAGS_EFFECTIVE;
    cap_data.data[0].permitted = (uint32_t) (capabilities & 0xffffffff);
    cap_data.data[0].inheritable = 0;
    cap_data.data[1].permitted = (uint32_t) (capabilities >> 32);
    cap_data.data[1].inheritable = 0;

    //fprintf(stderr, "cap_data.data[0].permitted : %u\n", cap_data.data[0].permitted);
    //fprintf(stderr, "cap_data.data[1].permitted : %u\n", cap_data.data[1].permitted);

    if(0)
    {
        dump_xattr_string(
            stderr,
            "make_vfs_cap_data",
            (char*)&cap_data,
            20);
    }

}


uint64_t get_capabilities(struct vfs_cap_data* vfs_cap_data)
{

    if(0)
    {
        dump_xattr_string(
            stderr,
            "get_capabilities",
            (char*)vfs_cap_data,
            20);
    }

    uint64_t capabilities = 0;

    capabilities = vfs_cap_data->data[1].permitted;

    //fprintf(stderr, "vfs_cap_data->data[1].permitted: %x\n", vfs_cap_data->data[1].permitted);

    capabilities <<= 32;

    capabilities += vfs_cap_data->data[0].permitted;

    //fprintf(stderr, "vfs_cap_data->data[1].permitted: %x\n", vfs_cap_data->data[0].permitted);

    return capabilities;
}



void xattr_fetch_capibilities(const EXT2_INODE *inode, uint16_t inode_size, uint64_t* out)
{
    struct ext2_ext_attr_entry *entry;
    __u32 *magic;
    char *start, *end;
    unsigned int storage_size;

    if (inode->i_extra_isize > inode_size - EXT2_GOOD_OLD_INODE_SIZE)
    {
        fprintf(stderr, "invalid inode->i_extra_isize (%u)\n",
                inode->i_extra_isize);
        return;
    }

    //storage_size = EXT2_INODE_SIZE(current_fs->super) -
    storage_size = (unsigned int)inode_size -
            (unsigned int)EXT2_GOOD_OLD_INODE_SIZE -
            (unsigned int)inode->i_extra_isize;

    magic = (__u32 *)((char *)inode + EXT2_GOOD_OLD_INODE_SIZE +
            inode->i_extra_isize);

    if (*magic == EXT2_EXT_ATTR_MAGIC)
    {
        end = (char *) inode + inode_size;

        start = (char *) magic + sizeof(__u32);

        entry = (struct ext2_ext_attr_entry *) start;

        while (!EXT2_EXT_IS_LAST_ENTRY(entry))
        {
            struct ext2_ext_attr_entry *next = EXT2_EXT_ATTR_NEXT(entry);

            if (entry->e_value_size > storage_size || (char *) next >= end)
            {
                //fprintf(out, "invalid EA entry in inode\n");
                return;
            }

            //fprintf(out, "  ");

            //dump_xattr_string(out, EXT2_EXT_ATTR_NAME(entry), entry->e_name_len);

            std::string name = EXT2_EXT_ATTR_NAME(entry);

            if (name == "capability")
            {
                //*out = *(uint64_t*)(start + entry->e_value_offs);

                //assert(0);

                //*out = start + entry->e_value_offs;

                //dump

                /*

                dump_xattr_string(
                    stderr,
                    "xattr_fetch_capibilities",
                    EXT2_EXT_ATTR_NAME(entry),
                    entry->e_name_len);

                fprintf(stderr, " = \"");

                dump_xattr_string(
                    stderr,
                    "xattr_fetch_capibilities",
                    start + entry->e_value_offs,
                    entry->e_value_size);

                fprintf(stderr, "\" (%u)\n", entry->e_value_size);

                */

                uint64_t cap = get_capabilities(
                            (struct vfs_cap_data*)(start + entry->e_value_offs)
                            );

                //make_vfs_cap_data(cap);

                *out = cap;

                return;
            }

            //fprintf(out, " = \"");

            //dump_xattr_string(out, start + entry->e_value_offs, entry->e_value_size);

            //fprintf(out, "\" (%u)\n", entry->e_value_size);

            entry = next;
        }

        //
        //*out = 0UL;
    }
}

void xattr_fetch_capibilities__(const EXT2_INODE *inode, uint16_t inode_size, uint64_t* out)
{
	struct ext2_ext_attr_entry *entry;
    __u32 *magic;
    char *start, *end;
    unsigned int storage_size;

    if (inode->i_extra_isize > inode_size - EXT2_GOOD_OLD_INODE_SIZE)
    {
        fprintf(stderr, "invalid inode->i_extra_isize (%u)\n",
                inode->i_extra_isize);
        return;
    }

    //storage_size = EXT2_INODE_SIZE(current_fs->super) -
    storage_size = (unsigned int)inode_size -
            (unsigned int)EXT2_GOOD_OLD_INODE_SIZE -
            (unsigned int)inode->i_extra_isize;

    magic = (__u32 *)((char *)inode + EXT2_GOOD_OLD_INODE_SIZE +
            inode->i_extra_isize);

    if (*magic == EXT2_EXT_ATTR_MAGIC)
    {
        end = (char *) inode + inode_size;

        start = (char *) magic + sizeof(__u32);

        entry = (struct ext2_ext_attr_entry *) start;

        while (!EXT2_EXT_IS_LAST_ENTRY(entry))
        {
            struct ext2_ext_attr_entry *next = EXT2_EXT_ATTR_NEXT(entry);

            if (entry->e_value_size > storage_size || (char *) next >= end)
            {
                //fprintf(out, "invalid EA entry in inode\n");
                return;
            }

            //fprintf(out, "  ");

            //dump_xattr_string(out, EXT2_EXT_ATTR_NAME(entry), entry->e_name_len);

			std::string name = EXT2_EXT_ATTR_NAME(entry);

			if (name == "capability")
			{
                *out = *(uint64_t*)(start + entry->e_value_offs);

                //assert(0);

                return;
			}

            //fprintf(out, " = \"");

            //dump_xattr_string(out, start + entry->e_value_offs, entry->e_value_size);

            //fprintf(out, "\" (%u)\n", entry->e_value_size);

            entry = next;
        }

		//
		*out = 0UL;
    }
}


void internal_dump_inode_extra(FILE *out,
                      //const char *prefix EXT2FS_ATTR((unused)),
                      //ext2_ino_t inode_num EXT2FS_ATTR((unused)),
                      //struct ext2_inode_large *inode)
                      const EXT2_INODE *inode,
                      uint16_t inode_size)
{
    struct ext2_ext_attr_entry *entry;
    __u32 *magic;
    char *start, *end;
    unsigned int storage_size;

    fprintf(out, "Size of extra inode fields: %u\n", inode->i_extra_isize);

    if (inode->i_extra_isize > inode_size - EXT2_GOOD_OLD_INODE_SIZE)
    {
        fprintf(stderr, "invalid inode->i_extra_isize (%u)\n",
                inode->i_extra_isize);
        return;
    }

    //storage_size = EXT2_INODE_SIZE(current_fs->super) -
    storage_size = (unsigned int)inode_size -
            (unsigned int)EXT2_GOOD_OLD_INODE_SIZE -
            (unsigned int)inode->i_extra_isize;

	//fprintf(stderr, "internal_dump_inode_extra : storage_size (%u)\n", storage_size);

	std::cerr<<"internal_dump_inode_extra : storage_size : "<<storage_size<<std::endl;
	std::cerr<<"internal_dump_inode_extra : (unsigned int)inode_size : "<<(unsigned int)inode_size<<std::endl;
	std::cerr<<"internal_dump_inode_extra : EXT2_GOOD_OLD_INODE_SIZE : "<<EXT2_GOOD_OLD_INODE_SIZE<<std::endl;
	std::cerr<<"internal_dump_inode_extra : inode->i_extra_isize : "<<inode->i_extra_isize<<std::endl;

    magic = (__u32 *)((char *)inode + EXT2_GOOD_OLD_INODE_SIZE +
            inode->i_extra_isize);

	std::cerr
		<<"internal_dump_inode_extra : sizeof : sizeof(*inode)"
		<<sizeof(*inode)<<std::endl;

	std::cerr
		<<"internal_dump_inode_extra : magic offset : "
		<<EXT2_GOOD_OLD_INODE_SIZE + inode->i_extra_isize<<std::endl;

	std::cerr<<"internal_dump_inode_extra : *magic : "<<std::hex<<*magic<<std::endl;

    if (*magic == EXT2_EXT_ATTR_MAGIC)
    {
        fprintf(out, "Extended attributes stored in inode body: \n");

        end = (char *) inode + inode_size;

        start = (char *) magic + sizeof(__u32);

        entry = (struct ext2_ext_attr_entry *) start;

		//debug
		{
			std::cerr<<"internal_dump_inode_extra - start : "<<start<<std::endl;
			std::cerr<<"internal_dump_inode_extra - end : "<<end<<std::endl;
			std::cerr<<"internal_dump_inode_extra - entry : "<<entry<<std::endl;
		}

        while (!EXT2_EXT_IS_LAST_ENTRY(entry))
        {
            struct ext2_ext_attr_entry *next = EXT2_EXT_ATTR_NEXT(entry);

            if (entry->e_value_size > storage_size || (char *) next >= end)
            {
                fprintf(out, "invalid EA entry in inode\n");
                return;
            }

            fprintf(out, "  ");

            if(0)
            {
                dump_xattr_string(
                    out,
                    "internal_dump_inode_extra",
                    EXT2_EXT_ATTR_NAME(entry),
                    entry->e_name_len);
            }

			std::string name = EXT2_EXT_ATTR_NAME(entry);

			if (name == "selinux")
			{
				NULL;
			}

            fprintf(out, " = \"");

            if(0)
            {
                dump_xattr_string(
                    out,
                    "internal_dump_inode_extra",
                    start + entry->e_value_offs,
                    entry->e_value_size);
            }

            fprintf(out, "\" (%u)\n", entry->e_value_size);

            entry = next;
        }
    }
}
