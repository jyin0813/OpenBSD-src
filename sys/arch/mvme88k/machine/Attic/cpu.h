/*
 * Copyright (c) 1996 Nivas Madhur
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _CPU_H_
#define _CPU_H_

/*
 * CTL_MACHDEP definitinos.
 */
#define	CPU_MAXID	1	/* no valid machdep ids */

#define	CTL_MACHDEP_NAMES { \
	{ 0, 0 }, \
}

#ifdef _KERNEL

#include <machine/psl.h>
#include <machine/pcb.h>

/*
 * definitions of cpu-dependent requirements
 * referenced in generic code
 */
#define	COPY_SIGCODE		/* copy sigcode above user stack in exec */

#define	cpu_exec(p)	/* nothing */
#define	cpu_wait(p)	/* nothing */
#define	cpu_swapout(p)	/* nothing */

/*
 * Arguments to hardclock and gatherstats encapsulate the previous
 * machine state in an opaque clockframe. CLKF_INTR is only valid
 * if the process is in kernel mode. Clockframe is really trapframe,
 * so pointer to clockframe can be safely cast into a pointer to
 * trapframe.
 */
struct clockframe {
	struct trapframe tf;
};

extern intstack;

#define	CLKF_USERMODE(framep)	((((struct trapframe *)(framep))->epsr & 80000000) == 0)
#define	CLKF_BASEPRI(framep)	(((struct trapframe *)(framep))->mask == 0)
#define	CLKF_PC(framep)		(((struct trapframe *)(framep))->sxip & ~3)
#define	CLKF_INTR(framep)	(((struct trapframe *)(framep))->r[31] > intstack)

#define SIR_NET		1
#define SIR_CLOCK	2

#define setsoftnet()	(ssir |= SIR_NET)
#define setsoftclock()	(ssir |= SIR_CLOCK)

#define siroff(x)	(ssir &= ~x)

int	ssir;
int	want_ast;

/*
 * Preempt the current process if in interrupt from user mode,
 * or after the current trap/syscall if in system mode.
 */
int	want_resched;		/* resched() was called */
#define	need_resched()		(want_resched = 1, want_ast = 1)

/*
 * Give a profiling tick to the current process when the user profiling
 * buffer pages are invalid.  On the sparc, request an ast to send us 
 * through trap(), marking the proc as needing a profiling tick.
 */
#define	need_proftick(p)	((p)->p_flag |= P_OWEUPC, want_ast = 1)

/*
 * Notify the current process (p) that it has a signal pending,
 * process as soon as possible.
 */
#define	signotify(p)		(want_ast = 1)

struct intrhand {
	int	(*ih_fn)();
	void	*ih_arg;
	int	ih_ipl;
	int	ih_wantframe;
	struct	intrhand *ih_next;
};

int	intr_establish __P((int vec, struct intrhand *));

/*
 * return values for intr_establish()
 */

#define INTR_EST_SUCC 		0
#define INTR_EST_BADVEC		1
#define INTR_EST_BADIPL		2


/*
 * There are 256 possible vectors on a MVME1x7 platform (including
 * onboard and VME vectors. Use intr_establish() to register a
 * handler for the given vector. vector number is used to index
 * into the intr_handlers[] table.
 */
extern struct intrhand *intr_handlers[256];

/*
 * switchframe - should be double word aligned.
 */
struct switchframe {
	u_int	sf_pc;			/* pc */
	void	*sf_proc;		/* proc pointer */
};

#endif /* _KERNEL */
#endif /* _CPU_H_ */
