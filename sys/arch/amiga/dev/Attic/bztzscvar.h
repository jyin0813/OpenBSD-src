/*	$OpenBSD: bztzscvar.h,v 1.1 1997/01/16 09:23:51 niklas Exp $	*/
/*	$NetBSD: bztzscvar.h,v 1.1 1996/12/16 16:17:30 is Exp $	*/

/*
 * Copyright (c) 1996 Ignatios Souvatzis
 * Copyright (c) 1995 Daniel Widenfalk
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
 *      This product includes software developed by Daniel Widenfalk
 *      for the NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _BZTZSCVAR_H_
#define _BZTZSCVAR_H_

#ifndef _SFASVAR_H_
#include <amiga/dev/sfasvar.h>
#endif

#ifndef _BZTZSCREG_H_
#include <amiga/dev/bztzscreg.h>
#endif

struct	bztzsc_softc {
	struct sfas_softc	sc_softc;
	bztzsc_regmap_t		sc_regmap;
};

#endif /* _BZTZSCVAR_H_ */
