/*
 *	Generic parts
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/llc.h>
#include <net/llc.h>
#include <net/stp.h>

#ifdef CONFIG_BR_TM_ACCURATE_CONTROL
#include <linux/netlink.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#define NETLINK_TM 21
#endif

#include "br_private.h"

#ifdef CONFIG_DNI_MCAST_TO_UNICAST
#include <linux/proc_fs.h>
struct __mac_cache mac_cache[MCAST_ENTRY_SIZE];
int igmp_snoop_enable;
struct __mgroup_list *mhead = NULL;
static struct proc_dir_entry *mcast_proc_entry;
static struct proc_dir_entry *igmp_snoop_proc_entry;
static struct proc_dir_entry *mcast_set_proc_entry;
extern struct proc_dir_entry proc_root;
#endif

#ifdef CONFIG_BR_TM_ACCURATE_CONTROL
unsigned long long tm_limit = 0xffffffffffffffff;
int need_back;
int msg_recive = 0;
int br_tm_dir;
unsigned long long counter_rx = 0;
unsigned long long counter_tx = 0;
struct sock *nl_sk = NULL;
int need_drop = 0;
#endif


int (*br_should_route_hook)(struct sk_buff *skb);

#ifdef CONFIG_DNI_MCAST_TO_UNICAST
unsigned long 
a2n(char *addr)
{
      int i1 = 0, i2 = 0, i3 = 0, i4 = 0;
      sscanf(addr,"%d.%d.%d.%d", &i1, &i2, &i3, &i4);
      return (unsigned long)( (i1<<24) | (i2<<16) | (i3<<8) | (i4));
}

int mcast_read( char *page, char **start, off_t off,
                                int count, int *eof, void *data )
{
      struct __mgroup_list *ptr = mhead;
      struct __mgroup_mbr_list *mptr;
      int i;
      while (ptr)
      {
              printk("group %8x has wireless member :\n", ptr->gip);
              mptr = ptr->member;
              while (mptr)
              {
                      printk("client : %8x\n", mptr->sip);
                      mptr = mptr->next;
              }
              ptr = ptr->next;
      }
      for (i = 0; i < MCAST_ENTRY_SIZE; i++)
      {
              if (mac_cache[i].valid)
                      printk("mac cache entry %d for %x (%2x:%2x:%2x:%2x:%2x:%2x)\n",
                      i, mac_cache[i].sip, mac_cache[i].mac[0], mac_cache[i].mac[1], mac_cache[i].mac[2],
                      mac_cache[i].mac[3], mac_cache[i].mac[4], mac_cache[i].mac[5]);
      }
      return 0;
}

ssize_t mcast_write( struct file *filp, const char __user *buff,
                                         unsigned long len, void *data )
{
      char line[32];
	char *ptr, *tmp = line;
      unsigned long ip, gip;
      if (copy_from_user( line, buff, len ))
              return -EFAULT;
      line[len] = 0;
      ptr = strsep(&tmp, " ");
      switch(line[0])
      {
              case 'a':
              case 'd':
                      ptr = strsep(&tmp, " ");
                      ip = a2n(ptr);
                      gip = a2n(tmp);
                      proc_mcast_entry(line[0], ip, gip);
                      break;
              default:
                      break;
      }
      return len;
}

int igmp_snoop_read( char *page, char **start, off_t off,
                                int count, int *eof, void *data )
{
      printk("IGMP Snoop %s.\n", (igmp_snoop_enable)?"Enable":"Disable");
      return 0;
}

ssize_t igmp_snoop_write( struct file *filp, const char __user *buff,
                                         unsigned long len, void *data )
{
      char line[4];
      if (copy_from_user( line, buff, len ))
	return -EFAULT;
      igmp_snoop_enable = line[0] - '0';
      return len;
}

void create_porc_entry(void)
{
      mcast_proc_entry = create_proc_entry(BR_PROC_MCAST_NAME, 0666, NULL);

      if(mcast_proc_entry) {
              mcast_proc_entry->read_proc = mcast_read;
              mcast_proc_entry->write_proc = mcast_write;
      }

      igmp_snoop_proc_entry = create_proc_entry(BR_IGMP_SNOOP_NAME, 0666, NULL);

      if(igmp_snoop_proc_entry) {
              igmp_snoop_proc_entry->read_proc = igmp_snoop_read;
              igmp_snoop_proc_entry->write_proc = igmp_snoop_write;
      }

      memset(mac_cache, 0, MCAST_ENTRY_SIZE* sizeof(struct __mac_cache));
      igmp_snoop_enable = 0;

      mcast_set_proc_entry = create_proc_entry(BR_MCAST_SET_NAME, 0666, NULL);

      if (mcast_set_proc_entry) {
              mcast_set_proc_entry->read_proc = mcast_set_read;
              mcast_set_proc_entry->write_proc = mcast_set_write;
      }

}

void delete_porc_entry(void)
{
      remove_proc_entry(BR_PROC_MCAST_NAME, &proc_root);
}
#endif
static const struct stp_proto br_stp_proto = {
	.rcv	= br_stp_rcv,
};

static struct pernet_operations br_net_ops = {
	.exit	= br_net_exit,
};
#ifdef CONFIG_BR_TM_ACCURATE_CONTROL
void test_netlink(struct sk_buff *__skb)
{
        struct sk_buff * skb = NULL;
        struct nlmsghdr * nlh = NULL;
        struct completion notify;
        struct msg_data *msgdata;
        struct msg_data sendata;
        int err;
        int p;

        skb = skb_get(__skb);
        if(skb == NULL || skb->len < NLMSG_LENGTH(0)){
                printk("Kernel recive message ERROR ...\n");
        }

        nlh = nlmsg_hdr(skb);
        msgdata = (struct msg_data *)NLMSG_DATA(nlh);
	if(msgdata->tm_dir == 2 || msgdata->tm_dir == 3)
		tm_limit == 0xffffffffffffffff;
	else
        	tm_limit = msgdata->leftdata;
        br_tm_dir = msgdata->tm_dir;
        kfree_skb(skb);
        msg_recive = 1;
	if(msgdata->tm_dir == 3)
		need_drop = 1;
	else 
		need_drop = 0;
        printk("Left volume is %llu dir is %d ...\n",tm_limit,br_tm_dir);

       counter_rx = 0;
       counter_tx = 0;
}
#endif

#ifdef CONFIG_DNI_IPTV_PORT_SUPPORT
int iptv_port_enable = 0;//disable in default
struct __iptv_port_mgroup iptv_port_mgroup[IPTV_PORT_GROUP_MAX];
struct timer_list iptv_port_timer;
static struct proc_dir_entry *iptv_port_proc_entry;
static struct proc_dir_entry *iptv_enable_proc_entry;
static void iptv_port_mgroup_init(void)
{
	int i;
	for(i=0; i<IPTV_PORT_GROUP_MAX; i++)
	{
		iptv_port_mgroup[i].group_ip = 0;
		*(iptv_port_mgroup[i].port_name) = '\0';
		iptv_port_mgroup[i].group_time = 0;
	}
}
static int iptv_port_mgroup_read( char *page, char **start, off_t off,
					int count, int *eof, void *data )
{
	iptv_port_print_mgroup();
	return 0;
}
static ssize_t iptv_port_enable_write( struct file *filp, const char __user *buff,
					unsigned long len, void *data )
{
	char line[4];
	if (copy_from_user( line, buff, len ))
		 return -EFAULT;
	iptv_port_enable = line[0] - '0';
	return len;
}
static int iptv_port_enable_read( char *page, char **start, off_t off,
					int count, int *eof, void *data )
{
	printk("Brige IPTV port status: %d(0:Disable, 1:Enable).\n", iptv_port_enable);
	return 0;
}
static void iptv_port_create_proc_entry(void)
{
	iptv_port_proc_entry = create_proc_entry(BR_PROC_IPTV_PORT, 0666, NULL);
	if(iptv_port_proc_entry)
	{
		iptv_port_proc_entry->read_proc = iptv_port_mgroup_read;
	}
	iptv_enable_proc_entry = create_proc_entry(BR_PROC_IPTV_ENABLE, 0666, NULL);
	if(iptv_enable_proc_entry)
	{
		iptv_enable_proc_entry->write_proc = iptv_port_enable_write;
		iptv_enable_proc_entry->read_proc = iptv_port_enable_read;
	}
}
static void iptv_port_delete_proc_entry(void)
{
	remove_proc_entry(BR_PROC_IPTV_PORT, &proc_root);
}
#endif

static int __init br_init(void)
{
	int err;

	err = stp_proto_register(&br_stp_proto);
	if (err < 0) {
		printk(KERN_ERR "bridge: can't register sap for STP\n");
		return err;
	}
#ifdef CONFIG_BR_TM_ACCURATE_CONTROL
        nl_sk = netlink_kernel_create(&init_net, NETLINK_TM,1,test_netlink,NULL,THIS_MODULE);
        if(nl_sk < 0){
                printk("netlink_kernel_create error ...\n");
        }
#endif
	err = br_fdb_init();
	if (err)
		goto err_out;

	err = register_pernet_subsys(&br_net_ops);
	if (err)
		goto err_out1;

	err = br_netfilter_init();
	if (err)
		goto err_out2;

	err = register_netdevice_notifier(&br_device_notifier);
	if (err)
		goto err_out3;

	err = br_netlink_init();
	if (err)
		goto err_out4;

	brioctl_set(br_ioctl_deviceless_stub);
	br_handle_frame_hook = br_handle_frame;

#if defined(CONFIG_ATM_LANE) || defined(CONFIG_ATM_LANE_MODULE)
	br_fdb_test_addr_hook = br_fdb_test_addr;
#endif

#ifdef CONFIG_DNI_MCAST_TO_UNICAST
	create_porc_entry();
#endif

#ifdef CONFIG_DNI_IPTV_PORT_SUPPORT
	iptv_port_create_proc_entry();
	iptv_port_mgroup_init();
	iptv_port_timer_thread();
#endif

#ifdef CONFIG_BRIDGE_NETGEAR_ACL
	br_acl_init();
#endif

	return 0;
err_out4:
	unregister_netdevice_notifier(&br_device_notifier);
err_out3:
	br_netfilter_fini();
err_out2:
	unregister_pernet_subsys(&br_net_ops);
err_out1:
	br_fdb_fini();
err_out:
	stp_proto_unregister(&br_stp_proto);
	return err;
}

static void __exit br_deinit(void)
{
	stp_proto_unregister(&br_stp_proto);

	br_netlink_fini();
	unregister_netdevice_notifier(&br_device_notifier);
	brioctl_set(NULL);

	unregister_pernet_subsys(&br_net_ops);

	rcu_barrier(); /* Wait for completion of call_rcu()'s */

	br_netfilter_fini();
#if defined(CONFIG_ATM_LANE) || defined(CONFIG_ATM_LANE_MODULE)
	br_fdb_test_addr_hook = NULL;
#endif

	br_handle_frame_hook = NULL;
	br_fdb_fini();

#ifdef CONFIG_DNI_MCAST_TO_UNICAST
      delete_porc_entry();
#endif

#ifdef CONFIG_DNI_IPTV_PORT_SUPPORT
	iptv_port_delete_proc_entry();
#endif

#ifdef CONFIG_BRIDGE_NETGEAR_ACL
	br_acl_fini();
#endif
}

EXPORT_SYMBOL(br_should_route_hook);

module_init(br_init)
module_exit(br_deinit)
MODULE_LICENSE("GPL");
MODULE_VERSION(BR_VERSION);