#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>

#define TDEV_PROC "tdev_op"
#define to_test_device(d) container_of(d, struct test_device, dev)

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

struct test_device {
    struct device dev;
    int ref;
};

struct class *test_class = NULL;
static int id = 0;
static char op_buf[64];
struct test_device *g_tdev = NULL;


static void tdev_del(void);
static void tdev_add(void);

static ssize_t tdev_read(struct file *file, char __user *buf,
						size_t count, loff_t *ppos)
{
    char *op = "tdev_op";
	size_t len;

    printk(KERN_INFO "read -> pos %ld, count %zu\n", (long)*ppos, count);
    if (*ppos != 0)
        return 0;

	len = strlen(op) + 1;
    len = MIN(len, count);
	if (copy_to_user(buf, op, len)) {
        printk(KERN_ERR "copy to user failed\n");
        return -EFAULT;
    }
	return len;
}

static ssize_t tdev_write(struct file *file, const char __user *buf,
						size_t count, loff_t *ppos)
{
	size_t len;

    printk(KERN_INFO "write <- pos %ld, count %zu\n", (long)*ppos, count);

    memset(op_buf, 0, sizeof(op_buf));
    len = sizeof(op_buf);
    len = MIN(len, count);
    if (copy_from_user(&op_buf, buf, len) != 0) {
        printk(KERN_ERR "copy from user failed\n");
        return -EFAULT;
    }

    printk(KERN_INFO "write %s\n", op_buf);
    if (!strncmp(op_buf, "add", 3)) {
        tdev_add();
    } else if (!strncmp(op_buf, "del", 3)) {
        tdev_del();
    } else if (!strncmp(op_buf, "put", 3)) {

    } else if (!strncmp(op_buf, "get", 3)) {

    }

	return len;
}

static const struct file_operations test_device_op = {
	.owner		= THIS_MODULE,
	.read		= tdev_read,
	.write		= tdev_write,
	.llseek		= noop_llseek,
};

static ssize_t ref_show(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	struct test_device *tdev = to_test_device(dev);

	return sprintf(buf, "%d\n",
		       tdev->ref);
}

static DEVICE_ATTR_RO(ref);

static struct attribute *tdev_attrs[] = {
	&dev_attr_ref.attr,
	NULL,
};
ATTRIBUTE_GROUPS(tdev);


static struct test_device *test_device_create(void)
{
    struct test_device *tdev;

    tdev = kzalloc(sizeof(tdev), GFP_KERNEL);
    if (!tdev) {
        return NULL;
    }
    device_initialize(&tdev->dev);
    tdev->dev.class = test_class;
    tdev->dev.id = id;
    dev_set_name(&tdev->dev, "tdev%d", id);
    id++;
    return tdev;
}


static void test_device_free(struct test_device *tdev)
{
    printk(KERN_INFO "test_device_free IN\n");

    if (tdev->ref != 0) {
        printk(KERN_ERR "device ref %d, not equal zero", tdev->ref);
    }
    kfree(tdev);
}

static void tdev_add(void) 
{
    printk(KERN_INFO "%s IN\n", __FUNCTION__);

    if (g_tdev != NULL) {
        return;
    }

    g_tdev = test_device_create();
    if (g_tdev == NULL) {
        printk(KERN_ERR "create tdev failed\n");
        return;
    }
    if (device_add(&g_tdev->dev) != 0) {
        put_device(&g_tdev->dev);
        printk(KERN_ERR "add device failed\n");
        return;
    }
}

static void tdev_del(void)
{
    printk(KERN_INFO "%s IN\n", __FUNCTION__);
    if (g_tdev == NULL) {
        return;
    }
    put_device(&g_tdev->dev);

}

static void test_device_release(struct device *dev)
{
    struct test_device *tdev = to_test_device(dev);

    printk(KERN_INFO "test_device_release IN\n");
    test_device_free(tdev);
}

static int test_init(void)
{
    test_class = class_create(THIS_MODULE, "test_put_device");
    if (IS_ERR(test_class)) {
        return PTR_ERR(test_class);
    }

	if (proc_create(TDEV_PROC, 0666, NULL, &test_device_op) == NULL) {
        kfree(test_class);
		return -ENOMEM;
	}

    test_class->dev_groups = tdev_groups;
    test_class->dev_release = test_device_release;
    return 0;
}

static void __exit test_exit(void)
{
    remove_proc_entry(TDEV_PROC, NULL);
    class_destroy(test_class);
}

subsys_initcall(test_init)
module_exit(test_exit)


MODULE_DESCRIPTION("put_device condiction test");
MODULE_AUTHOR("joans <xjq9296@gmail.com>");
MODULE_LICENSE("GPL v2");
