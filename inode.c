#include <linux/fs.h>


static int dummyfs_get_inode(struct super_block *sb, unsigned long i_no)
{
    return 0;
}



/*
 * called when the VFS needs to look up an inode in a parent directory.
 * The name to look for is found in the dentry. 
 * This method must call d_add() to insert the found inode into the dentry.  
 * The “i_count” field in the inode structure should be incremented. 
 * 
 */
struct dentry *dummyfs_lookup(struct inode *i, struct dentry *d, unsigned int flag)
{
    struct dentry *ret;

    return ret;
}


int dummyfs_create(struct mnt_idmap *id,
                    struct inode *i,
                    struct dentry *d,
                    umode_t mode,
                    bool flag
                    )
{
    return 0;
}


int dummyfs_unlink(struct inode *i, struct dentry *d)
{
    return 0;
}


int dummyfs_mkdir(struct mnt_idmap *id,
                    struct inode *i,
                    struct dentry *d,
                    umode_t mode)
{
    return 0;
}


int dummyfs_rmdir(struct inode *i, struct dentry *d)
{
    return 0;
}

/*
 * called by the rename(2) system call to rename the object to have the parent 
 * and name given by the second inode and dentry.
 */
int dummyfs_rename(struct mnt_idmap *id,
                    struct inode *i1,
                    struct dentry *d1,
                    struct inode *i2,
                    struct dentry *d2,
                    unsigned int flag)
{
    return 0;
}

/*
 * called by the link(2) system call
 * Only required if you want to support hard links.
 * You will probably need to call d_instantiate() just as you would in the create() method
 */
int dummyfs_link(struct dentry *src, struct inode *i, struct dentry *dst)
{

}


int dummyfs_symlink(struct mn_idmap *id,
                    struct inode *i,
                    struct dentry *d,
                    const char *name)
{
    return 0;
}

static const struct inode_operations simplefs_inode_ops = {
    .lookup = dummyfs_lookup,
    .create = dummyfs_create,
    .unlink = dummyfs_unlink,
    .mkdir = dummyfs_mkdir,
    .rmdir = dummyfs_rmdir,
    .rename = dummyfs_rename,
    .link = dummyfs_link,
    .symlink = dummyfs_symlink,
};