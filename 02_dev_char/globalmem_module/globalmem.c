#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define GLOBALMEM_SIZE 0x1000
#define MEM_CLEAR 0x1
#define GLOBALMEM_MAJOR 230

static int globalmem_major = GLOBALMEM_MAJOR; //定义主设备号
module_param(globalmem_major, int, S_IRUGO);//模块传参

struct globalmem_dev {   //定义globalmen_dev结构体
    struct cdev cdev;//字符结构体
    unsigned char mem[GLOBALMEM_SIZE];//使用内存
};

struct globalmem_dev *globalmem_devp;//申明globalmem结构对象


//globalmem设备驱动的读函数
static ssize_t globalmem_read(struct file *filp, char __user * buf, size_t size,
 loff_t * ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct globalmem_dev *dev = filp->private_data;

    if (p >= GLOBALMEM_SIZE)
        return 0;
    if (count > GLOBALMEM_SIZE - p)
        count = GLOBALMEM_SIZE - p;
    if (copy_to_user(buf, dev->mem + p, count)) {
        ret = -EFAULT;
    } else {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "read %u bytes(s) from %lu\n", count, p);
    }
    return ret;
}
//globalmem设备驱动的写函数
static ssize_t globalmem_write(struct file *filp, const char __user * buf,
 size_t size, loff_t * ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct globalmem_dev *dev = filp->private_data;


    if (p >= GLOBALMEM_SIZE)
        return 0;
    if (count > GLOBALMEM_SIZE - p)
        count = GLOBALMEM_SIZE - p;

    if (copy_from_user(dev->mem + p, buf, count))
        ret = -EFAULT;
    else {
        *ppos += count;
        ret = count;

        printk(KERN_INFO "written %u bytes(s) from %lu\n", count, p);
 }
 return ret;
}
//寻址函数
static loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig)
{
    loff_t ret = 0;
    switch (orig) {
        case 0: /* 从文件开头位置seek */
        if (offset< 0) {
            ret = -EINVAL;
            break;
        }
        if ((unsigned int)offset > GLOBALMEM_SIZE) {
            ret = -EINVAL;
            break;
        }
        filp->f_pos = (unsigned int)offset;
        ret = filp->f_pos;
        break;

        case 1: /* 从文件当前位置开始seek */
        if ((filp->f_pos + offset) > GLOBALMEM_SIZE) {
            ret = -EINVAL;
            break;
        }
        if ((filp->f_pos + offset) < 0) {
            ret = -EINVAL;
            break;
        }
        filp->f_pos += offset;
        ret = filp->f_pos;
        break;

        default:
        ret = -EINVAL;
        break;
    }
    return ret;
}

static long globalmem_ioctl(struct file *filp, unsigned int cmd,
 unsigned long arg)
{
    struct globalmem_dev *dev = filp->private_data;
    switch (cmd) {
        case MEM_CLEAR:
        memset(dev->mem, 0, GLOBALMEM_SIZE);
        printk(KERN_INFO "globalmem is set to zero\n");
        break;

        default:
        return -EINVAL;
    }

    return 0;
}
//open函数
static int globalmem_open(struct inode *inode, struct file *filp)
{
    filp->private_data = globalmem_devp;
    return 0;
}

//release函数
static int globalmem_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/*定义字符结构体方法*/
static const struct file_operations globalmem_fops = {
    .owner = THIS_MODULE,
    .llseek = globalmem_llseek,
    .read = globalmem_read,
    .write = globalmem_write,
    .unlocked_ioctl = globalmem_ioctl,
    .open = globalmem_open,
    .release = globalmem_release,
};
//字符设备加载函数
static void globalmem_setup_cdev(struct globalmem_dev *dev, int index)  
{
    int err, devno = MKDEV(globalmem_major, index);//获取设备结构体dev_t

    cdev_init(&dev->cdev, &globalmem_fops);//初始化字符设备和字符设备处理方法
    dev->cdev.owner = THIS_MODULE;//初始化字符设备所属模块
    err = cdev_add(&dev->cdev, devno, 1);//添加一个字符设备
    if (err)
        printk(KERN_NOTICE "Error %d adding globalmem%d", err, index);
}

//模块初始化
static int __init globalmem_init(void) //初始化模块
{
    int ret;
    dev_t devno = MKDEV(globalmem_major, 0);//获取字符设备结构体
    if (globalmem_major)
        ret = register_chrdev_region(devno, 1, "globalmem");//注册此cdev设备
    else {
        ret = alloc_chrdev_region(&devno, 0, 1, "globalmem");//申请字符设备cdev空间
        globalmem_major = MAJOR(devno);//获取主设备号
    }
    if (ret < 0)
        return ret;
    globalmem_devp = kzalloc(sizeof(struct globalmem_dev), GFP_KERNEL);//分配globalmem结构体内存
    if (!globalmem_devp) {
        ret = -ENOMEM;
        goto fail_malloc;    //分配失败择跳转
    }

    //主次设备的不同
    globalmem_setup_cdev(globalmem_devp, 0);
    return 0;
fail_malloc:
    unregister_chrdev_region(devno, 1);
    return ret;
}

//模块卸载函数
static void __exit globalmem_exit(void)
{
    cdev_del(&globalmem_devp->cdev);
    kfree(globalmem_devp);
    unregister_chrdev_region(MKDEV(globalmem_major, 0), 1);
}

module_init(globalmem_init);
module_exit(globalmem_exit);

