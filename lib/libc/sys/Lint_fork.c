/*	$OpenBSD: Lint_fork.c,v 1.1 1998/02/08 22:45:10 tholo Exp $	*/
/*	$NetBSD: Lint_fork.c,v 1.1 1997/11/06 00:52:57 cgd Exp $	*/

/*
 * This file placed in the public domain.
 * Chris Demetriou, November 5, 1997.
 */

#include <unistd.h>

/*ARGSUSED*/
pid_t
fork(void)
{
	return (0);
}
