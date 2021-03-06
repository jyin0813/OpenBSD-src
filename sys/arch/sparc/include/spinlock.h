/*	$OpenBSD: spinlock.h,v 1.1 1999/01/08 08:25:36 d Exp $	*/

#ifndef _MACHINE_SPINLOCK_H_
#define _MACHINE_SPINLOCK_H_

#define _SPINLOCK_UNLOCKED	(0x00)
#define _SPINLOCK_LOCKED	(0xFF)
typedef unsigned char _spinlock_lock_t;

#endif
