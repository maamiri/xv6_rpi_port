/*****************************************************************
*       main.c
*       by Zhiyi Huang, hzy@cs.otago.ac.nz
*       University of Otago
*
********************************************************************/


#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "arm.h"
#include "mailbox.h"

extern char end[]; // first address after kernel loaded from ELF file
extern pde_t *kpgdir;
extern FBI fbinfo;
extern volatile uint *mailbuffer;

void OkLoop()
{
   setgpiofunc(16, 1); // gpio 16, set as an output
   while(1){
        setgpioval(16, 0);
        delay(1000000);
        setgpioval(16, 1);
        delay(1000000);
   }
}

void NotOkLoop()
{
   setgpiofunc(16, 1); // gpio 16, set as an output
   while(1){
        setgpioval(16, 0);
        delay(100000);
        setgpioval(16, 1);
        delay(100000);
   }
}

void machinit(void)
{
    memset(cpus, 0, sizeof(struct cpu)*NCPU);
}


void enableirqminiuart(void);

int cmain()
{

  mmuinit0();
  //XX OK
  mmuinit1();
  machinit();

  #if defined (RPI1) || defined (RPI2)
  uartinit();
  #elif defined (FVP)
  uartinit_fvp();
  #endif
  //XXX OK
  dsb_barrier();

  consoleinit();

  cprintf("\nHello World from xv6\n");
  acknowledge();
  kinit1(end, P2V((8*1024*1024)+PHYSTART));  // reserve 8 pages for PGDIR
  kpgdir=p2v(K_PDX_BASE);

  #if defined (RPI1) || defined (RPI2)
  mailboxinit();
  create_request(mailbuffer, MPI_TAG_GET_ARM_MEMORY, 8, 0, 0);
  writemailbox((uint *)mailbuffer, 8);
  readmailbox(8);
  if(mailbuffer[1] != 0x80000000) cprintf("new error readmailbox\n");
  //else

  cprintf("ARM memory is %x %x\n", mailbuffer[MB_HEADER_LENGTH + TAG_HEADER_LENGTH], mailbuffer[MB_HEADER_LENGTH + TAG_HEADER_LENGTH+1]);
  #endif

  pinit();
  tvinit();
  cprintf("it is ok after tvinit\n");
  binit();
cprintf("it is ok after binit\n");
  fileinit();
cprintf("it is ok after fileinit\n");
  iinit();
cprintf("it is ok after iinit\n");
  ideinit();
cprintf("it is ok after ideinit\n");
  kinit2(P2V((8*1024*1024)+PHYSTART), P2V(PHYSTOP));
cprintf("it is ok after kinit2\n");
  userinit();
cprintf("it is ok after userinit\n");
  timer3init();
cprintf("it is ok after timer3init\n");
  scheduler();
  NotOkLoop();
  return 0;
}
