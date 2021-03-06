/*	$OpenBSD: locore.h,v 1.8 2006/05/08 14:03:34 miod Exp $	*/

#ifndef _MACHINE_LOCORE_H_
#define _MACHINE_LOCORE_H_

/*
 * C prototypes for various routines defined in locore_* and friends
 */

/* subr.S */

int badaddr(vaddr_t addr, int size);
#define badwordaddr(x) badaddr(x, 4)
void doboot(void);

/* machdep.c */

unsigned getipl(void);
void set_cpu_number(cpuid_t);

/* eh.S */

void sigsys(void);
void stepbpt(void);
void userbpt(void);
void syscall_handler(void);
void cache_flush_handler(void);

#endif /* _MACHINE_LOCORE_H_ */
