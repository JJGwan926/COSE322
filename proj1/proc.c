/**
 *  author: minsu kim 
 *  date: 2019.10.29
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <asm/uaccess.h>

#define PROC_DIRNAME    "proj1"             // proc file system directory name
#define PROC_FILENAME   "proj1file"         // proc file system file name
#define Q_SIZE          800               // circular queue size

static struct proc_dir_entry *proc_dir;     // proc file system directory
static struct proc_dir_entry *proc_file;    // proc file system file

// struct Info_node is one node for circular queue
// same as kernel's Info_node struct
struct Info_node {
	unsigned long long block_no;    // 1. block number
	struct timeval time;            // 2. write time
	const char* name;               // 3. file system name
};
extern struct Info_node Qnode[Q_SIZE];  // get kernel-defined circular queue
extern int cnt;                         // index for circular queue's current value

struct tm   times;                  // struct tm is for formatting time data in Info_node variable
char        info[Q_SIZE][100];      // info is temporary variable for connecting kernel-user memory

// custom open function for open proc file
static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Proc File Open!!\n");

    return 0;
}

// custom read function for reading proc file
static ssize_t my_read(struct file *file, char *buffer, size_t length, loff_t *offset) {

    // copy kernel's data into user buffer
    if ( copy_to_user(buffer, info, sizeof(info)) ) {   // copy_to_user returns 0 in error
      return -EFAULT;
    }

    return length;
}

// custom write function for writing to proc file
static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos) {

    int     i;


    // write data into queues
    for (i=cnt+1; i<Q_SIZE; ++i) {
        time_to_tm(Qnode[i].time.tv_sec, 0, &times);
        sprintf(info[i], "File System: %s, Block Number: %lld, Time: %d:%d:%d:%ld\n", Qnode[i].name, Qnode[i].block_no, times.tm_hour, times.tm_min, times.tm_sec, Qnode[i].time.tv_usec);
    }
    for (i=0; i<=cnt; ++i) {
        time_to_tm(Qnode[i].time.tv_sec, 0, &times);
        sprintf(info[i], "File System: %s, Block Number: %lld, Time: %d:%d:%d:%ld\n", Qnode[i].name, Qnode[i].block_no, times.tm_hour, times.tm_min, times.tm_sec, Qnode[i].time.tv_usec);
    }

    return count;
}

static const struct file_operations myproc_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_read,
    .write = my_write,
};

static int __init simple_init(void) {
    printk(KERN_INFO "Simple Module Init!!\n");

    // make directory in proc file system
    proc_dir = proc_mkdir(PROC_DIRNAME, NULL); 
    // create proc file in proc file system
    proc_file = proc_create(PROC_FILENAME, 0600, proc_dir, &myproc_fops);

    return 0;
}

static void __exit simple_exit(void) {
    printk(KERN_INFO "Simple Module Exit!!\n");

    return ;
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("2017320168 2017320192");
MODULE_DESCRIPTION("Project1 LKM for profiling file systems");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");