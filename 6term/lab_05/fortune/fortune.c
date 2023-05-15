#include <asm/uaccess.h>
#include <linux/init.h>  
#include <linux/init_task.h>
#include <linux/module.h>  
#include <linux/proc_fs.h> 
#include <linux/string.h>
#include <linux/version.h>  
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chepigo Darya");

#define _BUF_SIZE PAGE_SIZE

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#define HAVE_PROC_OPS
#endif

static char *_cookie_pot; // буфер 
static unsigned read_inx;
static unsigned write_inx;
static struct proc_dir_entry *proc_entry, *proc_dir, *sym_link;
char tmp[256];

int fortune_open(struct inode *sp_inode, struct file *sp_file) {
  printk(KERN_INFO "+ call fortune open\n");
  return 0;
}

int fortune_release(struct inode *sp_node, struct file *sp_file) {
    printk(KERN_INFO "+ call fortune release\n");
    return 0;
}

ssize_t fortune_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos) {
  int space_left = (_BUF_SIZE - write_inx) + 1;

  printk(KERN_INFO "+ fortine write is called");

  if (space_left < count) {
    printk(KERN_INFO "+ ERROR: buffer is full. Terminating...");
    return -ENOSPC;
    }
    if (copy_from_user(&_cookie_pot[write_inx], buf, count)) {
      return -EFAULT; 
    }

    write_inx += count;
    _cookie_pot[write_inx - 1] = '\0';

    printk(KERN_INFO "+ writing successful");
    return count;
}

static ssize_t fortune_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos) {
    int len;
    if (*ppos > 0) {
        return 0;
    }
    if (read_inx >= write_inx) {
        read_inx = 0;
    }
    len = 0;

    if (write_inx > 0) {
        len = sprintf(tmp, "%s\n", &_cookie_pot[read_inx]);

        copy_to_user(buf, tmp, len);
        buf += len;
        read_inx += len;
    }

    *ppos += len;

    printk(KERN_INFO "+ reading successful");
    return len;
}


#ifdef HAVE_PROC_OPS
static struct proc_ops fileops = {
    .proc_read = fortune_read,
    .proc_write = fortune_write,
    .proc_open = fortune_open,
    .proc_release = fortune_release,
};
#else
static struct file_operations fileops = {
    .owner = THIS_MODULE,
    .read = fortune_read,
    .write = fortune_write,
    .open = fortune_open,
    .release = fortune_release,
};
#endif


static int __init fortune_init(void) {
    _cookie_pot = (char *)vmalloc(_BUF_SIZE);
    if (!_cookie_pot) {
        printk(KERN_INFO "+ ERROR: cant vmalloc!\n");
        return -ENOMEM;
    }
    memset(_cookie_pot, 0, _BUF_SIZE);

    proc_entry = proc_create("fortune_file", S_IRUGO | S_IWUGO, NULL, &fileops);
    if (!proc_entry) {
        vfree(_cookie_pot);
        printk(KERN_INFO "+ cant proc_create\n");
        return -ENOMEM;
    }
    
    proc_dir = proc_mkdir("fortune_dir", NULL);
    sym_link = proc_symlink("fortune_symlink", NULL, "/proc/fortune_dir");

    if ((proc_dir == NULL) || (sym_link == NULL)) {
        vfree(_cookie_pot);
        printk(KERN_INFO "+ cant proc_create\n");
        return -ENOMEM;
    }

    read_inx = 0;
    write_inx = 0;

    printk(KERN_INFO "+ init.");
    return 0;
}

static void __exit fortune_exit(void) {
  remove_proc_entry("fortune_seq", NULL);
  remove_proc_entry("fortune_symlink", NULL);
  remove_proc_entry("fortune_dir", NULL);

  if (_cookie_pot) {
    vfree(_cookie_pot);
    }

    printk(KERN_INFO "+ module unloaded.\n");
}

module_init(fortune_init);
module_exit(fortune_exit);
