#include <linux/fs.h>


struct dentry *dummyfs_lookup(struct inode *i, struct dentry *d, unsigned int flag)
{

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


int dummyfs_rename(struct mnt_idmap *id,
                    struct inode *i1,
                    struct dentry *d1,
                    struct inode *i2,
                    struct dentry *d2,
                    unsigned int flag)
{
    return 0;
}


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