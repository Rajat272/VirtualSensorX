#include "vsensor_internal.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antigravity Engineer");
MODULE_DESCRIPTION("VirtualSensorX Kernel Module");
MODULE_VERSION("1.0");

#define DEVICE_NAME "virtualsensor"
#define CLASS_NAME  "virtualsensor"

struct vs_device *g_vs_dev = NULL;

static const struct file_operations vs_fops = {
	.owner          = THIS_MODULE,
	.open           = vs_open,
	.release        = vs_release,
	.read           = vs_read,
	.write          = vs_write,
	.unlocked_ioctl = vs_ioctl,
	.poll           = vs_poll,
};

static int __init vsensor_init(void)
{
	int ret;

	pr_info("virtualsensor: initializing module\n");

	g_vs_dev = kzalloc(sizeof(*g_vs_dev), GFP_KERNEL);
	if (!g_vs_dev)
		return -ENOMEM;

	mutex_init(&g_vs_dev->lock);
	init_waitqueue_head(&g_vs_dev->read_wait);

	vs_ringbuf_init(&g_vs_dev->ring);
	vs_state_init(g_vs_dev);
	vs_gen_init(g_vs_dev);

	ret = alloc_chrdev_region(&g_vs_dev->dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		pr_err("virtualsensor: failed to allocate chrdev region\n");
		goto err_free_dev;
	}

	cdev_init(&g_vs_dev->cdev, &vs_fops);
	g_vs_dev->cdev.owner = THIS_MODULE;

	ret = cdev_add(&g_vs_dev->cdev, g_vs_dev->dev_num, 1);
	if (ret < 0) {
		pr_err("virtualsensor: failed to add cdev\n");
		goto err_unregister_chrdev;
	}

	g_vs_dev->class = class_create(CLASS_NAME);
	if (IS_ERR(g_vs_dev->class)) {
		ret = PTR_ERR(g_vs_dev->class);
		pr_err("virtualsensor: failed to create device class\n");
		goto err_cdev_del;
	}

	g_vs_dev->device = device_create(g_vs_dev->class, NULL, g_vs_dev->dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(g_vs_dev->device)) {
		ret = PTR_ERR(g_vs_dev->device);
		pr_err("virtualsensor: failed to create device\n");
		goto err_class_destroy;
	}

	pr_info("virtualsensor: device /dev/%s created successfully (major %d, minor %d)\n",
		DEVICE_NAME, MAJOR(g_vs_dev->dev_num), MINOR(g_vs_dev->dev_num));

	return 0;

err_class_destroy:
	class_destroy(g_vs_dev->class);
err_cdev_del:
	cdev_del(&g_vs_dev->cdev);
err_unregister_chrdev:
	unregister_chrdev_region(g_vs_dev->dev_num, 1);
err_free_dev:
	kfree(g_vs_dev);
	g_vs_dev = NULL;
	return ret;
}

static void __exit vsensor_exit(void)
{
	pr_info("virtualsensor: cleaning up module\n");

	if (g_vs_dev) {
		vs_gen_cleanup(g_vs_dev);

		/* Wake any waiting readers so they exit cleanly */
		wake_up_interruptible_all(&g_vs_dev->read_wait);

		device_destroy(g_vs_dev->class, g_vs_dev->dev_num);
		class_destroy(g_vs_dev->class);
		cdev_del(&g_vs_dev->cdev);
		unregister_chrdev_region(g_vs_dev->dev_num, 1);
		kfree(g_vs_dev);
		g_vs_dev = NULL;
	}

	pr_info("virtualsensor: module unloaded\n");
}

module_init(vsensor_init);
module_exit(vsensor_exit);
