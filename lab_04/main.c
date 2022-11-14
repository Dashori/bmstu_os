
#include <string.h>
#include <stddef.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h> 
#include <stdio.h>

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define SLEEP_TIME 15

void daemonize(const char *name);
int lockfile(int fd);
int already_running(void);

int main(int argc, char *argv[])
{
    int error;
    pthread_t thread_id;
    char *cmd;
    struct sigaction sa;

    printf(" BBB ");

    daemonize("dasha_deamon");

    printf(" AAA ");

    if (already_running())
    {
        syslog(LOG_ERR, "can\'t launch daemon - already running\n");
        exit(1);
    }

    return 0;
}

void daemonize(const char *name)
{
    // чтобы процесс мог создавать файлы с любыми правами доступа

    umask(0); 

    //получаем максимально возможный номер дескриптора файла, чтоб потом закрыть все

    struct rlimit r_limit;

    if (getrlimit(RLIMIT_NOFILE, &r_limit) < 0) 
    { 
        syslog(LOG_ERR, "can\'t get file limit\n");
    }

    pid_t deamon_pid;

    if ((deamon_pid = fork()) < 0)
    {
        syslog(LOG_ERR, "can\'t fork\n");
    } 
    else if (deamon_pid != 0)
    {
        exit(0);
    }

    // потомок теперь не является лидером группы и это является условием вызова функции setsid()
    // создаем сессию. При этом процесс становися лидером новой сессии, лидером новый группы процессов
    // и лишается управляющего терминала

    if (setsid() == -1) 
    {
        syslog(LOG_ERR, "can\'t setsid\n");
        exit(1);
    }

    struct sigaction sig_action;

    // обеспечиваем невозможность обретения управляющего терминала

    sig_action.sa_handler = SIG_IGN; // макрос ignore this signal
    sigemptyset(&sig_action.sa_mask); // установка empty mask
    sig_action.sa_flags = 0;

    if (sigaction(SIGHUP, &sig_action, NULL) < 0) // SIGHUB сообщает процессу, что процесс утратил управляющий терминал
    {
        syslog(LOG_ERR, "can\'t ignore SIGHUP\n");
    }  

    // меняем текущий каталог на корневой
    

    if (chdir("/") < 0) 
    {
        syslog(LOG_ERR, "can\'t set directory to /\n");
    }

    // закрываем все открытые файловые дескрипторы

    if (r_limit.rlim_max == RLIM_INFINITY)
    {
        r_limit.rlim_max = 1024; // максимально возможное количество открытых файлов
    }

    for (int i = 0; i < r_limit.rlim_max; i++)
    {
        close(i);
    }

    // присоединяем файловые дескрипторы к 0, 1, 2 к /dev/null

    int fd0, fd1, fd2;

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

   
    // инициализируем файл журнала

    openlog(name, LOG_CONS, LOG_DAEMON);

    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "unexpected file descriptors %d, %d, %d\n", fd0, fd1, fd2);
        exit(1);
    }

}

int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    return fcntl(fd, F_SETLK, &fl);
}

int already_running(void)
{
    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);

    if (fd < 0)
    {
        syslog(LOG_ERR, "can\'t open %s : %s\n", LOCKFILE, strerror(errno));
        exit(1);
    }

    if (lockfile(fd) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);

            return 1;
        }

        syslog(LOG_ERR, "can't lock %s: %s\n", LOCKFILE, strerror(errno));
        exit(1);
    }

    // усечение размера файла необходимо по той причине, что 
    // идентификатор процесса предыдцщей копии демона, 
    // представленный в виде строки, мог иметь большую длину.
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);

    return 0;
}