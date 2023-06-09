#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chepigo Darya");

#define IRQ_NUM 1

struct workqueue_struct *workqueue;

typedef struct
{
   struct work_struct my_work;
   int code;
} work_t;

work_t *work1;
work_t *work2;

char * ascii[84] = 
    {" ", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "Backspace", 
     "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Ctrl",
     "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "\"", "'", "Shift (left)", "|", 
     "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "Shift (right)", 
     "*", "Alt", "Space", "CapsLock", 
     "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
     "NumLock", "ScrollLock", "Home", "Up", "Page-Up", "-", "Left",
     " ", "Right", "+", "End", "Down", "Page-Down", "Insert", "Delete"};

     

void queue_function1(struct work_struct *work)
{
	work_t *my_work = (work_t *)work;
    if (my_work->code < 84) 
    {
        printk(KERN_INFO "=: work 1 keyboard=%s, time=%llu\n", ascii[my_work->code], ktime_get());
    }
}

void queue_function2(struct work_struct *work)
{
	work_t *my_work = (work_t *)work;
	printk(KERN_INFO "=: work 2 before delay =%llu\n", ktime_get());
	msleep(1000);
	printk(KERN_INFO "=: work 2 after delay=%llu\n", ktime_get());
	if (my_work->code < 84) 
    {
        printk(KERN_INFO "=: work 2 keyboard=%s, time=%llu\n", ascii[my_work->code], ktime_get());
    }
}

irqreturn_t handler(int irq, void *dev)
{
	int code = inb(0x60);
	work1->code = code;
	work2->code = code;
	
    if (irq == IRQ_NUM)
    {
        queue_work(workqueue, (struct work_struct *)work1);                                                                                     
        queue_work(workqueue, (struct work_struct *)work2);
        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}


static int __init work_queue_init(void)
{
    int ret;
    ret = request_irq(IRQ_NUM, handler, IRQF_SHARED, "my_irq", &handler);                                     // регистрируем обработчик
	
    if (ret)
    {
        printk(KERN_ERR "=: request_irq() failed!\n");
        return ret;
    }

    if (!(workqueue = alloc_workqueue("my_queue", WQ_FREEZABLE, 1)))                                                                         
    {
        free_irq(IRQ_NUM, &handler);
        printk(KERN_INFO "=: create_workqueue() failed!");
        return -ENOMEM;
    }

	work1 = (work_t *)kmalloc(sizeof(work_t), GFP_KERNEL);
	if (!work1)
	{
		printk(KERN_ERR "=: kmalloc() work1 failed");	
		return -ENOMEM;
	}
    INIT_WORK((struct work_struct *)work1, queue_function1);

    work2 = (work_t *)kmalloc(sizeof(work_t), GFP_KERNEL);
	if (!work2)
	{
		printk(KERN_ERR "=: kmalloc() work1 failed");	
		return -ENOMEM;
	}
    INIT_WORK((struct work_struct *)work2, queue_function2);

    printk(KERN_INFO "=: module loaded!\n");
    return 0;
}

static void __exit work_queue_exit(void)
{
    flush_workqueue(workqueue);
    destroy_workqueue(workqueue);
    free_irq(IRQ_NUM, &handler);
    kfree(work1);
    kfree(work2);
    printk(KERN_INFO "=: module unloaded!\n");
}

module_init(work_queue_init) 
module_exit(work_queue_exit)
