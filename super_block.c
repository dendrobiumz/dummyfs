#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

#include "dummyfs.h"

static struct kmem_cache *dummyfs_inode_cache;



int dummyfs_init_inode_cache()
{
    dummyfs_inode_cache = kmem_cache_create_usercopy("dummyfs_inode_cache", 
                            sizeof(struct dummy_inode_mem),
                            0, 0, 0,
                            sizeof(struct dummy_inode_mem), NULL);
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
    struct dummy_inode_mem *di = kmem_cache_alloc(dummyfs_inode_cache, GFP_KERNEL);
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
    struct dummyfs_inode_mem *di = i;
    kmeme_cache_free(dummyfs_inode_cache, di);
}

/*
 * this method is called when the VFS needs to write an inode to disc. 
 * The second parameter indicates whether the write should be synchronous or not, not all filesystems check this flag.
 */
int dummyfs_write_inode(struct inode *i, struct writeback_control *wbc)
{
    struct dummyfs_inode_disk *inode_disk;
    struct dummyfs_inode_mem *inode_mem = i;
    struct buffer_head *bh;
    // read the inode block by block number into memory
    // calc real number by i->i_ino;
    uint32_t ino = i->i_ino;
    uint32_t block_cnt = ino / INODE_PER_BLOCK;
    uint32_t block_shift = ino % INODE_PER_BLOCK;

    return 0;
}

/*
 * called when VFS is writing out all dirty data associated with a superblock.
 * The second parameter indicates whether the method should wait until the write out has been completed. Optional.
 */
int dummyfs_sync_fs(struct super_block *sb, int wait)
{
    return 0;
}

/*
 * called when the VFS needs to get filesystem statistics.
 */
int dummyfs_statfs(struct dentry *d, struct kstatfs *stat)
{
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