/* This file is being created and modified by Namrata nag - nxn230019*/
#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

void spin()
{
    int i = 0;
    int j = 0;
    int k = 0;
    for (i = 0; i < 50; ++i)
    {
        for (j = 0; j < 400000; ++j)
        {
            k = j % 10;
            k = k + 1;
        }
    }
}

void print(struct pstat *st, int child_pid[])
{
    int i;
    for (i = 0; i < NPROC; i++)
    {
        if (st->pid[i] == child_pid[0] || st->pid[i] == child_pid[1] || st->pid[i] == child_pid[2])
        {
            printf(1, "pid: %d tickets: %d ticks: %d\n", st->pid[i], st->tickets[i], st->ticks[i]);
        }
    }
}

int main()
{
    int numtickets[] = {10, 20, 30};
    int pid_chds[3] = {-1, -1, -1};
    int i = 0;

    for (i = 0; i < 3; i++)
    {
        pid_chds[i] = fork();
        if (pid_chds[i] == 0)
        {
            settickets(numtickets[i]);
            for (;;)
            {
                spin();
            }
            break;
        }
    }

    sleep(1);
    printf(1, "Spinning...\n");
    spin();

    struct pstat st_after;

    if(getpinfo(&st_after)!=0){          printf(1,"check failed: getpinfo\n");
            goto Cleanup;
     }


    print(&st_after, pid_chds);
    goto Cleanup;

Cleanup:
    for (i = 0; pid_chds[i] > 0; i++)
    {
        kill(pid_chds[i]);
    }
    while (wait() > -1);

    exit();
}

/* End of code added/modified */
