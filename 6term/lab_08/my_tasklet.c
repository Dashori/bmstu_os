#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/time.h>
#include <linux/delay.h>

#define IRQ_NUM 1 // прерывание от клавиатуры

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chepigo Darya");

char *my_tasklet_data = "my_tasklet data";
struct tasklet_struct* my_tasklet;


void my_tasklet_function(unsigned long data)
{
    int code = inb(0x60);
    
    printk( "==: "  "keyboard code=%d, state=%lu\n", code, my_tasklet->state);
}



irqreturn_t my_irq_handler(int irq, void *dev) 
{
    if (irq == IRQ_NUM)
    {
        printk("==: " "info before scheduling: state=%lu, time=%llu", my_tasklet->state, ktime_get());
        tasklet_schedule(my_tasklet); 
        printk("==: " "after:                  state=%lu, time=%llu", my_tasklet->state, ktime_get());
        return IRQ_HANDLED; 
    }
    printk("==: " "irq wasn't handled\n");
    return IRQ_NONE; 
}



static int __init my_init(void)
{
    my_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL); 
    if (!my_tasklet)
    {
        printk("==: " "ERROR kmalloc!\n");
        return -1;
    }

    tasklet_init(my_tasklet, my_tasklet_function, (unsigned long)my_tasklet_data);

    if (request_irq(IRQ_NUM, my_irq_handler, IRQF_SHARED, "my_dev_name", &my_irq_handler))
    {
        printk("==: " "ERROR request_irq\n");
        return -1;
    }
    printk("==: " "module loaded\n");
    return 0;
}

static void __exit my_exit(void)
{
    tasklet_kill(my_tasklet);
    kfree(my_tasklet);
    free_irq(IRQ_NUM, &my_irq_handler);
    printk("==: " "module unloaded\n");
}

module_init(my_init) 
module_exit(my_exit)
























/*
sudo rmmod my_tasklet.ko
make disclean
make
sudo insmod my_tasklet.ko
lsmod | grep my_tasklet
sudo dmesg | grep my_tasklet:
// Посмотреть инф-ию о обработчике прерывания. Там CPUi - число прерываний, полученных i-ым процессорным ядром.
cat /proc/interrupts | head -n 1 && cat /proc/interrupts| grep my_dev_name
*/



    // IRQF_SHARED 0x00000080 – устанавливается, чтобы разрешить разделение линии IRQ раными устройствами.
    //(ДРАЙВЕР управляет устройством – разные драйвера устройсств мб заинтересованы в использовании одной и той же линии IRQ).
    // Одно устройство может иметь несколько обработчиков прерываний