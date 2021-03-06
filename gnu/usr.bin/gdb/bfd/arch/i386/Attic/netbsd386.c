/* BFD back-end for NetBSD/386 a.out-ish binaries.
   Copyright (C) 1990, 1991, 1992 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	$Id: netbsd386.c,v 1.1 1995/10/18 08:39:54 deraadt Exp $
*/

#define	BYTES_IN_WORD	4
#define	ARCH		32
#undef TARGET_IS_BIG_ENDIAN_P
#undef HOST_BIG_ENDIAN_P

#define	PAGE_SIZE	4096
#define	SEGMENT_SIZE	PAGE_SIZE
#define __LDPGSZ	4096

#define	DEFAULT_ARCH	bfd_arch_i386
#define MACHTYPE_OK(mtype) ((mtype) == M_386 || (mtype) == M_386_NETBSD || (mtype) == M_UNKNOWN)

#define MY(OP) CAT(netbsd_386_,OP)
/* This needs to start with a.out so GDB knows it is an a.out variant.  */
#define TARGETNAME "a.out-netbsd-386"

#include "netbsd.h"

