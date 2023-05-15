#include <linux/init.h>  
#include <linux/init_task.h>  
#include <linux/kernel.h>     
#include <linux/module.h>     
#include <linux/sched.h>      

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chepigo Darya");

static int __init mod_init(void) {
      struct task_struct *task = &init_task;
       do
       {
       printk(KERN_INFO "+ taskp - %llu, task - %s, pid - %d, parent - %s, ppid - %d, state - %d, flags - %x, prio - %d, policy - %d\n", 
       task,
       task->comm,
       task->pid,
       task->parent->comm,
       task->parent->pid,
       task->__state,
       task->flags,
       task->prio,
       task->policy);
       } while ((task = next_task(task)) != &init_task);
       printk(KERN_INFO "+ Current taskp - %llu, task - %s, pid - %d, parent - %s, ppid - %d, state - %d, flags - %x, prio - %d, policy - %d\n", 
       current,
       current->comm,
       current->pid,
       current->parent->comm,
       current->parent->pid,
       current->__state,
       current->flags,
       current->prio,
       current->policy
       );

      return 0;

}

static void __exit mod_exit(void) {}

module_init(mod_init);
module_exit(mod_exit);
