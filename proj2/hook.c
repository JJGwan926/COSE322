/**
 *  author: Minsu Kim & Wonmi Choi 
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/netfilter.h>

#define PROC_DIRNAME    "proj2"
#define PROC_FILENAME   "proj2-procfile"

static unsigned int hook_impl(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
  /**
   *  Todo
   */
}

static struct nf_hook_ops hook_struct {
  .hook = hook_impl
  /**
   *  Todo
   */
}



static const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_read,
    .write = my_write,
};

static int __init hook_init(void) {
  // make directory in proc file system
  proc_dir = proc_mkdir(PROC_DIRNAME, NULL); 
  // create proc file in proc file system
  proc_file = proc_create(PROC_FILENAME, 0600, proc_dir, &proc_fops);
  nf_register_hook(&hook_struct);
  return 0;
}

static void __exit hook_exit(void) {
  remove_proc_entry(PROC_FILENAME, PROC_DIRNAME);
  nf_unregister_hook(&hook_struct);
}

module_init(hook_init);
module_exit(hook_exit);

MODULE_AUTHOR("2017320168 2017320192");
MODULE_DESCRIPTION("Project2 LKM for Hooking");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");