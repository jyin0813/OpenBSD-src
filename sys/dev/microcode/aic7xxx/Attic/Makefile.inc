#	$OpenBSD: Makefile.inc,v 1.5 1996/11/28 23:27:58 niklas Exp $
#	$NetBSD: Makefile.inc,v 1.4 1996/05/20 00:48:43 thorpej Exp $

.if target(aic7xxx.o)
PATH: $S/dev/microcode/aic7xxx

aic7xxx.o: aic7xxx_seq.h

aic7xxx_seq.h: aic7xxx_asm $S/dev/microcode/aic7xxx/aic7xxx.seq
	./aic7xxx_asm -o ${.TARGET} $S/dev/microcode/aic7xxx/aic7xxx.seq

aic7xxx_asm: $S/dev/microcode/aic7xxx/aic7xxx_asm.c
	${HOSTCC} -U_KERNEL -o ${.TARGET} $<

clean::
	rm -f aic7xxx_asm
.endif
