/* hello.c -- a very simple kernel module */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name Here <you@mail.srv>");
MODULE_DESCRIPTION("A simple hello world kernel module.");
MODULE_VERSION("1.0");

static int __init hello_init(void)
{
    printk(KERN_INFO "Hello, kernel world!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye, kernel world!\n");
}

module_init(hello_init);
module_exit(hello_exit);
