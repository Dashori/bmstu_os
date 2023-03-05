const GET_NUMBER = 0;
const ENTER_CR = 1;

struct BAKERY
{
    int op;
    int num;
    int pid;
    int result;
};

program BAKERY_PROG
{
    version BAKERY_VER
    {
        struct BAKERY BAKERY_PROC(struct BAKERY) = 1;
    } = 1; /* Version number = 1 */
} = 0x20000001;
