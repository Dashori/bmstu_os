struct audit_names;
	
struct filename {
    const char          *name;/* pointer to actual string */
    const __user char   *uptr;/* original userland pointer */
    int                 refcnt;
    struct audit_names  *aname;
    const char          iname[];
};	

struct audit_names {
    struct list_head    list;/* audit_context->names_list */
    struct filename     *name;
    int                 name_len;/* number of chars to log */
    bool                hidden;	/* don't log this record */
    unsigned long       ino;
    dev_t               dev;
    umode_t             mode;
    kuid_t              uid;
    kgid_t              gid;
    dev_t               rdev;
    u32                 osid;
    struct audit_cap_data   fcap;
    unsigned int        fcap_ver;
    unsigned char       type;	/* record type */
/*
* This was an allocated audit_names and not from the array of
* names allocated in the task audit context.  Thus this name
* should be freed on syscall exit.
*/
    bool                should_free;
};