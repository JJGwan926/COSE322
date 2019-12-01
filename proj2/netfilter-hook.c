/**
 *  author: Minsu Kim & Wonmi Choi 
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <uapi/linux/netfilter_ipv4.h>

// for parsing Ipv4 address from struct sk_buff
#define NIPQUAD(addr) \
  ((unsigned char *)&addr)[0], \
  ((unsigned char *)&addr)[1], \
  ((unsigned char *)&addr)[2], \
  ((unsigned char *)&addr)[3]
// inet_addr and NIPQUAD are dual
static unsigned int inet_addr(char *ip){
	int a,b,c,d;
	char arr[4];
	sscanf(ip,"%d.%d.%d.%d",&a,&b,&c,&d);
	arr[0] = a;
	arr[1] = b;
 	arr[2] = c; 
	arr[3] = d;
	return *(unsigned int*)arr;
}

// setting for proc file system
#define PROC_DIRNAME    "proj2"
#define PROC_FILENAME   "proj2-procfile"
#define PORT_NUM_SIZE   6
static struct proc_dir_entry *proc_dir;     // proc file system directory
static struct proc_dir_entry *proc_file;    // proc file system file

// variable for port number
static char port_num[PORT_NUM_SIZE];

// utils to convert string to int
// @parameter str must contain numbers only
static unsigned short string2short(char* str) {

  int i;
  unsigned short num = 0;
  int len = 0;

  // get length of string
  for (i=0; (int)'0'<=(int)str[i] && (int)str[i]<=(int)'9'; ++i) {
    len++;
  }

  // convert string to int
  for (i=len-1; i>=0; --i) {
    
    unsigned short c = (unsigned short)str[i];

    if ((unsigned short)'0' <= c && c <= (unsigned short)'9') {
      
      int j;
      int order = 1;
      for (j=0; j<i; ++j) {
        order *= 10;
      }
      num += ((c - (unsigned short)'0') * order);
    } else {
      printk(KERN_INFO "NaN");
      return -1;  // error
    }
  }

  return num;
}

// hook function for monitoring hook point "NF_INET_PRE_ROUTING"
static unsigned int pre_routing_hook_impl(void *priv, 
                                          struct sk_buff *skb, 
                                          const struct nf_hook_state *state) {
  
  // struct sk_buff *skb is packet
  // get ip header to get src/dest IPv4 address
  struct iphdr  *ih = ip_hdr(skb);
  // get tcp header to get src/dest port number
  struct tcphdr *th = tcp_hdr(skb);
  
  // if destination port is 33333, then do forwarding
  if (ntohs(th->dest) == string2short(port_num)) {
    // print captured packet info before manipulation
    printk(KERN_INFO "Before manipulation\n");
    printk(KERN_INFO "  PRE_ROUTING[(%u;%d;%d;%d.%d.%d.%d;%d.%d.%d.%d)]\n", 
            ih->protocol,
            ntohs(th->source),  // 33333
            ntohs(th->dest),    // 33333
            NIPQUAD(ih->saddr),
            NIPQUAD(ih->daddr)
          );
    // manipulate src port number from 33333 to 7777
    th->source = htons(7777);
    // manipulate dest port number from 33333 to 7777
    th->dest = htons(7777);
    /**
     *  !!For Forwarding!!
     *    Since no routing entry's addr matches,
     *    packet would go forward to 'default gateway'
     */
    ih->daddr = inet_addr("111.111.111.111");

    // print manipulated packet info
    printk(KERN_INFO "After manupulation\n");
    printk(KERN_INFO "  PRE_ROUTING[(%u;%d;%d;%d.%d.%d.%d;%d.%d.%d.%d)]\n", 
            ih->protocol,
            ntohs(th->source),  // 7777
            ntohs(th->dest),    // 7777
            NIPQUAD(ih->saddr),
            NIPQUAD(ih->daddr)
          );
  }

  return NF_ACCEPT;   // do forwarding or going up
}

// hook function for monitoring hook point "NF_INET_FORWARD"
static unsigned int forward_hook_impl(void *priv,
                                      struct sk_buff *skb, 
                                      const struct nf_hook_state *state) {
  
  // struct sk_buff *skb is packet
  // get ip header to get src/dest IPv4 address
  struct iphdr  *ih = ip_hdr(skb);
  // get tcp header to get src/dest port number
  struct tcphdr *th = tcp_hdr(skb);

  // print manipulated packet only
  if (ntohs(th->source) == 7777 && ntohs(th->dest) == 7777) {
    // struct sk_buff *skb is packet
    printk(KERN_INFO "  FORWARD[(%u;%d;%d;%d.%d.%d.%d;%d.%d.%d.%d)]\n", 
            ih->protocol,
            ntohs(th->source),  // should be 7777
            ntohs(th->dest),    // should be 7777
            NIPQUAD(ih->saddr),
            NIPQUAD(ih->daddr)
          );
  }
  
  return NF_ACCEPT;   // do forwarding
}

// hook function for monitoring hook point "NF_INET_POST_ROUTING"
static unsigned int post_routing_hook_impl(void *priv, 
                                           struct sk_buff *skb, 
                                           const struct nf_hook_state *state) {

  // struct sk_buff *skb is packet
  // get ip header to get src/dest IPv4 address
  struct iphdr  *ih = ip_hdr(skb);
  // get tcp header to get src/dest port number
  struct tcphdr *th = tcp_hdr(skb);

  // print manipulated packet only
  if (ntohs(th->source) == 7777 && ntohs(th->dest) == 7777) {
    // struct sk_buff *skb is packet
    printk(KERN_INFO "  POST_ROUTING[(%u;%d;%d;%d.%d.%d.%d;%d.%d.%d.%d)]\n", 
              ih->protocol,
              ntohs(th->source),  // should be 7777
              ntohs(th->dest),    // should be 7777
              NIPQUAD(ih->saddr),
              NIPQUAD(ih->daddr)
            );
  }

  return NF_ACCEPT;   // do routing
}

// struct for setting hook at pre routing
static struct nf_hook_ops pre_routing_hook_struct = {
  .hook = pre_routing_hook_impl,    // function to call
  .pf = PF_INET,                    // using TCP/IP protocol
  .hooknum = NF_INET_PRE_ROUTING,   // at NF_INET_PRE_ROUTING (hook point)
  .priority = NF_IP_PRI_FIRST       // set priority (doesn't matter)
};
// struct for setting hook at forward
static struct nf_hook_ops forward_hook_struct = {
  .hook = forward_hook_impl,        // function to call
  .pf = PF_INET,                    // using TCP/IP protocol
  .hooknum = NF_INET_FORWARD,       // at NF_INET_FORWARD (hook point)
  .priority = NF_IP_PRI_FIRST       // set priority (doesn't matter)
};
// struct for setting hook at post routing
static struct nf_hook_ops post_routing_hook_struct = {
  .hook = post_routing_hook_impl,   // function to call
  .pf = PF_INET,                    // using TCP/IP protocol
  .hooknum = NF_INET_PRE_ROUTING,   // at NF_INET_POST_ROUTING (hook point)
  .priority = NF_IP_PRI_FIRST       // set priority (doesn't matter)
};

// custom open function for open proc file
static int my_open(struct inode *inode, struct file *file) {

  // for debugging
  printk(KERN_INFO "Proc File Open!!\n");

  return 0;
}

// custom write function for writing to proc file
static ssize_t my_write(struct file *file, 
                        const char __user *user_buffer, 
                        size_t count, 
                        loff_t *ppos) {

  // for debugging
  printk(KERN_INFO "write");
  // set port number to be victimmed for forwarding
  sprintf(port_num, "%s", user_buffer);
  string2short(port_num);

  return count;
}

// struct for setting proc file system
static const struct file_operations proc_fops = {
  .owner = THIS_MODULE,
  .open = my_open,
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
  nf_register_hook(&pre_routing_hook_struct);
  nf_register_hook(&forward_hook_struct);
  nf_register_hook(&post_routing_hook_struct);
  
  return 0;
}

// module exit
static void __exit hook_exit(void) {
  
  remove_proc_entry(PROC_FILENAME, proc_dir);
  // unregister hook structures from netfilter
  nf_unregister_hook(&pre_routing_hook_struct);
  nf_unregister_hook(&forward_hook_struct);
  nf_unregister_hook(&post_routing_hook_struct);
  
  printk(KERN_INFO "LKM ends..\n");

  return;
}

module_init(hook_init);
module_exit(hook_exit);

MODULE_AUTHOR("2017320168 2017320192");
MODULE_DESCRIPTION("Project2 LKM for Hooking");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");