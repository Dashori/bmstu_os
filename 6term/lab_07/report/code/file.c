struct file {
union {
    struct llist_node   fu_llist;
    struct rcu_head     fu_rcuhead;
} f_u;
struct path     f_path;
struct inode    *f_inode;	/* cached value */
const struct file_operations	*f_op;

/*
* Protects f_ep, f_flags.
* Must not be taken from IRQ context.
*/
spinlock_t              f_lock;
enum rw_hint            f_write_hint;
atomic_long_t           f_count;
unsigned int            f_flags;
fmode_t                 f_mode;
struct mutex            f_pos_lock;
loff_t                  f_pos;
struct fown_struct      f_owner;
const struct cred      *f_cred;
struct file_ra_state	f_ra;

u64    	f_version;
#ifdef CONFIG_SECURITY
void    *f_security;
#endif
/* needed for tty driver, and maybe others */
void    	*private_data;

#ifdef CONFIG_EPOLL
/* Used by fs/eventpoll.c to link all the hooks to this file */
struct hlist_head	*f_ep;
#endif /* #ifdef CONFIG_EPOLL */
struct address_space	*f_mapping;
errseq_t    f_wb_err;
errseq_t    f_sb_err; /* for syncfs */
} __randomize_layout
/* lest something weird decides that 2 is OK */
__attribute__((aligned(4)));	
