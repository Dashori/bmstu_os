#include <linux/init.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chepigo Darya");

#define COOKIE_POT_SIZE PAGE_SIZE
#define FILENAME "fortune_file"
#define DIRNAME  "fortune_dir"
#define SYMLINK  "fortune_symlink"

static struct proc_dir_entry *fortuneFile;
static struct proc_dir_entry *fortuneDir;
static struct proc_dir_entry *fortuneLink;

static char *cookiePot = NULL;

static int readInd = 0;
static int writeInd = 0;

static void *fortuneStart(struct seq_file *file, loff_t *pos)
{
    printk(KERN_INFO "= start called\n");

    if ((*pos > 0) || (writeInd == 0))
    {
        *pos = 0;
        return NULL;
    }

    if (readInd >= writeInd)
        readInd = 0;

    return cookiePot + readInd;
}

static void *fortuneNext(struct seq_file *file, void *v, loff_t *pos)
{
    printk(KERN_INFO "= next called, v = %pX, pos = %Ld\n", v, *pos);

    readInd++;
    (*pos)++;

    return (*((char *) v) == '\0') ? NULL : (char *)v + 1;
}

static int fortuneShow(struct seq_file *file, void *v)
{
    printk(KERN_INFO "= show called\n");
    seq_printf(file, "%c", *((char *) v));

    return 0;
}

static void fortuneStop(struct seq_file *file, void *v)
{
    printk(KERN_INFO "= stop called\n");
    if (v)
    	printk(KERN_INFO "v is %pX\n", v);
    else 
    	printk(KERN_INFO "v is null\n");
}


static const struct seq_operations seqops =
{
    .start = fortuneStart,
    .next  = fortuneNext,
    .stop  = fortuneStop,
    .show  = fortuneShow
};

static int fortuneOpen(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "= open called\n");
    return seq_open(file, &seqops);
}

static ssize_t fortuneWrite(struct file *file, const char __user *buf,
                            size_t len, loff_t *fPos)
{
    printk(KERN_INFO "= write called\n");


    if (len > COOKIE_POT_SIZE - writeInd + 1)
    {
        printk(KERN_ERR "= buffer overflow\n");
        return -ENOSPC;
    }

    if (copy_from_user(&cookiePot[writeInd], buf, len) != 0)
    {
        printk(KERN_ERR "= copy_from_user error\n");
        return -EFAULT;
    }

    writeInd += len;
    cookiePot[writeInd - 1] = '\0';

    return len;
}

static int fortuneRelease(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "= release called\n");
    return 0;
}

static ssize_t fortuneRead(struct file *file, char __user *buf,
                           size_t len, loff_t *fPos)
{
    printk(KERN_INFO "= read called\n");
    return seq_read(file, buf, len, fPos);
}

static const struct proc_ops fops =
{
    .proc_open = fortuneOpen,
    .proc_read = fortuneRead,
    .proc_write = fortuneWrite,
    .proc_release = fortuneRelease
};


static void freeMemory(void)
{
    if (fortuneLink != NULL)
        remove_proc_entry(SYMLINK, NULL);

    if (fortuneFile != NULL)
        remove_proc_entry(FILENAME, NULL);

    if (fortuneDir != NULL)
        remove_proc_entry(DIRNAME, NULL);

    vfree(cookiePot);
}


static int __init md_init(void)
{
    if ((cookiePot = vmalloc(COOKIE_POT_SIZE)) == NULL)
    {
        printk(KERN_ERR "= memory error\n");
        return -ENOMEM;
    }

    memset(cookiePot, 0, COOKIE_POT_SIZE);

    if ((fortuneDir = proc_mkdir(DIRNAME, NULL)) == NULL)
    {
        printk(KERN_ERR "= create dir err\n");
        freeMemory();

        return -ENOMEM;
    }

    if ((fortuneFile = proc_create(FILENAME, 0666, NULL, &fops)) == NULL)
    {
        printk(KERN_ERR "= create file err\n");
        freeMemory();

        return -ENOMEM;
    }

    if ((fortuneLink = proc_symlink(SYMLINK, NULL, DIRNAME)) == NULL)
    {
        printk(KERN_ERR "= create link err\n");
        freeMemory();

        return -ENOMEM;
    }

    readInd = 0;
    writeInd = 0;

    printk(KERN_INFO "= init\n");
    return 0;
}

static void __exit md_exit(void)
{
    printk(KERN_INFO "= exit\n");
    freeMemory();
}


module_init(md_init);
module_exit(md_exit);
