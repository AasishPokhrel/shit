#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elian Chen");
MODULE_DESCRIPTION("The shit kernel module");
MODULE_VERSION("1.0");

static int __init shit_init(void)
{
    pr_info("Hello shit!\n");
    return 0;
}

static void __exit shit_exit(void)
{
    pr_info("Goodbye shit!\n");
}

module_init(shit_init);
module_exit(shit_exit);