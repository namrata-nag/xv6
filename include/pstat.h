/* This file is being created and modified by Namrata nag - nxn230019*/
#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

// Struct to save the statistics of Running process. 
// It can be accessed via getpinfo system call
struct pstat {
 int inuse[NPROC]; // whether this slot of the process table is in use (1 or 0)
 int tickets[NPROC]; // the number of tickets this process has
 int pid[NPROC]; // the PID of each process
 int ticks[NPROC]; // the number of ticks each process has accumulated
};
#endif // _PSTAT_H_
/* End of code added/modified */