#include <linux/slab.h>
#include <linux/fs.h>

#include "dummyfs.h"

static struct kmem_cache *dummyfs_inode_cache;



int dummyfs_init_inode_cache()
{
    dummyfs_inode_cache = kmem_cache_create_usercopy("dummyfs_inode_cache", sizeof(struct dummy_inode)), ;
    return 0;
}

// called when the VFS wishes to free the superblock (i.e. unmount). 
// This is called with the superblock lock held
void dummyfs_put_super(struct super_block *sb)
{

}

// this method is called by alloc_inode() to allocate memory 
// for struct inode and initialize it. 
// If this function is not defined, a simple ‘struct inode’ is allocated. 
// Normally alloc_inode will be used to allocate a larger structure 
// which contains a ‘struct inode’ embedded within it.
struct inode *dummyfs_alloc_inode(struct super_block *sb)
{
    
}

/*
 *this method is called by destroy_inode() to release resources allocated for struct inode. 
 *It is only required if ->alloc_inode was defined and simply undoes anything done by ->alloc_inode.
 */

void dummyfs_destroy_inode(struct inode *i)
{

}

/*
 *this method is called when the VFS needs to write an inode to disc. 
 *The second parameter indicates whether the write should be synchronous or not, not all filesystems check this flag.
 */
int dummyfs_write_inode(struct inode *i, struct writeback_control *wbc)
{
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