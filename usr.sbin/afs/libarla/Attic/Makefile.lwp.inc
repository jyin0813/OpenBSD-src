### lwp stuff
SRCS		+= lwp.c lock.c iomgr.c timer.c fasttime.c preempt.c q.c
OBJS		+= process.o
CLEANFILES	+= process.S process.o lwp.c lwp.h
.PATH:	${.CURDIR}/../src/lwp
CFLAGS	+= -DAFS_BSD_ENV -DFD_SPEED_HACK -I${.CURDIR}/../src/lwp

# We use the handcoded assembler stuff for context switching, not
# pthreads (because pthreads do not work for now).

beforedepend: lwp.c lwp.h

lwp.h:
	ln -s ${.CURDIR}/../src/lwp/lwp_asm.h lwp.h
lwp.c:
	ln -s ${.CURDIR}/../src/lwp/lwp_asm.c lwp.c

process.S:
	HOST_CPU=${MACHINE_ARCH}; \
	case "$$HOST_CPU" in \
		sparc64) PROCESS_S="process.sparc.S" ;; \
		sparc)   PROCESS_S="process.sparc.S" ;; \
		i*86*)   PROCESS_S="process.i386.S" ;; \
		m68k)    PROCESS_S="process.m68k.S" ;; \
		mips*)   PROCESS_S="process.mips.S" ;; \
		alpha*)  PROCESS_S="process.alpha.S" ;; \
		hppa*)   PROCESS_S="process.hpux.S" ;; \
		powerpc) PROCESS_S="process.ppc.S" ;; \
		rs6000)  PROCESS_S="process.rios.S" ;; \
                vax)     PROCESS_S="process.vax.S" ;; \
		*)  echo "Unknown host_cpu, good luck" ;; \
	esac; \
	ln -s ${.CURDIR}/../src/lwp/$$PROCESS_S process.S

process.o: process.S
	${CC} ${CFLAGS} -DHAVE_PIC -c process.S
