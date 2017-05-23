#include <linux/kernel.h>	/* KERN_INFO */
#include <linux/module.h>
#include <linux/init.h>		/* Makra */
#include <linux/fs.h> /*file_operations, inode*/
#include <linux/errno.h>
#include <linux/slab.h> /*kmalloc, kfree*/
#include <linux/gfp.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>	/*put_user*/


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple ramdisk");

static int ramdisk_open(struct inode *inode, struct file *file);
static int ramdisk_release(struct inode *inode, struct file *file);
static ssize_t ramdisk_read(struct file *file, char *buffer, size_t length, loff_t *offset);
static ssize_t ramdisk_write(struct file *file, const char *buffer, size_t length, loff_t *offset);
static loff_t ramdisk_llseek(struct file *file, loff_t offset, int whence);
//static int ramdisk_mmap(struct file *file, struct vm_area_struct *str);

static int proc_open(struct inode *inode, struct file *file);
static int proc_release(struct inode *inode, struct file *file);
static ssize_t proc_read(struct file *file, char *buffer, size_t length, loff_t *offset);
static ssize_t proc_write(struct file *file, const char *buffer, size_t length, loff_t *offset);


static int major;
static int opened;

static char *tab;
static size_t tab_size;
static char *position;

static struct proc_dir_entry *proc;
static char proc_buff[10];

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = ramdisk_read,
	.write = ramdisk_write,
	.open = ramdisk_open,
	.release = ramdisk_release,
	.llseek = ramdisk_llseek/*,
	.mmap = ramdisk_mmap*/
};

static struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.open = proc_open,
	.release = proc_release,
	.write = proc_write
};

static int __init ramdisk_init(void) { //Do wersji 2.2 jądra było init_module
	printk(KERN_INFO "ramdisk: init\n");

	major = __register_chrdev(0, 0, 1, "ramdisk", &fops);

	if (major < 0) {
		printk(KERN_ALERT "ramdisk: register_chardev failed with %d error\n", major);
		return major;
	}
	
	printk(KERN_INFO "ramdisk: got %d major number form kernel\n", major);
	printk(KERN_INFO "ramdisk: now create dev file with 'mknod /dev/ramdisk c %d 0'\n", major);
	opened = 0;
	
	proc = proc_create("ramdisk_size", 0644, NULL, &proc_fops);
	if (proc == NULL) {
		proc_remove(proc);
		printk(KERN_ALERT "ramdisk: Could not initialize proc file\n");
		return -ENOMEM;
	}
	
	printk(KERN_INFO "ramdisk: /proc/ramdisk_size created\n");
	
	
	tab = kcalloc(1024, sizeof(char), GFP_KERNEL);
	if(tab == NULL) {
		printk(KERN_ALERT "ramdisk: can't get ram\n");
		return -ENOMEM;
	}
	tab_size = ksize(tab);
	printk(KERN_INFO "ramdisk: allocated %d bytes of memory\n", tab_size);

	return 0;
}


static void __exit ramdisk_cleanup(void) { //Do wersji 2.2 jądra było cleanup_module
	proc_remove(proc);
	kfree(tab);
	printk(KERN_INFO "ramdisk: cleanup\n");
	__unregister_chrdev(major, 0, 1, "ramdisk");
	return;
}

static int ramdisk_open(struct inode *inode, struct file *file) {
	try_module_get(THIS_MODULE);
	if(opened) {
		module_put(THIS_MODULE);
		return -EBUSY;
	}
	position = tab;
	opened++;
	return 0;
}

static int ramdisk_release(struct inode *inode, struct file *file) {
	opened--;
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t ramdisk_read(struct file *file, char *buffer, size_t length, loff_t *offset) {
	size_t tab_left = tab_size - *offset;
	size_t to_cpy = (length > tab_left) ? tab_left : length;
	
	if( _copy_to_user(buffer, position, to_cpy) != 0)
		return -EPERM;
		
	*offset += to_cpy;
	position += to_cpy;
	return to_cpy;
}

static ssize_t ramdisk_write(struct file *file, const char *buffer, size_t length, loff_t *offset){
	size_t tab_left = tab_size - *offset;
	size_t to_cpy = (length > tab_left) ? tab_left : length;
	if(tab_left <= 0) return -ENOSPC;

	if( _copy_from_user(position, buffer, to_cpy) != 0)
		return -EPERM;
		
	*offset += to_cpy;
	position += to_cpy;
	return to_cpy;
}

static loff_t ramdisk_llseek(struct file *file, loff_t off, int whence) {
    loff_t newpos;

    switch(whence) {
      case 0: /* SEEK_SET */
        newpos = off;
        break;

      case 1: /* SEEK_CUR */
        newpos = file->f_pos + off;
        break;

      case 2: /* SEEK_END */
        newpos = tab_size + off;
        break;

      default: /* can't happen */
        return -EINVAL;
    }
    if (newpos < 0 || newpos > tab_size ) return -EINVAL;
    file->f_pos = newpos;
    return newpos;

}
/*
static int ramdisk_mmap(struct file *file, struct vm_area_struct *str) {
	if (remap_pfn_range(vma, vma->vm_start, vm->vm_pgoff, vma->vm_end - vma->vm_start, vma->vm_page_prot))
		return -EAGAIN;

    simple_vma_open(vma);
    return 0;
}
*/

static int proc_open(struct inode *inode, struct file *file) {
	try_module_get(THIS_MODULE);
	if(opened) {
		module_put(THIS_MODULE);
		return -EBUSY;
	}
	opened++;
	return 0;
}

static int proc_release(struct inode *inode, struct file *file) {
	opened--;
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t proc_read(struct file *file, char *buffer, size_t length, loff_t *offset) {
	size_t to_cpy;
	if(*offset > 0)
		return 0;
	to_cpy = sprintf(proc_buff, "%u", tab_size);
	
	if( _copy_to_user(buffer, proc_buff, to_cpy) != 0)
		return -EPERM;

	*offset += to_cpy;
	return to_cpy;
}

static ssize_t proc_write(struct file *file, const char *buffer, size_t length, loff_t *offset){
	size_t new_tab_size;
	char *tmp_tab;
	
	if(length > 100) return -ENOSPC;
	if( _copy_from_user(proc_buff, buffer, length) != 0) return -EPERM;
		
	sscanf(proc_buff, "%u", &new_tab_size);
	
	printk(KERN_INFO "ramdisk: new_tab_size %d bytes\n", new_tab_size);
	tmp_tab = kcalloc(new_tab_size, sizeof(char), GFP_KERNEL);
	if(tmp_tab == NULL) {
		printk(KERN_ALERT "ramdisk: can't get ram\n");
		return -ENOMEM;
	}
	new_tab_size = ksize(tmp_tab);
	strncpy(tmp_tab, tab, (new_tab_size > tab_size) ? tab_size : new_tab_size);
	kfree(tab);
	
	tab = tmp_tab;
	tab_size = new_tab_size;
	printk(KERN_INFO "ramdisk: allocated %d bytes of memory\n", tab_size);

	return length;
}

module_init(ramdisk_init);
module_exit(ramdisk_cleanup);
