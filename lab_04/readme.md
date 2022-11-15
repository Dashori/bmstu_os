Демон момент

sudo ./a.out

ps -ajx

cat /var/run/daemon.pid

sudo cat /var/log/syslog | grep dasha_deamon

sudo kill -1 PID SIGHUB

sudo kill -15 PID SIGTERM

sudo kill -9 PID

пояснить за каждую колоночку
PPID     PID    PGID     SID TTY        TPGID STAT   UID   TIME COMMAND

почему PPID такой? почему его усынловляет 2059? 

потому что демона запускаем сами из терминала

PID == PGID == SID

TTY == ? нет управляющего терминала

TPGID == -1 отсутсувте терминальная группа (т.к. нет управляющего терминала)

“S” - ожидание завершения события (interruptable sleep). 

прерываемый сон (S) -- в состоянии ожидания завершения события (прерывать можем с помощью сигналов)
непрерываемый сон (D) -- в состоянии ожидания завершения процесса ввода-вывода

“s” - лидер сессии
“l” - многопоточость


umask(0) мы делаем, чтоб процесс мог создавать файлы с любыми правами доступа


sigaction на SIGHUP сделать ДО setsid




