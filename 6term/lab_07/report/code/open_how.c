struct path {
	struct vfsmount *mnt;
	struct dentry *dentry;
} __randomize_layout;

struct open_how {
	__u64 flags;
	__u64 mode;
	__u64 resolve;
};

inline struct open_how build_open_how(int flags, umode_t mode)
{
	struct open_how how = {
		.flags = flags & VALID_OPEN_FLAGS,
		.mode = mode & S_IALLUGO,};
	/* O_PATH beats everything else. */
	if (how.flags & O_PATH)
		how.flags &= O_PATH_FLAGS;
	/* Modes should only be set for create-like flags. */
	if (!WILL_CREATE(how.flags))
		how.mode = 0;
	return how;
}