#include "asm/unistd_64.h"
#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/printk.h>

#define die(...)                                                               \
	{                                                                          \
		pr_err(__VA_ARGS__);                                                   \
		return -1;                                                             \
	}

#ifndef CONFIG_ARCH_HAS_SYSCALL_WRAPPER
#error "need syscall wrapper"
#endif

static void** sys_call_table;

static asmlinkage long (*old_fsmount)(const struct pt_regs* regs);

static asmlinkage long our_fsmount(const struct pt_regs* regs) {
	pr_info("fs_fd:   %lu\n", regs->di);
	pr_info("flags:   %lu\n", regs->si);
	pr_info("msflags: %lu\n", regs->dx);

	return -1;
}

static int __init hello_init(void) {
	pr_info("Hello, World!\n");

	return 0;

	struct kprobe kp = {
		.symbol_name = "kallsyms_lookup_name",
	};
	if(register_kprobe(&kp) < 0) die("register kprobe");

	void** (*kallsyms_lookup_name)(const char* name) =
		(void** (*) (const char*) ) kp.addr;
	unregister_kprobe(&kp);

	sys_call_table               = kallsyms_lookup_name("sys_call_table");
	old_fsmount                  = sys_call_table[__NR_fsmount];
	sys_call_table[__NR_fsmount] = our_fsmount;

	return 0;
}

static void __exit hello_exit(void) {
	pr_info("Goodbye, World\n");
	sys_call_table[__NR_fsmount] = old_fsmount;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
