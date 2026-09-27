/*
 * Copyright (C) 2026 Motorola Mobility LLC
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt) "moto_asoc: " fmt

#include <linux/module.h>
#include <linux/version.h>
#include <linux/errno.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 189) && LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 0))
#include <trace/hooks/asoc.h>

#define REGISTER_HOOK(name) do {\
	rc = register_trace_android_vh_##name(name##_hook, NULL);\
	if (rc) {\
		pr_err("register hook %s failed", #name);\
	}\
} while (0)

#define UNREGISTER_HOOK(name) do {\
	unregister_trace_android_vh_##name(name##_hook, NULL);\
} while (0)

static void put_volsw_hook(void *data, int platform_max, int min, unsigned int val, int *err) {
	*err = 1;

	if (platform_max && ((int)val + min) > platform_max) {
		pr_debug("put_volsw_hook: pm=%d min=%d val=%u", platform_max, min, val);
		pr_debug("put_volsw_hook: returning -EINVAL");
		*err = -EINVAL;
	}

	pr_debug("put_volsw_hook: returning err=%d", *err);
}

static int register_all_hooks(void) {
	int rc = 0;
	REGISTER_HOOK(put_volsw);
	return rc;
}

static void unregister_all_hooks(void) {
	UNREGISTER_HOOK(put_volsw);
}
#endif

static int __init moto_asoc_init(void) {
	int ret = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 189) && LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 0))
	ret = register_all_hooks();
	if (ret) {
		pr_err("moto_asoc_init failed to register hook!");
		return ret;
	}

	pr_debug("moto_asoc_init succeed!");
#else
	pr_err("moto_asoc_init: hook unavailable on this kernel: -ENOSYS");
	ret = -ENOSYS;
#endif
	return ret;
}

static void __exit moto_asoc_exit(void) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 189) && LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 0))
	unregister_all_hooks();
#endif

	pr_debug("moto_asoc_exit succeed!");
}

module_init(moto_asoc_init);
module_exit(moto_asoc_exit);
MODULE_DESCRIPTION("Motorola asoc hook driver");
MODULE_LICENSE("GPL v2");
