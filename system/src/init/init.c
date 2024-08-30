#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/kdev_t.h>

void set_stdio_to_dev_null() {
    int fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        return;
    }
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    if (fd > STDERR_FILENO)
        close(fd);
}

int init_kmsg() {
    int fd = 0;
    char *msg;
    if (access("/dev/kmsg", W_OK) == 0) {
        fd = open("/dev/kmsg", O_WRONLY | O_CLOEXEC);
        msg = "creating from /dev/kmsg";
    } else {
        mknod("/kmsg", S_IFCHR | 0666, MKDEV(1, 11));
        fd = open("/kmsg", O_WRONLY | O_CLOEXEC);
        unlink("/kmsg");
        msg = "creating from /kmsg";
    }
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    puts(msg);
    return fd;
}

int main() {
    set_stdio_to_dev_null();
    int fd = init_kmsg();
    printf("kmsg fd %d\n", fd);
    char *argv[] = {"/bin/busybox", "sh", "/bin/init.sh", NULL};
    printf("execvp /bin/busybox sh /bin/init.sh\n");
    return execvp(argv[0], argv);
}