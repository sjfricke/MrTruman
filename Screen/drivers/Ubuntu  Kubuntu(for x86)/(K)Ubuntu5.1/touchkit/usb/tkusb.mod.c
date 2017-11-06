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
	{ 0x2720b98e, "struct_module" },
	{ 0xed728fed, "usb_register" },
	{ 0x1f143a0c, "usb_deregister" },
	{ 0xdc016488, "wait_for_completion" },
	{ 0xfd94c077, "usb_unlink_urb" },
	{ 0x3e6d3591, "usb_deregister_dev" },
	{ 0x7afa6a88, "usb_register_dev" },
	{ 0xdc20a362, "usb_alloc_urb" },
	{ 0x2642591c, "kmem_cache_alloc" },
	{ 0x3de88ff0, "malloc_sizes" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0x74cc238d, "current_kernel_time" },
	{ 0x51caddd0, "fasync_helper" },
	{ 0x98e4e879, "finish_wait" },
	{ 0x4292364c, "schedule" },
	{ 0x98d23e12, "prepare_to_wait" },
	{ 0x2e60bace, "memcpy" },
	{ 0x4e45624a, "autoremove_wake_function" },
	{ 0x625acc81, "__down_failed_interruptible" },
	{ 0x10d51eaa, "usb_find_interface" },
	{ 0x96b27088, "__down_failed" },
	{ 0x60a4461c, "__up_wakeup" },
	{ 0x37a0cba, "kfree" },
	{ 0xf27fc505, "usb_free_urb" },
	{ 0x277e190a, "kill_fasync" },
	{ 0x942613ad, "complete" },
	{ 0xbce89f77, "__wake_up" },
	{ 0xe34db861, "usb_submit_urb" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=usbcore";

MODULE_ALIAS("usb:v1234p0001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1234p0002d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0EEFp0001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0EEFp0002d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v3823p0001d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "B0872D8913B07BF5665B5BA");
