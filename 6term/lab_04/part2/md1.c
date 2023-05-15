#include <linux/init.h>
#include <linux/module.h>

#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chepigo Darya");

char* md1_data = "Привет мир!";

extern char* md1_proc(void) { return md1_data; }

//локальное имя непригодное для связывания
static char* md1_local(void) { return md1_data; }

// не объявлено как экспортируемое имя и не может быть использовано вне модуля
// EXPORT_SYMBOL() помогает вам предоставлять API для других модулей / кода.
extern char* md1_noexport(void) { return md1_data; }

// так определяются данные, используемые другими модулями.
// функции local и noexport не экпортируются (хотя и md1_noexport extern)
EXPORT_SYMBOL(md1_data);
EXPORT_SYMBOL(md1_proc);

static int __init md_init(void) {
  printk("+ module md1 start!\n");
  printk("+ module md1 use local from md1: %s\n", md1_local());
  printk("+ module md1 use noexport from md1: %s\n", md1_noexport());
  return 0;
}

static void __exit md_exit(void) { printk("+ module md1 unloaded!\n"); }

module_init(md_init);
module_exit(md_exit);