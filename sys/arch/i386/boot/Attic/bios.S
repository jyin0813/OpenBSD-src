/*	$NetBSD: bios.S,v 1.13 1996/01/06 22:15:29 mycroft Exp $	*/

/*
 * Ported to boot 386BSD by Julian Elischer (julian@tfs.com) Sept 1992
 *
 * Mach Operating System
 * Copyright (c) 1992, 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

/*
  Copyright 1988, 1989, 1990, 1991, 1992 
   by Intel Corporation, Santa Clara, California.

                All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Intel
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <machine/asm.h>
#define	addr32	.byte 0x67
#define	data32	.byte 0x66

/*
# BIOS call "INT 0x13 Function 0x2" to read sectors from disk into memory
#	Call with	%ah = 0x2
#			%al = number of sectors
#			%ch = cylinder
#			%cl = sector
#			%dh = head
#			%dl = drive (0x80 for hard disk, 0x0 for floppy disk)
#			%es:%bx = segment:offset of buffer
#	Return:		
#			%al = 0x0 on success; err code on failure
*/
ENTRY(biosread)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi

	movb	16(%ebp), %dh
	movw	12(%ebp), %cx
	xchgb	%ch, %cl	# cylinder; the highest 2 bits of cyl is in %cl
	rorb	$2, %cl
	movb	20(%ebp), %al
	orb	%al, %cl
	incb	%cl		# sector; sec starts from 1, not 0
	movb	8(%ebp), %dl	# device
	movl	28(%ebp), %ebx	# offset
				# prot_to_real will set %es to BOOTSEG

	call	_C_LABEL(prot_to_real)	# enter real mode

	movb	$0x2, %ah	# subfunction
	addr32
	movb	24(%ebp), %al	# number of sectors
	int	$0x13
	setc	%bl

	data32
	call	_C_LABEL(real_to_prot) # back to protected mode

	xorl	%eax, %eax
	movb	%bl, %al	# return value in %ax

	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret


#ifdef DOSREAD
/*
# MSDOS call "INT 0x21 Function 0x3d" to open a file.
# Call with   %ah = 0x3d
#             %al = 0x0  (access and sharing modes)
#             %ds:%dx = ASCIZ filename
#             %cl = attribute mask of files to look for
*/
ENTRY(dosexit)
      pushl %ebp
      movl  %esp, %ebp
      pushl   %ebx
      pushl   %esi
      pushl   %edi
      movl    0x8(%ebp), %ebx # exit code
      call    _C_LABEL(prot_to_real)  # enter real mode
      movb    %bl, %al        # exit code
      movb    $0x4c , %ah     # Exit
      int     $0x21
      cli
      hlt

ENTRY(dosopen)
      pushl %ebp
      movl  %esp, %ebp
      pushl   %ebx
      pushl   %esi
      pushl   %edi
      movl    0x8(%ebp), %edx # File name.
      movb    $0x0 , %cl      # Attribute mask.
      call    _C_LABEL(prot_to_real)  # enter real mode
      movb    $0x3d, %ah      # Open existing file.
      movb    $0x0 , %al      # Compatibility mode
      int     $0x21
      jnc     ok1
      addr32
      movl    %eax, _C_LABEL(doserrno)
      data32
      movl    $-1, %eax
ok1:
      pushl   %eax
      data32
      call    _C_LABEL(real_to_prot) # back to protected mode
      xorl    %eax, %eax
      popw    %ax             # return value in %eax
      popl    %edi
      popl    %esi
      popl    %ebx
      popl    %ebp
      ret
ENTRY(dosread)
      pushl %ebp
      movl  %esp, %ebp
      pushl   %ebx
      pushl   %esi
      pushl   %edi
      movl    0x8(%ebp), %ebx # File handle
      movl    0xc(%ebp), %edx # Buffer.
      movl    0x10(%ebp) , %ecx       # Bytes to read
      call    _C_LABEL(prot_to_real)  # enter real mode
      movb    $0x3f, %ah      # Read from file or device
      movb    $0x0 , %al      # Compatibility mode
      int     $0x21
      jnc     ok2
      addr32
      movl    %eax, _C_LABEL(doserrno)
      data32
      movl    $-1, %eax
ok2:
      pushl   %eax
      data32
      call    _C_LABEL(real_to_prot) # back to protected mode
      xorl    %eax, %eax
      popw    %ax             # return value in %eax
      popl    %edi
      popl    %esi
      popl    %ebx
      popl    %ebp
      ret
ENTRY(dosclose)
      pushl %ebp
      movl  %esp, %ebp
      pushl   %ebx
      pushl   %esi
      pushl   %edi
      movl    0x8(%ebp), %ebx # File handle
      call    _C_LABEL(prot_to_real)  # enter real mode
      movb    $0x3e, %ah      # Close file.
      movb    $0x0 , %al      # Compatibility mode
      int     $0x21
      jnc     ok3
      addr32
      movl    %eax, _C_LABEL(doserrno)
      data32
      movl    $-1, %eax
ok3:
      pushl   %eax
      data32
      call    _C_LABEL(real_to_prot) # back to protected mode
      xorl    %eax, %eax
      popw    %ax             # return value in %eax
      popl    %edi
      popl    %esi
      popl    %ebx
      popl    %ebp
      ret
#endif

#ifndef SERIAL
/*
# BIOS call "INT 10H Function 0Eh" to write character to console
#	Call with	%ah = 0x0e
#			%al = character
#			%bh = page
#			%bl = foreground color
*/
ENTRY(putc)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi

	movb	8(%ebp), %cl

	call	_C_LABEL(prot_to_real)

	movb	%cl, %al

	movb	$0x0e, %ah
	data32
	movl	$0x0001, %ebx
	int	$0x10		# display a byte

	data32
	call	_C_LABEL(real_to_prot)

	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret


/*
# BIOS call "INT 16H Function 00H" to read character from the keyboard
#	Call with	%ah = 0x00
#	Return:		%ah = keyboard scan code
#			%al = ASCII character
*/
ENTRY(getc)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi

	call	_C_LABEL(prot_to_real)

	movb	$0x00, %ah
	int	$0x16

	movb	%al, %bl	# real_to_prot uses %eax

	data32
	call	_C_LABEL(real_to_prot)

	xorl	%eax, %eax
	movb	%bl, %al

	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret


/*
# BIOS call "INT 16H Function 01H" to check whether a character is pending
#	Call with	%ah = 0x01
#	Return:	
#		If key waiting to be input:
#			%ah = keyboard scan code
#			%al = ASCII character
#			ZF  = clear
#		else
#			ZF  = set
*/
ENTRY(ischar)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi

	call	_C_LABEL(prot_to_real)

	movb	$0x01, %ah
	int	$0x16
	setnz	%ah

	movb	%ah, %bl	# real_to_prot uses %eax

	data32
	call	_C_LABEL(real_to_prot)

	xorl	%eax, %eax
	movb	%bl, %al

	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret

#endif /* #ifndef SERIAL */

/*
#
# get_diskinfo():  return a word that represents the
#	max number of sectors and  heads and drives for this device
#
*/

ENTRY(get_diskinfo)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi

	movb	8(%ebp), %dl		# diskinfo(drive #)

	call	_C_LABEL(prot_to_real)	# enter real mode

	movb	$0x08, %ah		# ask for disk info
	int	$0x13
	jnc	ok

	/*
	 * Urk.  Call failed.  It is not supported for floppies by old BIOS's.
	 * Guess it's a 15-sector floppy.  Initialize all the registers for
	 * documentation, although we only need head and sector counts.
	 */
	subb	%ah, %ah		# %ax = 0
	movb	%ah, %bh		# %bh = 0
	movb	$2, %bl			# %bl bits 0-3 = drive type, 2 = 1.2M
	movb	$79, %ch		# max track
	movb	$15, %cl		# max sector
	movb	$1, %dh			# max head
	movb	$1, %dl			# # floppy drives installed
	# es:di = parameter table
	# carry = 0

ok:
	data32
	call	_C_LABEL(real_to_prot)	# back to protected mode

	xorl	%eax, %eax

	/*form a longword representing all this gunk*/
	movb	%dh, %ah		# max head
	andb	$0x3f, %cl		# mask of cylinder gunk
	movb	%cl, %al		# max sector (and # sectors)

	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret

/*
#
# memsize(i) :  return the memory size in KB. i == 0 for conventional memory,
#		i == 1 for extended memory
#	BIOS call "INT 12H" to get conventional memory size
#	BIOS call "INT 15H, AH=88H" to get extended memory size
#		Both have the return value in AX.
#
*/

ENTRY(memsize)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi

	movl	8(%ebp), %ebx

	call	_C_LABEL(prot_to_real)	# enter real mode

	testb	%bl, %bl
	data32
	jnz	xext
	
	int	$0x12
	data32
	jmp	xdone

xext:
	movb	$0x88, %ah
	int	$0x15

xdone:
	movl	%eax, %ebx

	data32
	call	_C_LABEL(real_to_prot)

	movl	%ebx, %eax
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret

/*
# BIOS call "INT 15H Function 86H" to sleep for a set number of microseconds
#	Call with	%ah = 0x86
#			%cx = time interval (high)
#			%dx = time interval (low)
#	Return:	
#		If error
#			CF  = set
#		else
#			CF  = clear
*/
ENTRY(usleep)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx

	movw	8(%ebp), %dx
	movw	10(%ebp), %cx

	call	_C_LABEL(prot_to_real)

	movb	$0x86, %ah
	int	$0x15
	setnc	%ah

	movb	%ah, %bl	# real_to_prot uses %eax

	data32
	call	_C_LABEL(real_to_prot)

	xorl	%eax, %eax
	movb	%bl, %al

	popl	%ebx
	popl	%ebp
	ret
