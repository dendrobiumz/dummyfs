
#define DUMMY_MAGIC  (0x12110610)

#define BLOCK_BIT     (12)
#define DUMMY_BLOCK_SIZE    (1 << BLOCK_BIT)

#define MAX_DATA_BLOCK_NB   (BLOCK_SIZE * sizeof(uint64_t))
#define MAX_INODE_NB        (1 << 10)

#define MAX_NAME_LEN  (255)
#define MAX_BLK_NR    (16)

#define SB_BLOCK_NO   (0)
#define BG_BLOCK_NO   (1)
#define INODE_BITMAP_BLOCK_NO (2)
#define BLOCK_BITMAP_BLOCK_NO (3)
#define INODE_TABLE_BLOCK_NO  (4)
#define DATA_BLOCK_START_NO   (5)

#include <linux/stddef.h>
#include <linux/types.h>


struct dummyfs_inode_disk {
    uint16_t      i_mode;
    uint32_t      i_uid;
    uint32_t      i_gid;

    uint32_t      i_size;
    uint32_t      i_ctime; // time when inode was created
    uint32_t      i_mtime; // last time when inode was modified 
    uint32_t      i_atime; // time when inode was accessed  
    uint32_t      i_links_count;
    uint32_t      i_flags;
    uint32_t      i_block[MAX_BLK_NR]; // 0 - 11 direct block no, 12 - 15 indirect block
};

struct dummyfs_inode_mem {
    struct inode vfs_inode;
    char         i_block_no[MAX_BLK_NR];
};

struct dummyfs_superblock {
    uint32_t s_magic;
    uint32_t s_inodes_count;
    uint32_t s_blocks_count;

    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;

    uint32_t s_first_data_block;
    uint32_t s_blocks_per_group;
    uint32_t s_inodes_per_group;

    uint32_t s_mtime;
    uint32_t s_wtime;

    uint16_t s_state;
    uint16_t s_errors;

    uint32_t s_lastcheck;
    uint32_t s_inode_size;

    uint64_t *cur_free_inode_bmap;
    uint64_t *cur_free_data_blk_bmap;
};

#define INODE_PER_BLOCK   (BLOCK_SIZE / sizeof(struct dummyfs_inode_disk))

struct dummyfs_blockgroup {
    uint32_t bg_block_bmap; // block id
    uint32_t bg_inode_bmap;
    uint32_t bg_inode_table;
    uint32_t bg_free_blocks_count;
    uint32_t bg_free_inodes_count;
    uint32_t bg_used_dirs_count;
};

struct dummyfs_dentry {
    uint32_t inode_no; // inode number in inode table
    uint32_t rec_len;
    uint8_t  name_len;
    uint8_t  file_type;
    char     name[MAX_NAME_LEN];
};


struct dummyfs_file {
    uint32_t inode_no; // inode number in inode table
    char filename[MAX_NAME_LEN];
};

int dummyfs_fill_super(struct super_block *sb, void *data, int silent);