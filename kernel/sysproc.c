#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

/* This code is being added by Namrata nag - nxn230019 
 * Set custom ticket value to the process
 * takes integer value as input
 * return 0 if ticket set successfully
 * return -1 if invalid input or any error occured
 * */
int sys_settickets(void){
  int n;

  // Fetch  integer input
  if (argint(0, &n) < 0)
  {
    return -1;
  }

  // Input should not be 0 or less than 0 
  if(n <= 0){
     return -1;
  }

  // If proc->tickets available assign the custom ticket
  if (proc->tickets)
  {
    proc->tickets = n;
    return 0;
  }
  return -1;
}
/* End of code added/modified */


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
