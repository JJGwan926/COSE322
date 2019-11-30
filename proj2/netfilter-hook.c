/**
 *  author: Minsu Kim & Wonmi Choi 
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/netfilter.h>

// setting for proc file system
#define PROC_DIRNAME    "proj2"
#define PROC_FILENAME   "proj2-procfile"
#define PORT_NUM_SIZE   5
static struct proc_dir_entry *proc_dir;     // proc file system directory
static struct proc_dir_entry *proc_file;    // proc file system file

// variable for port number
char        port_num[PORT_NUM_SIZE];
static int  finished = 0;

/*
// hook function for monitoring hook point "NF_INET_PRE_ROUTING"
// at this point, 
static unsigned int pre_routing_hook_impl(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
  
  // struct sk_buff *skb is packet
}
// hook function for monitoring hook point "NF_INET_FORWARD"
static unsigned int forward_hook_impl(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
  
  // struct sk_buff *skb is packet
}

// struct for setting hook at pre_routing
static struct nf_hook_ops pre_routing_hook_struct {
  .hook = pre_routing_hook_impl,    // function to call
  .pf = PF_INET,                    // using TCP/IP protocol
  .hooknum = NF_INET_PRE_ROUTING,   // at NF_INET_PRE_ROUTING (hook point)
  .priority = NF_IP_PRI_FIRST       // set priority (doesn't matter)
};
// struct for setting hook at forward
static struct nf_hook_ops forward_hook_struct {
  .hook = forward_hook_impl,        // function to call
  .pf = PF_INET,                    // using TCP/IP protocol
  .hooknum = NF_INET_FORWARD,       // at NF_INET_FORWARD (hook point)
  .priority = NF_IP_PRI_FIRST       // set priority (doesn't matter)
}
*/
// custom open function for open proc file
static int my_open(struct inode *inode, struct file *file) {

  // for debugging
  printk(KERN_INFO "Proc File Open!!\n");

  return 0;
}

// custom read function for reading proc file
static ssize_t my_read(struct file *file, char *buffer, size_t length, loff_t *offset) {

  if (finished == 1) {
    return 0;
  }

  printk(KERN_INFO "read");
  // copy kernel's data into user buffer if written
  if ( copy_to_user(buffer, port_num, PORT_NUM_SIZE) ) {   // copy_to_user returns 0 in error
    return -EFAULT;
  }

  finished = 1;

  return length;
}

// custom write function for writing to proc file
static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos) {

  printk(KERN_INFO "write");
  // set port number to be used
  sprintf(port_num, "7777");

  return count;
}

// struct for setting proc file system
static const struct file_operations proc_fops = {
  .owner = THIS_MODULE,
  .open = my_open,
  .read = my_read,
  .write = my_write,
};

// module init
static int __init hook_init(void) {

  printk(KERN_INFO "LKM starts!!\n");
  // make directory in proc file system
  proc_dir = proc_mkdir(PROC_DIRNAME, NULL);
  // create proc file in proc file system
  proc_file = proc_create(PROC_FILENAME, 0600, proc_dir, &proc_fops);
  // register hook structures to netfilter
  /*
  nf_register_hook(&pre_routing_hook_struct);
  nf_register_hook(&forward_hook_impl);
  */
  return 0;
}

// module exit
static void __exit hook_exit(void) {
  remove_proc_entry(PROC_FILENAME, proc_dir);
  // unregister hook structures from netfilter
  /*
  nf_unregister_hook(&pre_routing_hook_struct);
  nf_unregister_hook(&forward_hook_impl);
  */
  printk(KERN_INFO "LKM ends..\n");
}

module_init(hook_init);
module_exit(hook_exit);

MODULE_AUTHOR("2017320168 2017320192");
MODULE_DESCRIPTION("Project2 LKM for Hooking");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");