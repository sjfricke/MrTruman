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
	{ 0x9ebe240b, "struct_module" },
	{ 0x12493de4, "usb_register" },
	{ 0x4b96afb4, "usb_deregister" },
	{ 0x88ce4ace, "wait_for_completion" },
	{ 0x16c63665, "usb_unlink_urb" },
	{ 0x45571cf4, "usb_deregister_dev" },
	{ 0xd4748dfa, "usb_register_dev" },
	{ 0xcbf1a523, "usb_alloc_urb" },
	{ 0x819e11d6, "kmem_cache_alloc" },
	{ 0xa47da14e, "malloc_sizes" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0x74cc238d, "current_kernel_time" },
	{ 0x95d0373e, "fasync_helper" },
	{ 0x2a7b11d4, "finish_wait" },
	{ 0x7d1ed1f2, "prepare_to_wait" },
	{ 0x4292364c, "schedule" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x625acc81, "__down_failed_interruptible" },
	{ 0xbe189f32, "usb_find_interface" },
	{ 0x96b27088, "__down_failed" },
	{ 0x60a4461c, "__up_wakeup" },
	{ 0x37a0cba, "kfree" },
	{ 0x5d8c1720, "usb_free_urb" },
	{ 0x617d9b67, "kill_fasync" },
	{ 0xc281c899, "__wake_up" },
	{ 0xa9392f2b, "complete" },
	{ 0x4ce1a5e3, "usb_submit_urb" },
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
