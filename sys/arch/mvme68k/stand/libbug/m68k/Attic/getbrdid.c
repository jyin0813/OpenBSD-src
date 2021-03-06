/*	$OpenBSD: getbrdid.c,v 1.2 1996/04/28 10:48:39 deraadt Exp deraadt $ */

/*
 * bug routines -- assumes that the necessary sections of memory
 * are preserved.
 */
#include <sys/types.h>
#include <machine/prom.h>

/* BUG - query board routines */
struct mvmeprom_brdid *
mvmeprom_getbrdid()
{
	struct mvmeprom_brdid *id;

	asm volatile ("clrl sp@-");
	MVMEPROM_CALL(MVMEPROM_GETBRDID);
	asm volatile ("movel sp@+,%0": "=d" (id):);
	return (id);
}
