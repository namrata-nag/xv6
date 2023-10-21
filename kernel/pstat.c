/* This file is being created and modified by Namrata nag - nxn230019*/
#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "pstat.h"
#include "spinlock.h"

struct
{
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

// This system call saves the statistics of the current ptable in a specific struct - pstat 
// If data is saved successfully then return 0 else return -1
int sys_getpinfo(void)
{
  struct pstat *pst;
  /**
     * argptr takes three argument
     * first arg is the index of the pointer argument passed in the system call 
     * second arg is the address of the pointer passed to the sys call
     * size of the pointer
     * return -1 is not valid input or an error occured
    */
  if (argptr(0, (void *)&pst, sizeof(*pst)) < 0)
  {
    return -1;
  }

  // Is address in valid i.e address is not NULL
  if (!!pst)
  {
    sti();

    //Need to acquire lock else it might get updated by other process
    acquire(&ptable.lock);
    struct proc *p;
    uint i = 0;

    // Loop over process table looking for process to run.
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    {
      // Process has multiple state. here we are considering all the process state except UNUSED
      // Below are the process state and 
      // - EMBRYO : Allocproc scans the table for a process with state UNUSED and mark it as EMBRYO and assign a pid. However other resources like stack heap etc is not allocated yet hence it stays in                        ptable but not ready to run
      // - SLEEPING : Waiting for an I/O or other task
      // - ZOMBIE : Child process who got detached from its parent. Even if parent process is completed or terminated OS still allows child process to run
      // - RUNNING : Process which is currently running
      // - RUNNABLE : Process which is in the ready queue waiting to be scheduled
      if (p->state == EMBRYO || p->state == SLEEPING || p->state == ZOMBIE || p->state == RUNNING || p->state == RUNNABLE)
      {
        pst->inuse[i] = 1;
        pst->pid[i] = p->pid;
        pst->tickets[i] = p->tickets;
        pst->ticks[i] = p->ticks;
      }else{
        pst->inuse[i] =0;
      }
      i++;
    }
    release(&ptable.lock);
    return 0;
  }
  return -1;
}
/* End of code added/modified */