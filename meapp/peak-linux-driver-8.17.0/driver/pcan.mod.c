#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x8e6402a9, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x2d3385d3, "system_wq" },
	{ 0x85bd1608, "__request_region" },
	{ 0x595451f1, "kmalloc_caches" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0xc4f0da12, "ktime_get_with_offset" },
	{ 0x1ed8b599, "__x86_indirect_thunk_r8" },
	{ 0x928ddd97, "pci_free_irq_vectors" },
	{ 0x451a35b1, "pci_write_config_word" },
	{ 0xe53e512c, "single_open" },
	{ 0x77358855, "iomem_resource" },
	{ 0x39db796f, "dma_set_mask" },
	{ 0xd7ed5de7, "single_release" },
	{ 0x3d3934db, "usb_reset_endpoint" },
	{ 0x18f144e1, "pci_disable_device" },
	{ 0x71c12a08, "i2c_transfer" },
	{ 0x20000329, "simple_strtoul" },
	{ 0xffeedf6a, "delayed_work_timer_fn" },
	{ 0xad280409, "seq_printf" },
	{ 0xb43f9365, "ktime_get" },
	{ 0x332e474f, "usb_kill_urb" },
	{ 0xb75bc01, "remove_proc_entry" },
	{ 0xf595d267, "device_destroy" },
	{ 0xcb720829, "__register_chrdev" },
	{ 0x5edea5, "driver_for_each_device" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x473882c7, "pci_release_regions" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x9fa7184a, "cancel_delayed_work_sync" },
	{ 0x409bcb62, "mutex_unlock" },
	{ 0x4842a737, "dma_free_attrs" },
	{ 0xf3b4e804, "device_create_with_groups" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x8e596aa8, "seq_read" },
	{ 0x360b6b7a, "pv_ops" },
	{ 0x25548a91, "dma_set_coherent_mask" },
	{ 0x15ba50a6, "jiffies" },
	{ 0xe2d5255a, "strcmp" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0xefc3cd2, "dma_get_required_mask" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x90ec5eae, "param_ops_charp" },
	{ 0x59a78ecb, "pci_set_master" },
	{ 0x97934ecf, "del_timer_sync" },
	{ 0x985e7e01, "pci_alloc_irq_vectors_affinity" },
	{ 0xe90b4c14, "_dev_warn" },
	{ 0xfb578fc5, "memset" },
	{ 0xdbdf6c92, "ioport_resource" },
	{ 0x1e1e140e, "ns_to_timespec64" },
	{ 0x9fad0ccf, "pci_iounmap" },
	{ 0xd35cce70, "_raw_spin_unlock_irqrestore" },
	{ 0x69af1880, "current_task" },
	{ 0x37befc70, "jiffies_to_msecs" },
	{ 0x99a6bbcb, "usb_deregister" },
	{ 0x977f511b, "__mutex_init" },
	{ 0xc5850110, "printk" },
	{ 0xfef216eb, "_raw_spin_trylock" },
	{ 0x26cc59c8, "sysfs_remove_file_from_group" },
	{ 0x449ad0a7, "memcmp" },
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0xc5645b17, "class_unregister" },
	{ 0xde80cd09, "ioremap" },
	{ 0x1edb69d6, "ktime_get_raw_ts64" },
	{ 0xae167e29, "usb_set_interface" },
	{ 0x9166fada, "strncpy" },
	{ 0x95d22770, "usb_control_msg" },
	{ 0x1bd65a5f, "pci_read_config_word" },
	{ 0x593c1bac, "__x86_indirect_thunk_rbx" },
	{ 0x9c5429ca, "dma_alloc_attrs" },
	{ 0x2ab7989d, "mutex_lock" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xdd2f8305, "__class_register" },
	{ 0x151ae673, "_dev_err" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0xeb6fb104, "pci_find_capability" },
	{ 0x800473f, "__cond_resched" },
	{ 0x167c5967, "print_hex_dump" },
	{ 0x498a8997, "i2c_del_adapter" },
	{ 0xaf777158, "_dev_info" },
	{ 0xff3a7e30, "usb_submit_urb" },
	{ 0xb601be4c, "__x86_indirect_thunk_rdx" },
	{ 0xb2fcb56d, "queue_delayed_work_on" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x5912036c, "usb_reset_device" },
	{ 0xe9602485, "usb_bulk_msg" },
	{ 0x1000e51, "schedule" },
	{ 0x8ddd8aad, "schedule_timeout" },
	{ 0x92b18ef6, "usb_clear_halt" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x1035c7c2, "__release_region" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xec646bf1, "pci_unregister_driver" },
	{ 0xcc5005fe, "msleep_interruptible" },
	{ 0xe78dfe6d, "kmem_cache_alloc_trace" },
	{ 0x34db050b, "_raw_spin_lock_irqsave" },
	{ 0xfde9e2ba, "param_ops_byte" },
	{ 0xedc4b55f, "pci_irq_vector" },
	{ 0x3eeb2322, "__wake_up" },
	{ 0xf6ebc03b, "net_ratelimit" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0xc3055d20, "usleep_range_state" },
	{ 0x91c1631c, "seq_lseek" },
	{ 0x37a0cba, "kfree" },
	{ 0x69acdf38, "memcpy" },
	{ 0x29015475, "pci_request_regions" },
	{ 0xed1f2dff, "param_array_ops" },
	{ 0xedc03953, "iounmap" },
	{ 0x988d943b, "__pci_register_driver" },
	{ 0x4fe357bb, "usb_register_driver" },
	{ 0x92540fbf, "finish_wait" },
	{ 0xb3ad018a, "sysfs_add_file_to_group" },
	{ 0x4c876149, "i2c_bit_add_bus" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xb0e602eb, "memmove" },
	{ 0x251321d, "pci_iomap" },
	{ 0xc7bd5755, "param_ops_ushort" },
	{ 0xbe8b05f8, "proc_create" },
	{ 0x79c3da40, "usb_get_current_frame_number" },
	{ 0x5e515be6, "ktime_get_ts64" },
	{ 0x7f02188f, "__msecs_to_jiffies" },
	{ 0xcf2d6503, "pci_enable_device" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x4083b059, "param_ops_ulong" },
	{ 0x3d58fb01, "param_ops_uint" },
	{ 0x8971e83c, "usb_free_urb" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x8a68ac56, "usb_alloc_urb" },
	{ 0xc1514a3b, "free_irq" },
};

MODULE_INFO(depends, "i2c-algo-bit");

MODULE_ALIAS("pci:v0000001Cd00000001sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000003sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000004sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000005sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000006sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000007sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000008sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000009sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000002sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd0000000Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000010sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000013sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000014sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000016sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000017sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000018sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000019sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd0000001Asv*sd*bc*sc*i*");
MODULE_ALIAS("usb:v0C72p000Cd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0C72p000Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0C72p0012d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0C72p0011d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0C72p0013d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0C72p0014d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "9B91AA025C2BA6422309BBB");
