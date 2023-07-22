#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/time.h>

#define MYFS_MAGIC_NUMBER 0x13131313
#define MAX_CACHE_SIZE 128
#define SLAB_NAME "myfs_slab"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chepigo Darya");

struct myfs_inode {
    int i_mode;
    unsigned long i_ino;
};

static struct kmem_cache *my_inode_cache = NULL;
static struct myfs_inode **inode_pointers = NULL;
static int cached_count = 0;

static struct myfs_inode *cache_get_inode(void) 
{
    printk(KERN_DEBUG "+ cache_get_inode started\n");
    if (cached_count == MAX_CACHE_SIZE)
    {
        printk(KERN_DEBUG "+ reached MAX_CACHE_SIZE\n");
        return NULL;
    }
    return inode_pointers[cached_count++] = kmem_cache_alloc(my_inode_cache, GFP_KERNEL);
}

static struct inode *myfs_make_inode(struct super_block *sb, int mode) 
{
    struct inode *ret = new_inode(sb);
    struct myfs_inode *my_inode_cache = NULL;
    if (ret) 
    {
        inode_init_owner(&init_user_ns, ret, NULL, mode);
        ret->i_ino = 1;
        ret->i_size = PAGE_SIZE;
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
        if ((my_inode_cache = cache_get_inode()) != NULL) 
        {
            my_inode_cache->i_mode = ret->i_mode;
            my_inode_cache->i_ino = ret->i_ino;
        }
        ret->i_private = my_inode_cache;
    }
    return ret;
}

static void myfs_put_super(struct super_block *sb) 
{
    printk(KERN_DEBUG "+ super block destroyed\n");
}

static struct super_operations const myfs_super_ops = 
{
      .put_super = myfs_put_super,
      .statfs = simple_statfs,
      .drop_inode = generic_delete_inode,
};

static int myfs_fill_sb(struct super_block *sb, void *data, int silent) 
{
    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = MYFS_MAGIC_NUMBER;
    sb->s_op = &myfs_super_ops;
    struct inode *root = myfs_make_inode(sb, S_IFDIR | 0755);
    if (!root) 
    {
        printk(KERN_ERR "+ inode allocation failed\n");
        return -ENOMEM;
    }
    root->i_op = &simple_dir_inode_operations;
    root->i_fop = &simple_dir_operations;
    if (!(sb->s_root = d_make_root(root)))
    {
        printk(KERN_ERR "+ root creation failed\n");
        iput(root);
        return -ENOMEM;
    }
    return 0;
}

static struct dentry *myfs_mount(struct file_system_type *type, int flags, char const *dev, void *data) 
{
    printk(KERN_ERR "+ myfs_mount called\n");
    struct dentry *const entry = mount_nodev(type, flags, data, myfs_fill_sb);
    if (IS_ERR(entry))
    {
        printk(KERN_ERR "+ mounting failed\n");
    }
    else
    {
        printk(KERN_DEBUG "+ mounted\n");
    }
    return entry;
}

static struct file_system_type myfs_type = {
    .owner = THIS_MODULE,
    .name = "myfs",
    .mount = myfs_mount,
    .kill_sb = kill_anon_super,
    // прекращение доступа к суперблоку
};

static int __init myfs_init(void) 
{
    int ret = register_filesystem(&myfs_type);
    if (ret != 0) 
    {
        printk(KERN_ERR "+ Failed to register filesystem\n");
        return ret;
    }
    if ((inode_pointers = kmalloc(sizeof(struct myfs_inode *) * MAX_CACHE_SIZE, GFP_KERNEL)) == NULL)
    {
        printk(KERN_ERR "+ Failed to allocate inode_pointers\n");
        return -ENOMEM;
    }
    if ((my_inode_cache = kmem_cache_create(SLAB_NAME, sizeof(struct myfs_inode), 0, 0, NULL)) == NULL)
    {
        kfree(inode_pointers);
        printk(KERN_ERR "+ Failed to create cache\n");

        return -ENOMEM;
    }
    printk(KERN_DEBUG "+ Module loaded\n");
    return 0;
}

static void __exit myfs_exit(void) 
{
    int i;
    for (i = 0; i < cached_count; i++) 
    {
        kmem_cache_free(my_inode_cache, inode_pointers[i]);
    }
    kmem_cache_destroy(my_inode_cache); 
    kfree(inode_pointers);
    int ret = unregister_filesystem(&myfs_type);
    if (ret != 0)
    {
        printk(KERN_ERR "+ Can't unregister filesystem\n");
        return;
    }
    printk(KERN_DEBUG "+ Module unloaded\n");
}

module_init(myfs_init);
module_exit(myfs_exit);