/*	$OpenBSD: genassym.c,v 1.7 1997/03/26 08:32:40 downsj Exp $	*/
/*	$NetBSD: genassym.c,v 1.22 1997/02/02 07:53:16 thorpej Exp $	*/

/*
 * Copyright (c) 1982, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *
 *	@(#)genassym.c	8.3 (Berkeley) 1/4/94
 */

/* XXXX */
#define _VA_LIST_ _BSD_VA_LIST_
#define _PTRDIFF_T_ _BSD_PTRDIFF_T_

#undef _KERNEL		/* XXX for errno declaration */
#include <errno.h>
#define _KERNEL

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/map.h>
#include <sys/proc.h>
#include <sys/mbuf.h>
#include <sys/msgbuf.h>
#include <sys/syscall.h>
#include <sys/user.h>

#include <vm/vm.h>

#include <machine/cpu.h>
#include <machine/trap.h>
#include <machine/psl.h>
#include <machine/reg.h>
#include <machine/pte.h>

#include <hp300/hp300/clockreg.h>
#ifdef USELEDS
#include <hp300/hp300/led.h>
#endif

#include <stdio.h>
#include <stddef.h>
#include <string.h>

void
def(what, val)
	char *what;
	int val;
{

	if (printf("#define\t%s\t%d\n", what, val) < 0) {
		(void)fprintf(stderr, "genassym: printf: %s\n",
		    strerror(errno));
		exit(1);
	}
}

void
flush()
{

	if (fflush(stdout) || fsync(fileno(stdout)) < 0) {
		(void)fprintf(stderr, "genassym: flush stdout: %s\n",
		    strerror(errno));
		exit(1);
	}
}

#define	off(what, s, m)	def(what, (int)offsetof(s, m))

main()
{
	/* CPU options */
#ifdef M68020
	def("M68020", 1);
#endif
#ifdef M68030
	def("M68030", 1);
#endif
#ifdef M68040
	def("M68040", 1);
#endif

	/* MMU options */
#ifdef M68K_MMU_MOTOROLA
	def("M68K_MMU_MOTOROLA", 1);
#endif
#ifdef M68K_MMU_HP
	def("M68K_MMU_HP", 1);
#endif

	/* MMU types */
	def("MMU_68040", MMU_68040);
	def("MMU_68030", MMU_68030);
	def("MMU_HP", MMU_HP);
	def("MMU_68851", MMU_68851);

	/* CPU types */
	def("CPU_68020", CPU_68020);
	def("CPU_68030", CPU_68030);
	def("CPU_68040", CPU_68040);

	/* FPU types */
	def("FPU_NONE", FPU_NONE);
	def("FPU_68881", FPU_68881);
	def("FPU_68882", FPU_68882);
	def("FPU_68040", FPU_68040);
	def("FPU_68060", FPU_68060);
	def("FPU_UNKNOWN", FPU_UNKNOWN);

	/* values for machineid */
	def("HP_320", HP_320);
	def("HP_330", HP_330);
	def("HP_350", HP_350);
	def("HP_360", HP_360);
	def("HP_370", HP_370);
	def("HP_340", HP_340);
	def("HP_375", HP_375);
	def("HP_380", HP_380);
	def("HP_433", HP_433);

	/* values for ectype */
	def("EC_PHYS", EC_PHYS);
	def("EC_NONE", EC_NONE);
	def("EC_VIRT", EC_VIRT);

	/* general constants */
	def("UPAGES", UPAGES);
	def("USPACE", USPACE);
	def("NBPG", NBPG);
	def("PGSHIFT", PGSHIFT);
	def("USRSTACK", USRSTACK);
	def("MAXADDR", MAXADDR);

	/* proc fields and values */
	off("P_FORW", struct proc, p_forw);
	off("P_BACK", struct proc, p_back);
	off("P_VMSPACE", struct proc, p_vmspace);
	off("P_ADDR", struct proc, p_addr);
	off("P_PRIORITY", struct proc, p_priority);
	off("P_STAT", struct proc, p_stat);
	off("P_WCHAN", struct proc, p_wchan);
	off("P_FLAG", struct proc, p_flag);
	off("P_MD_FLAGS", struct proc, p_md.md_flags);
	off("P_MD_REGS", struct proc, p_md.md_regs);
	def("SSLEEP", SSLEEP);
	def("SRUN", SRUN);

	/* VM structure fields */
	off("VM_PMAP", struct vmspace, vm_pmap);
	off("PM_STCHG", struct pmap, pm_stchanged);

	/* interrupt/fault metering */
	off("V_INTR", struct vmmeter, v_intr);

	/* trap types (should just include trap.h?) */
	def("T_BUSERR", T_BUSERR);
	def("T_ADDRERR", T_ADDRERR);
	def("T_ILLINST", T_ILLINST);
	def("T_ZERODIV", T_ZERODIV);
	def("T_CHKINST", T_CHKINST);
	def("T_TRAPVINST", T_TRAPVINST);
	def("T_PRIVINST", T_PRIVINST);
	def("T_TRACE", T_TRACE);
	def("T_MMUFLT", T_MMUFLT);
	def("T_SSIR", T_SSIR);
	def("T_FMTERR", T_FMTERR);
	def("T_COPERR", T_COPERR);
	def("T_FPERR", T_FPERR);
	def("T_ASTFLT", T_ASTFLT);
	def("T_TRAP15", T_TRAP15);
	def("T_FPEMULI", T_FPEMULI);
	def("T_FPEMULD", T_FPEMULD);

	/* PSL values (should just include psl.h?) */
	def("PSL_S", PSL_S);
	def("PSL_IPL7", PSL_IPL7);
	def("PSL_LOWIPL", PSL_LOWIPL);
	def("PSL_HIGHIPL", PSL_HIGHIPL);
	def("PSL_USER", PSL_USER);
	def("SPL1", PSL_S | PSL_IPL1);
	def("SPL2", PSL_S | PSL_IPL2);
	def("SPL3", PSL_S | PSL_IPL3);
	def("SPL4", PSL_S | PSL_IPL4);
	def("SPL5", PSL_S | PSL_IPL5);
	def("SPL6", PSL_S | PSL_IPL6);

	/* magic */
	def("FC_USERD", FC_USERD);
	def("FC_PURGE", FC_PURGE);
	def("INTIOBASE", INTIOBASE);
	def("MMUBASE", MMUBASE);
	def("MMUSTAT", MMUSTAT);
	def("MMUCMD", MMUCMD);
	def("MMUSSTP", MMUSSTP);
	def("MMUUSTP", MMUUSTP);
	def("MMUTBINVAL", MMUTBINVAL);
	def("MMU_BERR", MMU_BERR);
	def("MMU_ENAB", MMU_ENAB);
	def("MMU_FAULT", MMU_FAULT);
	def("MMU_CEN", MMU_CEN);
	def("MMU_IEN", MMU_IEN);
	def("MMU_FPE", MMU_FPE);
	def("CACHE_ON", CACHE_ON);
	def("CACHE_OFF", CACHE_OFF);
	def("CACHE_CLR", CACHE_CLR);
	def("IC_CLEAR", IC_CLEAR);
	def("DC_CLEAR", DC_CLEAR);

	/* pte/ste bits */
	def("PG_V", PG_V);
	def("PG_NV", PG_NV);
	def("PG_RO", PG_RO);
	def("PG_RW", PG_RW);
	def("PG_CI", PG_CI);
	def("PG_PROT", PG_PROT);
	def("PG_FRAME", PG_FRAME);
	def("SG_V", SG_V);
	def("SG_NV", SG_NV);
	def("SG_RW", SG_RW);
	def("SG_FRAME", SG_FRAME);
	def("SG_ISHIFT", SG_ISHIFT);

	/* pcb fields */
	off("PCB_PS", struct pcb, pcb_ps);
	off("PCB_USTP", struct pcb, pcb_ustp);
	off("PCB_USP", struct pcb, pcb_usp);
	off("PCB_REGS", struct pcb, pcb_regs);
	off("PCB_ONFAULT", struct pcb, pcb_onfault);
	off("PCB_FPCTX", struct pcb, pcb_fpregs);
	def("SIZEOF_PCB", sizeof(struct pcb));

	/* exception frame offset/sizes */
	off("FR_SP", struct frame, f_regs[15]);
	off("FR_HW", struct frame, f_sr);
	off("FR_ADJ", struct frame, f_stackadj);
	def("FR_SIZE", sizeof(struct trapframe));

	/* system calls */
	def("SYS_exit", SYS_exit);
	def("SYS_execve", SYS_execve);
	def("SYS_sigreturn", SYS_sigreturn);

	/* errno */
	def("EFAULT", EFAULT);
	def("ENAMETOOLONG", ENAMETOOLONG);

	/* clock registers */
	def("CLKSR", CLKSR);
	def("CLKMSB1", CLKMSB1);
	def("CLKMSB3", CLKMSB3);

#ifdef USELEDS
	/* LEDs */
	def("LED_PULSE", LED_PULSE);
	def("LED_DISK", LED_DISK);
	def("LED_LANRCV", LED_LANRCV);
	def("LED_LANXMT", LED_LANXMT);
#endif

	exit(0);
}
