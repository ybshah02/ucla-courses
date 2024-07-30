#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

static struct proc_dir_entry *count_func;

static int show_count(struct seq_file *s, void *v)
{
	int n = 0;
	struct task_struct *p;
	for_each_process(p)
	{
		n += 1;
	}
	seq_printf(s, "%d\n", n);
	return 0;
}

static int __init proc_count_init(void)
{
	pr_info("proc_count: init\n");
	count_func = proc_create_single("count", 0644, NULL, show_count);
	return 0;
}

static void __exit proc_count_exit(void)
{
	pr_info("proc_count: exit\n");
	proc_remove(count_func);
}

module_init(proc_count_init);
module_exit(proc_count_exit);

MODULE_AUTHOR("Yash Shah");
MODULE_DESCRIPTION("Creates a /proc/count file that ");
MODULE_LICENSE("GPL");
