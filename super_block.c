#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/statfs.h>

#include "dummyfs.h"

static struct kmem_cache *dummyfs_inode_cache;



int dummyfs_init_inode_cache()
{
    dummyfs_inode_cache = kmem_cache_create_usercopy("dummyfs_inode_cache", 
                            sizeof(struct dummyfs_inode_mem),
                            0, 0, 0,
                            sizeof(struct dummyfs_inode_mem), NULL);
    if (dummyfs_inode_cache == NULL)
        return -ENOMEM;
    return 0;
}


void dummyfs_destroy_inode_cache()
{
    rcu_barrier();
    if (dummyfs_destroy_inode_cache)
        kmem_cache_destroy(dummyfs_inode_cache);
}

// called when the VFS wishes to free the superblock (i.e. unmount). 
// This is called with the superblock lock held
void dummyfs_put_super(struct super_block *sb)
{
    struct dummy_superblock *dsb = sb->s_fs_info;

    sync_blockdev(sb->s_bdev);
    invalidate_bdev(sb->s_bdev);

    if(dsb) {
        
        kfree(dsb);
    }
}

// this method is called by alloc_inode() to allocate memory 
// for struct inode and initialize it. 
// If this function is not defined, a simple ‘struct inode’ is allocated. 
// Normally alloc_inode will be used to allocate a larger structure 
// which contains a ‘struct inode’ embedded within it.
struct inode *dummyfs_alloc_inode(struct super_block *sb)
{
    struct dummyfs_inode_mem *di = kmem_cache_alloc(dummyfs_inode_cache, GFP_KERNEL);
    if (di == NULL)
        return NULL;
    
    inode_init_once(&di->vfs_inode);
    return &di->vfs_inode;
}

/*
 * this method is called by destroy_inode() to release resources allocated for struct inode. 
 * It is only required if ->alloc_inode was defined and simply undoes anything done by ->alloc_inode.
 */

void dummyfs_destroy_inode(struct inode *i)
{
    struct dummyfs_inode_mem *di = (struct dummyfs_inode_mem*)i;
    kmeme_cache_free(dummyfs_inode_cache, di);
}

/*
 * this method is called when the VFS needs to write an inode to disc. 
 * The second parameter indicates whether the write should be synchronous or not, not all filesystems check this flag.
 */
int dummyfs_write_inode(struct inode *i, struct writeback_control *wbc)
{
    struct dummyfs_inode_disk *inode_disk;
    struct dummyfs_inode_mem *inode_mem = (struct dummyfs_inode_mem*)i;
    struct buffer_head *bh;
    struct super_block *sb = i->i_sb;
    struct dummyfs_superblock *dsb = (struct dummyfs_superblock *)sb->s_fs_info;
    // read the inode block by block number into memory
    // calc real number by i->i_ino;
    uint32_t ino = i->i_ino;
    uint32_t block_cnt = ino / INODE_PER_BLOCK;
    uint32_t block_shift_offset = ino % INODE_PER_BLOCK;

    if (ino >= dsb->s_inodes_count)
        return 0;

    bh = sb_bread(sb, block_cnt);
    if (bh == NULL)
        return -EIO;

    inode_disk = (struct dummyfs_inode_disk*)bh->b_data;
    inode_disk += block_shift_offset;

    inode_disk->i_mode = i->i_mode;
    inode_disk->i_size = i->i_size;
    inode_disk->i_flags = i->i_flags;
    inode_disk->i_uid = i_uid_read(i);
    inode_disk->i_gid = i_gid_read(i);
    inode_disk->i_atime = inode_get_atime_sec(i);
    inode_disk->i_mtime = inode_get_mtime_sec(i);
    struct timespec64 ctime = inode_get_ctime(i);
    inode_disk->i_ctime = ctime.tv_sec;
    strncpy(inode_disk->i_block, inode_mem->i_block_no, sizeof(inode_mem->i_block_no));

    mark_buffer_dirty(bh);
    sync_dirty_buffer(bh);
    brelse(bh);
    return 0;
}

/*
 * called when VFS is writing out all dirty data associated with a superblock.
 * The second parameter indicates whether the method should wait until the write out has been completed. Optional.
 */
int dummyfs_sync_fs(struct super_block *sb, int wait)
{
    struct dummyfs_superblock *dsb = sb->s_fs_info;
    struct dummyfs_superblock *disk_dsb;

    struct buffer_head *bh = sb_bread(sb, SB_BLOCK_NO);
    if (bh == NULL)
        return -EIO;
    disk_dsb = (struct dummyfs_superblock*) bh->b_data;
    disk_dsb->s_free_blocks_count = dsb->s_free_blocks_count;
    disk_dsb->s_free_inodes_count = dsb->s_free_inodes_count;
    disk_dsb->s_inodes_count = dsb->s_inodes_count;
    disk_dsb->s_blocks_count = dsb->s_blocks_count;

    mark_buffer_dirty(bh);
    if (wait)
        sync_dirty_buffer(bh);
    brelse(bh);

    // TO-DO
    // FLush free inode bitmask

    bh = sb_bread(sb, INODE_BITMAP_BLOCK_NO);
    uint32_t *inode_bitmap = (uint32_t*) bh->b_data;
    inode_bitmap += inode_bitmap - dsb->cur_free_inode_bmap;
    *inode_bitmap = dsb->cur_free_inode_bmap;
    mark_buffer_dirty(bh);
    if (wait)
        sync_dirty_buffer(bh);
    brelse(bh);

    // FLush free blocks bitmask
    bh = sb_bread(sb, BLOCK_BITMAP_BLOCK_NO);
    uint32_t *block_bitmap = (uint32_t*) bh->b_data;
    block_bitmap += block_bitmap - dsb->cur_free_data_blk_bmap;
    *block_bitmap = dsb->cur_free_data_blk_bmap;
    mark_buffer_dirty(bh);
    if (wait)
        sync_dirty_buffer(bh);
    brelse(bh);


    return 0;
}

/*
 * called when the VFS needs to get filesystem statistics.
 */
int dummyfs_statfs(struct dentry *d, struct kstatfs *stat)
{
    struct super_block *sb = d->d_sb;
    struct dummyfs_superblock *dsb = sb->s_fs_info;

    stat->f_type = DUMMY_MAGIC;
    stat->f_bsize = DUMMY_BLOCK_SIZE;
    stat->f_blocks = dsb->s_blocks_count;
    stat->f_bfree = dsb->s_free_blocks_count;
    stat->f_bavail = dsb->s_free_blocks_count;
    stat->f_files = dsb->s_inodes_count;
    stat->f_ffree = dsb->s_free_inodes_count;
    stat->f_namelen = MAX_NAME_LEN;

    return 0;
}


static struct super_operations simplefs_super_ops = {
    .put_super = dummyfs_put_super,
    .alloc_inode = dummyfs_alloc_inode,
    .destroy_inode = dummyfs_destroy_inode,
    .write_inode = dummyfs_write_inode,
    .sync_fs = dummyfs_sync_fs,
    .statfs = dummyfs_statfs,
};

int dummyfs_fill_super(struct super_block *sb, void *data, int silent)
{
    return 0;
}