
#define DUMMY_MAGIC  (0x12110610)

#define BLOCK_BIT     (12)
#define BLOCK_SIZE    (1 << BLOCK_BIT)

#define MAX_DATA_BLOCK_NB   (BLOCK_SIZE * sizeof(uint64_t))
#define MAX_INODE_NB        (1 << 10)

#include <linux/stddef.h>
#include <linux/types.h>


struct dummy_inode {
    uint16_t      i_mode;
    uint32_t      i_uid;
    uint32_t      i_gid;

};


// struct dummy_superblock {
//     uint32_t    magic;
    
//     uint32_t    nb_blocks;
//     uint32_t    nb_inodes;
//     uint32_t    block_sz;
//     uint32_t    nb_free_blocks;
//     uint32_t    nb_free_inodes;

// };

struct dummy_superblock {
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
}

#define INODE_PER_BLOCK   (BLOCK_SIZE / sizeof(struct dummy_inode))
