#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

struct stat statbuf;

#define PRINT_STAT(action) \
    do { \
        stat("q.txt", &statbuf); \
        fprintf(stdout, action ": inode number = %ld, \
                     size = %ld bytes, blksize = %ld\n", \
                    statbuf.st_ino, statbuf.st_size, \
                    statbuf.st_blksize); \
    } while (0)

int main()
{
    FILE *fs1 = fopen("q.txt", "w");
    PRINT_STAT("fopen fs1");
    FILE *fs2 = fopen("q.txt", "w");
    PRINT_STAT("fopen fs2");
    for (char c = 'a'; c <= 'z'; c++)
    {
        c % 2 ? fprintf(fs1, "%c", c) : fprintf(fs2, "%c", c);
        PRINT_STAT("fprintf");
    }
    fclose(fs1);
    PRINT_STAT("fclose fs1");
    fclose(fs2);
    PRINT_STAT("fclose fs2");
    return 0;
}