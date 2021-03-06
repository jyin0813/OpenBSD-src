/*	$OpenBSD: hpetreg.h,v 1.2 2005/07/10 17:24:18 grange Exp $	*/
/*
 * Copyright (c) 2005 Thorsten Lockert <tholo@sigmasoft.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _DEV_ACPI_HPETREG_H_
#define _DEV_ACPI_HPETREG_H_

#define	HPET_REG_SIZE		1024

#define	HPET_CAPABILITIES	0x000
#define	HPET_CONFIGURATION	0x010
#define	HPET_INTERRUPT_STATUS	0x020
#define	HPET_MAIN_COUNTER	0x0F0
#define	HPET_TIMER0_CONFIG	0x100
#define	HPET_TIMER0_COMPARE	0x108
#define	HPET_TIMER0_INTERRUPT	0x110
#define	HPET_TIMER1_CONFIG	0x200
#define	HPET_TIMER1_COMPARE	0x208
#define	HPET_TIMER1_INTERRUPT	0x310
#define	HPET_TIMER2_CONFIG	0x400
#define	HPET_TIMER2_COMPARE	0x408
#define	HPET_TIMER2_INTERRUPT	0x510

#endif	/* !_DEV_ACPI_HPETREG_H_ */
