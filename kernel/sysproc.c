#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

int numsyscalls = 0;
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
sys_howmanysys(void)
{
  return numsyscalls;
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

int
sys_settickets(void)
{
  int number;
  argint(0, &number);
  if (number < 1) return -1;

  proc->tickets = number;

  return 0;
}

int
sys_getpinfo(void){
  struct pstat *table;
  if(argint(0, (int*)(&table)) < 0){
    return -1;
  }

  acquire(&ptable.lock);

  // Run through the ptable
  for(struct proc* p=ptable.proc;p != &(ptable.proc[NPROC]); p++)
  {
    if (p->state != UNUSED){
      const int index = p - ptable.proc; // Determine index of the process

      table->pid[index] = p->pid;
			table->ticks[index] = p->ticks;
			table->tickets[index] = p->tickets;
			table->inuse[index] = p->inuse;
    }
  }

  release(&ptable.lock);
  return 0;
}
