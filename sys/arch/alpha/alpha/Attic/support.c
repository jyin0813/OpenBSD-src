/*	$OpenBSD: support.c,v 1.5 1997/01/24 19:56:44 niklas Exp $	*/
/*	$NetBSD: support.c,v 1.4 1996/11/13 22:20:56 cgd Exp $	*/

/*
 * Copyright (c) 1994, 1995, 1996 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 * 
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND 
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

/*
 * Some C support functions that aren't (yet) in libkern or assembly.
 */
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/errno.h>

struct qelem {
	struct qelem *q_forw;
	struct qelem *q_back;
};

void
_insque(entry, pred)
	void *entry;
	void *pred;
{
	struct qelem *e = (struct qelem *) entry;
	struct qelem *p = (struct qelem *) pred;

	e->q_forw = p->q_forw;
	e->q_back = p;
	p->q_forw->q_back = e;
	p->q_forw = e;
}

void
_remque(element)
	void *element;
{
	struct qelem *e = (struct qelem *) element;
	e->q_forw->q_back = e->q_back;
	e->q_back->q_forw = e->q_forw;
}
