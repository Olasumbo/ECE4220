

#ifndef MODULE
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");


int init_module(void)
{
        printk("HEllO WORLD");
        
        return 0;
}

void cleanup_module(void)
{
        printk(" Goodbye");
}