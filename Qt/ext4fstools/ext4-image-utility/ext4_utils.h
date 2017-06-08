/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _EXT4_UTILS_H_
#define _EXT4_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdint.h>

#include <io.h>

#include "sparse_defs.h"

#define EXT4_SUPER_MAGIC 0xEF53

#define EXT4_VALID_FS 0x0001
#define EXT4_ERROR_FS 0x0002
#define EXT4_ORPHAN_FS 0x0004

struct ext4_super_block {
  __le32 s_inodes_count;
 __le32 s_blocks_count_lo;
 __le32 s_r_blocks_count_lo;
 __le32 s_free_blocks_count_lo;
  __le32 s_free_inodes_count;
 __le32 s_first_data_block;
 __le32 s_log_block_size;
 __le32 s_obso_log_frag_size;
  __le32 s_blocks_per_group;
 __le32 s_obso_frags_per_group;
 __le32 s_inodes_per_group;
 __le32 s_mtime;
  __le32 s_wtime;
 __le16 s_mnt_count;
 __le16 s_max_mnt_count;
 __le16 s_magic;
 __le16 s_state;
 __le16 s_errors;
 __le16 s_minor_rev_level;
  __le32 s_lastcheck;
 __le32 s_checkinterval;
 __le32 s_creator_os;
 __le32 s_rev_level;
  __le16 s_def_resuid;
 __le16 s_def_resgid;

 __le32 s_first_ino;
 __le16 s_inode_size;
 __le16 s_block_group_nr;
 __le32 s_feature_compat;
  __le32 s_feature_incompat;
 __le32 s_feature_ro_compat;
  __u8 s_uuid[16];
  char s_volume_name[16];
  char s_last_mounted[64];
  __le32 s_algorithm_usage_bitmap;

 __u8 s_prealloc_blocks;
 __u8 s_prealloc_dir_blocks;
 __le16 s_reserved_gdt_blocks;

  __u8 s_journal_uuid[16];
  __le32 s_journal_inum;
 __le32 s_journal_dev;
 __le32 s_last_orphan;
 __le32 s_hash_seed[4];
 __u8 s_def_hash_version;
 __u8 s_reserved_char_pad;
 __le16 s_desc_size;
  __le32 s_default_mount_opts;
 __le32 s_first_meta_bg;
 __le32 s_mkfs_time;
 __le32 s_jnl_blocks[17];

  __le32 s_blocks_count_hi;
 __le32 s_r_blocks_count_hi;
 __le32 s_free_blocks_count_hi;
 __le16 s_min_extra_isize;
 __le16 s_want_extra_isize;
 __le32 s_flags;
 __le16 s_raid_stride;
 __le16 s_mmp_interval;
 __le64 s_mmp_block;
 __le32 s_raid_stripe_width;
 __u8 s_log_groups_per_flex;
 __u8 s_reserved_char_pad2;
 __le16 s_reserved_pad;
 __le64 s_kbytes_written;
 __u32 s_reserved[160];
};

int ext4_info(const char *filename);

#ifdef __cplusplus
}
#endif

#endif
