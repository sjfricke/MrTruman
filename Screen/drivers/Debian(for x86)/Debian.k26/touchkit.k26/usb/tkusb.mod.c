#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x4cdc490c, "cleanup_module" },
	{ 0xcd58c830, "init_module" },
	{ 0x7200d09a, "struct_module" },
	{ 0x75a1b860, "usb_register" },
	{ 0x134021d5, "usb_deregister" },
	{ 0xdc016488, "wait_for_completion" },
	{ 0x3084e371, "usb_unlink_urb" },
	{ 0xd0cfdfe0, "usb_deregister_dev" },
	{ 0x218effbc, "usb_register_dev" },
	{ 0xb275e381, "usb_alloc_urb" },
	{ 0x6a6f0027, "kmem_cache_alloc" },
	{ 0x3de88ff0, "malloc_sizes" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0xd6c963c, "copy_from_user" },
	{ 0x74cc238d, "current_kernel_time" },
	{ 0x695deb91, "fasync_helper" },
	{ 0xffa9a84d, "finish_wait" },
	{ 0x4292364c, "schedule" },
	{ 0x8cff2f5b, "prepare_to_wait" },
	{ 0x21d433a7, "autoremove_wake_function" },
	{ 0x28c3bbf5, "__down_failed_interruptible" },
	{ 0x7140b11b, "usb_find_interface" },
	{ 0xf2520b76, "__down_failed" },
	{ 0xd22b546, "__up_wakeup" },
	{ 0x37a0cba, "kfree" },
	{ 0x6b2e1601, "usb_free_urb" },
	{ 0xde9a20ca, "kill_fasync" },
	{ 0x942613ad, "complete" },
	{ 0xbce89f77, "__wake_up" },
	{ 0x2bebf427, "usb_submit_urb" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=usbcore";

MODULE_ALIAS("usb:v1234p0001dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1234p0002dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0EEFp0001dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0EEFp0002dl*dh*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v3823p0001dl*dh*dc*dsc*dp*ic*isc*ip*");
