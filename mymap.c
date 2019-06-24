#include <linux/miscdevice.h>
 #include <linux/delay.h>
 #include <linux/kernel.h>
 #include <linux/module.h>
 #include <linux/init.h>
 #include <linux/mm.h>
 #include <linux/fs.h>
 #include <linux/types.h>
 #include <linux/delay.h>
 #include <linux/moduleparam.h>
 #include <linux/slab.h>
 #include <linux/errno.h>
 #include <linux/ioctl.h>
 #include <linux/cdev.h>
 #include <linux/string.h>
 #include <linux/list.h>
 #include <linux/pci.h>
 #include <linux/gpio.h>
 #include <linux/kthread.h>

#include <linux/netdevice.h>
#include <net/cfg80211.h>
#include <net/mac80211.h>
#include "include/ieee80211_i.h"

 
 #define DEVICE_NAME "mymap"
 
 
 
 char *buffer=NULL;
 struct net_device *dev=NULL;
 struct task_struct * MyThread = NULL;

struct ieee80211_local *wls_local = NULL;
struct ieee80211_vif *wls_vif = NULL;
struct ieee80211_hw *wls_hw = NULL;
struct wiphy *wls_wi=NULL;
int wls_hacker_init(void);
int wls_conf_tx(char*data);

int wls_hacker_init()
{
    

    for_each_netdev(&init_net, dev)
    {
        if( dev->ieee80211_ptr &&strcmp(dev->name,"wlp2s0")==0 ) // <net/cfg80211.h>
        {
            wls_vif = wdev_to_ieee80211_vif(dev->ieee80211_ptr);
            wls_wi=dev->ieee80211_ptr->wiphy;
	    	wls_local= wiphy_priv(wls_wi);
		    
            wls_hw=&wls_local->hw;
        
            printk("find one driver: %s\n", dev->name);
            break;
            
            
        }
    }

    if (!wls_local)
        return -1;
    return 0;
}

int wls_conf_tx(char* data)
{
    struct ieee80211_tx_queue_params wls_params = 
    {
        .txop = 0,
	    .cw_min = data[2],
	    .cw_max = data[3],
	    .aifs = data[1],
        .acm = false,
        .uapsd = false
    };
    int ret;
   
    
    if(!wls_local)
    {
        ret=-1;
        printk("wls_local null pointer\n");
    }
    else
    {
        
        if(!wls_local->ops)
        {
            printk("ieeeops null\n");
        }
        else
        {   
            printk("Update\n");
            ret = wls_local->ops->conf_tx(wls_hw, wls_vif, 0, &wls_params);
            ret = wls_local->ops->conf_tx(wls_hw, wls_vif, 1, &wls_params);
            ret = wls_local->ops->conf_tx(wls_hw, wls_vif, 2, &wls_params);
            ret = wls_local->ops->conf_tx(wls_hw, wls_vif, 3, &wls_params);
        }
    }
    
    
    
    return ret;
}






static int tconf_tx(void *data)
{

while(!kthread_should_stop())
{
    schedule();
    if(buffer[0]==1)
    {
        buffer[0]=0;
        
        wls_conf_tx(buffer);
    }

}

return 0;
}

 static int my_open(struct inode *inode, struct file *file)
 {
     return 0;
 }
 
 
 static int my_map(struct file *filp, struct vm_area_struct *vma)
 {    
     unsigned long page;
     
     unsigned long start = (unsigned long)vma->vm_start;
     //unsigned long end =  (unsigned long)vma->vm_end;
     unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);
 
     
     page = virt_to_phys(buffer);    
     
     if(remap_pfn_range(vma,start,page>>PAGE_SHIFT,size,PAGE_SHARED))
         return -1;
 
     
     
     
     return 0;
 }
 
 
 static struct file_operations dev_fops = {
     .owner    = THIS_MODULE,
     .open    = my_open,
     .mmap   = my_map,
 };
 
 static struct miscdevice misc = {
     .minor = MISC_DYNAMIC_MINOR,
     .name = DEVICE_NAME,
     .fops = &dev_fops,
 };
 
 
 static int __init dev_init(void)
 {
     int ret;    
 
     ret=wls_hacker_init();
     ret = misc_register(&misc);
     
     buffer = (unsigned char *)kmalloc(PAGE_SIZE,GFP_KERNEL);
     buffer[0]=0;
     

     SetPageReserved(virt_to_page(buffer));
     MyThread = kthread_run(tconf_tx,NULL,"mythread");
     return ret;
 }
 
 
 static void __exit dev_exit(void)
 {
     kthread_stop(MyThread);
     misc_deregister(&misc);
     
     ClearPageReserved(virt_to_page(buffer));
     
     kfree(buffer);
     
 }
 
 
 module_init(dev_init);
 module_exit(dev_exit);
 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("LKN@SCUT");





 
