/*	$OpenBSD: cache_r5k.S,v 1.35 2012/04/21 12:20:30 miod Exp $ */

/*
 * Copyright (c) 1998-2004 Opsycon AB (www.opsycon.se)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * Processors supported:
 * R4600/R4700 (if option CPU_R4600)
 * R5000, RM52xx, RM7xxx, RM9xxx
 *
 * The following assumptions are made:
 * - L1 I$ is 2 way, 32 bytes/line
 * - L1 D$ is WB, 2 way, 32 bytes/line
 * - L2 may not exist
 * - L3 may not exist
 * - L3 implies internal L2
 * - all external caches are WT
 */

#include <sys/errno.h>
#include <sys/syscall.h>

#include <machine/param.h>
#include <machine/asm.h>
#include <machine/cpu.h>
#include <machine/regnum.h>

#include "assym.h"

	.set	mips3

/*
 *  Skip the .h file. Noone else need to know!
 */

#define	IndexInvalidate_I	0x00
#define	IndexWBInvalidate_D	0x01
#define	IndexWBInvalidate_S	0x03

#define	IndexStoreTag_S		0x0b

#define	HitInvalidate_D		0x11
#define	HitInvalidate_S		0x13

#define	HitWBInvalidate_D	0x15
#define	InvalidatePage_T	0x16
#define	HitWBInvalidate_S	0x17
#define	InvalidatePage_S	0x17	/* Only RM527[0-1] */

/*
 *  R5000 and RM52xx config register bits.
 */
#define	CF_5_SE		(1 << 12)	/* Secondary cache enable */
#define	CF_5_SC		(1 << 17)	/* Secondary cache not present */
#define	CF_5_SS		(3 << 20)	/* Secondary cache size */
#define	CF_5_SS_AL	20		/* Shift to align */

/*
 *  RM7000 config register bits.
 */
#define	CF_7_SE		(1 << 3)	/* Secondary cache enable */
#define	CF_7_SC		(1 << 31)	/* Secondary cache not present */
#define	CF_7_TE		(1 << 12)	/* Tertiary cache enable */
#define	CF_7_TC		(1 << 17)	/* Tertiary cache not present */
#define	CF_7_TS		(3 << 20)	/* Tertiary cache size */
#define	CF_7_TS_AL	20		/* Shift to align */

/*
 *  Define cache type definition bits. NOTE! the 3 lsb may NOT change!
 */
#define	CTYPE_DIR		0x0001	/* Cache is direct mapped */
#define	CTYPE_2WAY		0x0002	/* Cache is TWO way */
#define	CTYPE_4WAY		0x0004	/* Cache is FOUR way */
#define	CTYPE_WAYMASK		0x0007

#define	CTYPE_HAS_IL2		0x0100	/* Internal L2 Cache present */
#define	CTYPE_HAS_XL2		0x0200	/* External L2 Cache present */
#define	CTYPE_HAS_XL3		0x0400	/* External L3 Cache present */

/*
 *  Due to a flaw in RM7000 1.x processors a pipeline 'drain' is
 *  required after some mtc0 instructions.
 *  Ten nops in sequence does the trick.
 */
#define NOP10	nop;nop;nop;nop;nop;\
		nop;nop;nop;nop;nop	/* Two cycles for dual issue machine */

	.set	noreorder		# Noreorder is default style!

/*----------------------------------------------------------------------------
 *
 * Mips5k_ConfigCache(struct cpu_info *ci) --
 *
 *	Size and configure the caches.
 *	NOTE: should only be called from mips_init().
 *
 * Side effects:
 *	The size of the data cache is stored into ci_l1datacachesize.
 *	The size of instruction cache is stored into ci_l1instcachesize.
 *	Alignment mask for cache aliasing test is stored in cache_valias_mask.
 *	ci_l2size is set to the size of the secondary cache.
 *	ci_l3size is set to the size of the tertiary cache.
 *	ci_cacheways is set to 0 for direct mapped caches, 2 for two way
 *	caches and 4 for four way caches. This primarily indicates the
 *	primary cache associativity.
 *
 * Allocation:
 *	ta0, ta1 ta2 used to hold I and D set size and Alias mask.
 *
 *----------------------------------------------------------------------------
 */
LEAF(Mips5k_ConfigCache, 0)
	.set	noreorder
	LA	v0, 1f
	LA	v1, CKSEG1_BASE
	or	v0, v1
	jr	v0				# Switch to uncached.
	nop
1:
	mfc0	v1, COP_0_PRID			# read processor ID register
	mfc0	v0, COP_0_CONFIG		# Get configuration register

	srl	t1, v0, 9			# Get I cache size.
	and	t1, 7
	li	t2, 4096
	sllv	ta0, t2, t1			# ta0 = Initial I set size.

	and	t2, v0, 0x20
	srl	t2, 1				# Get I cache line size.
	addu	t2, 16
	sw	t2, CI_L1INSTCACHELINE(a0)

	srl	t1, v0, 6			# Get D cache size.
	and	t1, 7
	li	t2, 4096			# Fixed page size.
	sllv	ta1, t2, t1

	and	t2, v0, 0x10
	addu	t2, 16				# Get D cache line size.
	sw	t2, CI_L1DATACACHELINE(a0)

	li	t2, CTYPE_2WAY			# Assume two way cache
	li	ta2, 0				# Secondary size 0.
	li	ta3, 0				# Tertiary size 0.

	and	v1, 0xff00			# Recognize CPU's with
	li	t1, (MIPS_R5000 << 8)		# N way L1 caches only.
	beq	v1, t1, Conf5K			# R5K 2 way, check L2
	li	t1, (MIPS_RM52X0 << 8)
	beq	v1, t1, Conf5K			# RM52xx 2 way, check L2
	li	t1, (MIPS_RM7000 << 8)
	beq	v1, t1, Conf7K
	li	t1, (MIPS_RM9000 << 8)
	beq	v1, t1, Conf7K
	nop

	b	ConfResult			# R4[67]00 2 way, No L2 control
	nop

#---- R5K ------------------------------
Conf5K:						# R5xxx type, check for L2 cache
	and	t1, v0, CF_5_SC
	bnez	t1, ConfResult			# not enabled
	li	ta2, 0				# set size to 0.

	li	t3, CF_5_SS
	and	t1, t3, v0
	beq	t1, t3, ConfResult		# No L2 cache
	srl	t1, CF_5_SS_AL

	li	t3, CF_5_SE			# Set SE in conf
	or	v0, t3				# Update config register
	li	ta2, 512*1024			# 512k per 'click'.
	sll	ta2, t1

	mtc0	v0, COP_0_CONFIG		# Enable L2 cache
	or	t2, CTYPE_HAS_XL2		# External L2 present.
	mtc0	zero, COP_0_TAG_LO		# necessary for RM52xx
	LOAD_XKPHYS(t0, CCA_CACHED)
	PTR_ADDU t1, t0, ta2
1:
	cache	InvalidatePage_S, 0(t0)
	PTR_ADDU t0, 4096
	bne	t0, t1, 1b
	nop

	b	ConfResult
	nop

#---- RM7K -----------------------------
Conf7K:					# RM7000, check for L2 and L3 cache
	li	t2, CTYPE_4WAY			# 4-way cache
	and	t1, v0, CF_7_TC
	bnez	t1, Conf7KL2			# No L3 cache if set
	li	ta3, 0				# Set size = 0

#ifndef L3SZEXT
	li	t3, CF_7_TS
	and	t1, t3, v0
	beq	t1, t3, Conf7KL2		# No L3 cache
	srl	t1, CF_7_TS_AL

	or	t2, CTYPE_HAS_XL3		# External L2 present.
	li	t3, CF_7_TE			# Set TE in conf
	or	v0, t3				# Update config register
	li	ta3, 512*1024			# 512k per 'click'.
	sll	ta3, t1
#else
	lw	ta3, CI_L3SIZE(a0)
	and	t2, ~CTYPE_HAS_XL3
	beqz	ta3, Conf7KL2			# No L3 cache present
	nop

	li	t3, CF_7_TE			# Set SE in conf
	or	v0, t3				# Update config register
	mtc0	v0, COP_0_CONFIG		# Enable L3 cache
	or	t2, CTYPE_HAS_XL3
#endif

	mtc0	zero, COP_0_TAG_LO
	LOAD_XKPHYS(t0, CCA_CACHED)
	PTR_ADDU t1, t0, ta3
1:
	cache	InvalidatePage_T, 0(t0)
	PTR_ADDU t0, 4096
	bne	t0, t1, 1b
	nop

Conf7KL2:
	and	t1, v0, CF_7_SC			# check for L2 cache
	bnez	t1, ConfResult
	li	ta2, 0				# No L2?

	and	t1, v0, CF_7_SE
	bnez	t1, 3f
	ori	v0, CF_7_SE

	mtc0	v0, COP_0_CONFIG		# Enable and init L2 cache
	LOAD_XKPHYS(t0, CCA_CACHED)
	PTR_ADDU t1, t0, ta3
1:
	PTR_ADDU t0, 32
	bne	t0, t1, 1b
	cache	IndexStoreTag_S, -4(t0)
	sync

	LOAD_XKPHYS(t0, CCA_CACHED)
	PTR_ADDU t1, t0, ta3
1:
	PTR_ADDU t0, 32
	bne	t0, t1, 1b
	lw	zero, -4(t0)
	sync

	LOAD_XKPHYS(t0, CCA_CACHED)
	PTR_ADDU t1, t0, ta3
1:
	PTR_ADDU t0, 32
	bne	t0, t1, 1b
	cache	IndexStoreTag_S, -4(t0)
	sync

3:
	or	t2, CTYPE_HAS_IL2		# L2 is on chip
	b	ConfResult
	li	ta2, 256*1024			# L2 size = 256k

/*
 * Get here with t2 = Cache type, ta0 = L1 I size, ta1 = L1 D size.
 * ta2 = secondary size, ta3 = tertiary size.
 */
ConfResult:
	sw	t2, CI_CACHECONFIGURATION(a0)	# Save cache attributes
	and	t2, CTYPE_WAYMASK		# isolate number of sets.
	sw	t2, CI_CACHEWAYS(a0)
	srl	t2, 1				# get div shift for set size.

	sw	ta2, CI_L2SIZE(a0)
	sw	ta3, CI_L3SIZE(a0)

	addu	t1, ta0, -1			# Use icache for alias mask
	srl	t1, t2
	srl	t1, PAGE_SHIFT
	beqz	t1, 1f
	sll	t1, PAGE_SHIFT
	or	t1, (PAGE_SIZE - 1)
1:
	PTR_S	t1, cache_valias_mask
	PTR_S	t1, pmap_prefer_mask

	sw	ta0, CI_L1INSTCACHESIZE(a0)	# store cache size.
	srl	ta0, t2				# calculate set size.
	sw	ta0, CI_L1INSTCACHESET(a0)

	sw	ta1, CI_L1DATACACHESIZE(a0)	# store cache size.
	srl	ta1, t2				# calculate set size.
	sw	ta1, CI_L1DATACACHESET(a0)

	and	v0, ~7
	or	v0, CCA_CACHED			# set cachable writeback kseg0
	mtc0	v0, COP_0_CONFIG		# establish any new config
	NOP10
	j	ra
	nop
END(Mips5k_ConfigCache)

/*----------------------------------------------------------------------------
 *
 * Mips5k_SyncCache(struct cpu_info *ci) --
 *
 *	Sync ALL caches.
 *	No need to look at number of sets since we are cleaning out
 *	the entire cache and thus will address all sets anyway.
 *
 * Side effects:
 *	The contents of ALL caches are Invalidated or Synched.
 *
 *----------------------------------------------------------------------------
 */
LEAF(Mips5k_SyncCache, 0)
	.set	noreorder
	lw	t1, CI_L1INSTCACHESIZE(a0)
	lw	t2, CI_L1DATACACHESIZE(a0)

/*
 * Sync the instruction cache.
 */
#ifdef CPUR4600
	mfc0	v1, COP_0_STATUS_REG		# Save the status register.
	li	v0, SR_DIAG_DE
	mtc0	v0, COP_0_STATUS_REG		# Disable interrupts
#endif

	LOAD_XKPHYS(t0, CCA_CACHED)
	PTR_ADDU t1, t0, t1			# Compute end address
	PTR_SUBU t1, 128

1:
	cache	IndexInvalidate_I, 0(t0)
	cache	IndexInvalidate_I, 32(t0)
	cache	IndexInvalidate_I, 64(t0)
	cache	IndexInvalidate_I, 96(t0)

	bne	t0, t1, 1b
	PTR_ADDU t0, 128

/*
 * Sync the data cache. Do L1 first. Indexed only operate on
 * the selected cache and differs from Hit in that sense.
 */

	LOAD_XKPHYS(t0, CCA_CACHED)
	PTR_ADDU t1, t0, t2			# End address
	PTR_SUBU t1, 128
1:
	cache	IndexWBInvalidate_D, 0(t0)
	cache	IndexWBInvalidate_D, 32(t0)
	cache	IndexWBInvalidate_D, 64(t0)
	cache	IndexWBInvalidate_D, 96(t0)

	bne	t0, t1, 1b
	PTR_ADDU t0, 128

/* Do on chip L2 if present */
	lw	t0, CI_CACHECONFIGURATION(a0)
	and	t0, CTYPE_HAS_IL2
	beqz	t0, 20f
	nop

3:
	LOAD_XKPHYS(t3, CCA_CACHED)
	lw	ta0, CI_L2SIZE(a0)
1:
	cache	IndexWBInvalidate_S, 0(t3)
	PTR_SUBU ta0, 32			# Fixed cache line size.
	bgtz	ta0, 1b
	PTR_ADDU t3, 32

/* Do off chip L2 if present */
20:
	lw	t0, CI_CACHECONFIGURATION(a0)
	and	t0, CTYPE_HAS_XL2
	beqz	t0, 30f
	nop

	mtc0    zero, COP_0_TAG_LO
	LOAD_XKPHYS(t3, CCA_CACHED)
	lw	ta0, CI_L2SIZE(a0)
1:
	cache	InvalidatePage_S, 0(t3)
	PTR_SUBU ta0, 4096			# Fixed external cache page size
	bgtz	ta0, 1b
	PTR_ADDU t3, 4096

/* Do off chip L3 if present */
30:
	lw	t0, CI_CACHECONFIGURATION(a0)
	and	t0, CTYPE_HAS_XL3
	beqz	t0, 99f
	nop

	mtc0    zero, COP_0_TAG_LO
	LOAD_XKPHYS(t3, CCA_CACHED)
	lw	ta0, CI_L3SIZE(a0)
1:
	cache	InvalidatePage_T, 0(t3)
	PTR_SUBU ta0, 4096			# Fixed external cache page size
	bgtz	ta0, 1b
	PTR_ADDU t3, 4096

99:
#ifdef CPUR4600
	mtc0	v1, COP_0_STATUS_REG	# Restore the status register.
	NOP10
#endif
	sync
	j	ra
	nop
END(Mips5k_SyncCache)

/*----------------------------------------------------------------------------
 *
 * Mips5k_SyncICache(struct cpu_info *, vaddr_t va, size_t len)
 *
 *	Invalidate the L1 instruction cache for at least range of va to
 *	va + len - 1.
 *
 * Side effects:
 *	The contents of the L1 Instruction cache is flushed.
 *
 *----------------------------------------------------------------------------
 */
LEAF(Mips5k_InvalidateICache, 0)
#ifdef CPUR4600
	mfc0	v1, COP_0_STATUS_REG		# Save the status register.
	li	v0, SR_DIAG_DE
	mtc0	v0, COP_0_STATUS_REG		# Disable interrupts
#endif
	lw	v0, CI_CACHEWAYS(a0)		# Cache properties
	lw	t0, CI_L1INSTCACHESET(a0)	# Set size
	dsll	a1, (64 - 57)
	dsrl	a1, (64 - 57)
	LOAD_XKPHYS(a3, CCA_CACHED)
	PTR_ADDU a2, 31				# Round up size
	PTR_ADDU a2, a1				# Add extra from address
	dsrl	a1, 5				# Align start address
	dsll	a1, 5
	PTR_SUBU a2, a1
	PTR_ADDU a1, a3				# a1 now new XKPHYS address
	dsrl	a2, 5				# Number of unrolled loops
	addiu	v0, -2				# <0 1way, 0 = two, >0 four
1:
	bltz	v0, 3f
	PTR_ADDU a2, -1

2:
	PTR_ADDU t1, t0, a1			# Nway cache, flush set B.
	cache	IndexInvalidate_I, 0(t1)
	beqz	v0, 3f				# If two way do set A
	PTR_ADDU t1, t0				# else step to set C.

	cache	IndexInvalidate_I, 0(t1)

	PTR_ADDU t1, t0				# step to set D
	cache	IndexInvalidate_I, 0(t1)

3:
	cache	IndexInvalidate_I, 0(a1)	# do set (A if NWay)

	bnez	a2, 1b
	PTR_ADDU a1, 32

#ifdef CPUR4600
	mtc0	v1, COP_0_STATUS_REG	# Restore the status register.
	NOP10
#endif
	sync
	j	ra
	nop
END(Mips5k_InvalidateICache)

/*----------------------------------------------------------------------------
 *
 * Mips5k_SyncDCachePage(struct cpu_info *ci, vaddr_t va, paddr_t pa)
 *
 *	Sync the L1 data cache page for address va.
 *	The physical address is used to compute the L2 index.
 *
 * Side effects:
 *	The contents of the cache is written back to primary memory.
 *	The cache line is invalidated.
 *
 *----------------------------------------------------------------------------
 */
LEAF(Mips5k_SyncDCachePage, 0)
#ifdef CPUR4600
	mfc0	v1, COP_0_STATUS_REG		# Save the status register.
	li	v0, SR_DIAG_DE
	mtc0	v0, COP_0_STATUS_REG		# Disable interrupts
#endif
	LOAD_XKPHYS(a3, CCA_CACHED)
	lw	v0, CI_CACHEWAYS(a0)
	dsll	a1, (64 - 57)
	dsrl	a1, (64 - 57) + PAGE_SHIFT
	dsll	a1, PAGE_SHIFT			# Page align start address
	PTR_ADDU a1, a3				# a1 now new XKPHYS address
	PTR_ADDU a4, a1, PAGE_SIZE-128
	addiu	v0, -2				# <0 1way, 0 = two, >0 four
	lw	a3, CI_L1DATACACHESET(a0)

1:
	bltz	v0, 3f
	PTR_ADDU t1, a1, a3
	cache	IndexWBInvalidate_D, 0(t1)	# flush set B.
	cache	IndexWBInvalidate_D, 32(t1)
	cache	IndexWBInvalidate_D, 64(t1)
	cache	IndexWBInvalidate_D, 96(t1)
	beqz	v0, 3f				# two way, skip C and D.
	PTR_ADDU t1, a3

	cache	IndexWBInvalidate_D, 0(t1)	# do set C
	cache	IndexWBInvalidate_D, 32(t1)
	cache	IndexWBInvalidate_D, 64(t1)
	cache	IndexWBInvalidate_D, 96(t1)

	PTR_ADDU t1, a3				# do set D
	cache	IndexWBInvalidate_D, 0(t1)
	cache	IndexWBInvalidate_D, 32(t1)
	cache	IndexWBInvalidate_D, 64(t1)
	cache	IndexWBInvalidate_D, 96(t1)
3:
	cache	IndexWBInvalidate_D, 0(a1)	# do set A
	cache	IndexWBInvalidate_D, 32(a1)
	cache	IndexWBInvalidate_D, 64(a1)
	cache	IndexWBInvalidate_D, 96(a1)

	bne	a4, a1, 1b
	PTR_ADDU a1, 128

	lw	t0, CI_CACHECONFIGURATION(a0)
	and	t0, CTYPE_HAS_IL2		# Have internal L2?
	beqz	t0, 9f

	LOAD_XKPHYS(a3, CCA_CACHED)		# Yes, do L2 with the physical
	dsrl	a2, PAGE_SHIFT
	dsll	a2, PAGE_SHIFT			# Page align start address
	PTR_ADDU a1, a2, a3			# address for the index
	PTR_ADDU a4, a1, PAGE_SIZE-128
	lw	a3, CI_L2SIZE(a0)
	srl	a3, 2				# Hardcoded 4-way

1:
	cache	IndexWBInvalidate_S, 0(a1)	# do set A
	cache	IndexWBInvalidate_S, 32(a1)
	cache	IndexWBInvalidate_S, 64(a1)
	cache	IndexWBInvalidate_S, 96(a1)

	PTR_ADDU t1, a1, a3
	cache	IndexWBInvalidate_S, 0(t1)	# do set B.
	cache	IndexWBInvalidate_S, 32(t1)
	cache	IndexWBInvalidate_S, 64(t1)
	cache	IndexWBInvalidate_S, 96(t1)

	PTR_ADDU t1, a3
	cache	IndexWBInvalidate_S, 0(t1)	# do set C
	cache	IndexWBInvalidate_S, 32(t1)
	cache	IndexWBInvalidate_S, 64(t1)
	cache	IndexWBInvalidate_S, 96(t1)

	PTR_ADDU t1, a3				# do set D
	cache	IndexWBInvalidate_S, 0(t1)
	cache	IndexWBInvalidate_S, 32(t1)
	cache	IndexWBInvalidate_S, 64(t1)
	cache	IndexWBInvalidate_S, 96(t1)

	bne	a4, a1, 1b
	PTR_ADDU a1, 128

9:
#ifdef CPUR4600
	mtc0	v1, COP_0_STATUS_REG	# Restore the status register.
#endif
	sync
	j	ra
	nop
END(Mips5k_SyncDCachePage)

/*----------------------------------------------------------------------------
 *
 * Mips5k_HitSyncDCache(struct cpu_info *ci, vaddr_t va, size_t len)
 *
 *	Sync data cache for range of va to va + len - 1.
 *	Only lines with matching addresses are flushed.
 *
 * Side effects:
 *	The contents of the L1 cache is written back to primary memory.
 *	The cache line is invalidated.
 *
 * IMPORTANT NOTE:
 *	Since orphaned L1 cache entries will not be synched it is
 *	mandatory to pass over the L1 cache once after the L2 is done.
 *
 *----------------------------------------------------------------------------
 */
LEAF(Mips5k_HitSyncDCache, 0)
#ifdef CPUR4600
	mfc0	v1, COP_0_STATUS_REG		# Save the status register.
	li	v0, SR_DIAG_DE
	mtc0	v0, COP_0_STATUS_REG		# Disable interrupts
#endif
	beqz	a2, 3f				# size is zero!
	PTR_ADDU a2, 31				# Round up
	PTR_ADDU a2, a1				# Add extra from address
	dsrl	a1, 5
	dsll	a1, 5				# align address
	PTR_SUBU a2, a1
	dsrl	a2, 5				# Compute number of cache lines

1:
	PTR_ADDU a2, -1
	cache	HitWBInvalidate_D, 0(a1)
	bnez	a2, 1b
	PTR_ADDU a1, 32

3:
#ifdef CPUR4600
	mtc0	v1, COP_0_STATUS_REG	# Restore the status register.
	NOP10
#endif
	sync
	j	ra
	nop
END(Mips5k_HitSyncDCache)


/*----------------------------------------------------------------------------
 *
 * _mips5k_HitSyncSCache(struct cpu_info *ci, vaddr_t va, size_t len)
 *
 *	Sync secondary cache for range of va to va + len - 1.
 *	Only lines with matching addresses are flushed.
 *
 * Side effects:
 *	The contents of the L2 cache is written back to primary memory.
 *	The cache line is invalidated.
 *
 * IMPORTANT NOTE:
 *	Since orphaned L1 cache entries will not be synched it is
 *	mandatory to pass over the L1 cache once after the L2 is done.
 *
 *----------------------------------------------------------------------------
 */
ALEAF(_mips5k_HitSyncSCache)
#ifdef CPUR4600
	mfc0	v1, COP_0_STATUS_REG		# Save the status register.
	li	v0, SR_DIAG_DE
	mtc0	v0, COP_0_STATUS_REG		# Disable interrupts
#endif

	beqz	a2, 3f				# size is zero!
	PTR_ADDU a2, 31				# Round up
	PTR_ADDU a2, a1				# Add in extra from align
	dsrl	a1, 5
	dsll	a1, 5				# align address
	PTR_SUBU a2, a1
	dsrl	a2, 5				# Compute number of cache lines
1:
	PTR_ADDU a2, -1
	cache	HitWBInvalidate_S, 0(a1)
	cache	HitWBInvalidate_D, 0(a1)	# Orphans in L1
	bnez	a2, 1b
	PTR_ADDU a1, 32

3:
#ifdef CPUR4600
	mtc0	v1, COP_0_STATUS_REG	# Restore the status register.
	NOP10
#endif
	sync
	j	ra
	nop

/*----------------------------------------------------------------------------
 *
 * Mips5k_HitInvalidateDCache(struct cpu_info *ci, vaddr_t va, size_t len)
 *
 *	Invalidate data cache for range of va to va + len - 1.
 *	Only lines with matching addresses are invalidated.
 *
 * Side effects:
 *	The L1 cache line is invalidated.
 *
 *----------------------------------------------------------------------------
 */
LEAF(Mips5k_HitInvalidateDCache, 0)
#ifdef CPUR4600
	mfc0	v1, COP_0_STATUS_REG		# Save the status register.
	li	v0, SR_DIAG_DE
	mtc0	v0, COP_0_STATUS_REG		# Disable interrupts
#endif

	beqz	a2, 3f				# size is zero!
	PTR_ADDU a2, 31				# Round up
	PTR_ADDU a2, a1				# Add in extra from align
	dsrl	a1, 5
	dsll	a1, 5				# align address
	PTR_SUBU a2, a1
	dsrl	a2, 5				# Compute number of cache lines
1:
	PTR_ADDU a2, -1
	cache	HitInvalidate_D, 0(a1)
	bnez	a2, 1b
	PTR_ADDU a1, 32

3:
#ifdef CPUR4600
	mtc0	v1, COP_0_STATUS_REG		# Restore the status register.
	NOP10
#endif
	sync
	j	ra
	nop
END(Mips5k_HitInvalidateDCache)

/*----------------------------------------------------------------------------
 *
 * _mips5k_HitInvalidateSCache(struct cpu_info *ci, vaddr_t va, size_t len)
 *
 *	Invalidate secondary cache for range of va to va + len - 1.
 *	Only lines with matching addresses are invalidated.
 *
 * Side effects:
 *	The L2 cache line is invalidated.
 *
 *----------------------------------------------------------------------------
 */
ALEAF(_mips5k_HitInvalidateSCache)
#ifdef CPUR4600
	mfc0	v1, COP_0_STATUS_REG		# Save the status register.
	li	v0, SR_DIAG_DE
	mtc0	v0, COP_0_STATUS_REG		# Disable interrupts
#endif

	beqz	a2, 3f				# size is zero!
	PTR_ADDU a2, 31				# Round up
	PTR_ADDU a2, a1				# Add in extra from align
	dsrl	a1, 5
	dsll	a1, 5				# align address
	PTR_SUBU a2, a1
	dsrl	a2, 5				# Compute number of cache lines
1:
	PTR_ADDU a2, -1
	cache	HitInvalidate_S, 0(a1)
	cache	HitInvalidate_D, 0(a1)		# Orphans in L1
	bnez	a2, 1b
	PTR_ADDU a1, 32

3:
#ifdef CPUR4600
	mtc0	v1, COP_0_STATUS_REG	# Restore the status register.
	NOP10
#endif
	sync
	j	ra
	nop

/*----------------------------------------------------------------------------
 *
 * Mips5k_IOSyncDCache(struct cpu_info *ci, vaddr_t va, size_t len, int how)
 *
 *	Invalidate or flush data cache for range of va to va + len - 1.
 *
 *	In case of the existence of an external cache we invalidate pages
 *	which are in the given range ONLY if transfer direction is READ.
 *	The assumption here is a 'write through' external cache which is
 *	true for all now supported processors.
 *
 * Side effects:
 *	If how == 0 (read), L1 and on-chip L2 caches are invalidated or
 *		flushed if the area does not match the alignment requirements.
 *		Writethrough L2 and L3 cache are invalidated for the address
 *		range.
 *	If how == 1 (write), L1 and on-chip L2 caches are written back to
 *		memory and invalidated. Writethrough L2 and L3 caches are
 *		left alone.
 *	If how == 2 (write-read), L1 and on-chip L2 caches are written back
 *		to memory and invalidated. Writethrough L2 and L3 caches are
 *		invalidated.
 *
 *----------------------------------------------------------------------------
 */
NON_LEAF(Mips5k_IOSyncDCache, FRAMESZ(CF_SZ+2*REGSZ), ra)
	PTR_SUBU sp, FRAMESZ(CF_SZ+2*REGSZ)
	PTR_S	ra, CF_RA_OFFS+2*REGSZ(sp)
	REG_S	a1, CF_ARGSZ(sp)		# save args
	REG_S	a2, CF_ARGSZ+REGSZ(sp)
	beqz	a3, SyncRD			# Sync PREREAD
	lw	t0, CI_CACHECONFIGURATION(a0)

	addiu	a3, -1
	bnez	a3, SyncRDWB			# Sync PREWRITE+PREREAD
	nop

	and	t0, CTYPE_HAS_IL2		# Sync PREWRITE
	bnez	t0, SyncSC			# Have internal L2?
	PTR_ADDU sp, FRAMESZ(CF_SZ+2*REGSZ)
	j	Mips5k_HitSyncDCache		# No flush L1.
	nop
SyncSC:
	j	_mips5k_HitSyncSCache		# Do internal L2 cache
	nop					# L1 done in parallel

SyncRD:
	or	t1, a1, a2			# check if invalidate possible
	and	t1, 31				# both address and size must
	bnez	t1, SyncRDWB			# be aligned to the cache size
	nop

/*
 *  Sync for aligned read, no writeback required.
 */
	and	t0, CTYPE_HAS_IL2		# Have internal L2?
	bnez	t0, SyncRDL2
	nop

	jal	Mips5k_HitInvalidateDCache	# External L2 or no L2. Do L1.
	nop

	b	SyncRDXL2
	PTR_L	ra, CF_RA_OFFS+2*REGSZ(sp)	# External L2 if present

SyncRDL2:
	jal	_mips5k_HitInvalidateSCache	# Internal L2 cache
	nop					# L1 done in parallel

	b	SyncRDL3
	PTR_L	ra, CF_RA_OFFS+2*REGSZ(sp)	# L3 invalidate if present

/*
 *  Sync for unaligned read or write-read.
 */
SyncRDWB:
	and	t0, CTYPE_HAS_IL2		# Have internal L2?
	bnez	t0, SyncRDWBL2			# Yes, do L2
	nop

	jal	Mips5k_HitSyncDCache
	nop

	b	SyncRDXL2
	PTR_L	ra, CF_RA_OFFS+2*REGSZ(sp)	# External L2 if present

SyncRDWBL2:
	jal	_mips5k_HitSyncSCache		# Internal L2 cache
	nop					# L1 done in parallel

	b	SyncRDL3
	PTR_L	ra, CF_RA_OFFS+2*REGSZ(sp)	# L3 invalidate if present

SyncRDXL2:
	lw	t0, CI_CACHECONFIGURATION(a0)
	and	t0, CTYPE_HAS_XL2		# Have external L2?
	beqz	t0, SyncRDL3			# Nope.
	REG_L	a1, CF_ARGSZ(sp)
	REG_L	a2, CF_ARGSZ+REGSZ(sp)
	and	a3, a1, 4095			# align on external page size
	mtc0	zero, COP_0_TAG_LO
	PTR_SUBU a1, a3
	PTR_ADDU a2, a3
1:
	blez	a2, SyncDone
	PTR_SUBU a2, 4096			# Fixed external cache page size

	cache	InvalidatePage_S, 0(a1)
	b	1b
	PTR_ADDU a1, 4096

SyncRDL3:
	lw	t0, CI_CACHECONFIGURATION(a0)
	and	t0, CTYPE_HAS_XL3		# Have L3?
	beqz	t0, SyncDone			# Nope.
	REG_L	a1, CF_ARGSZ(sp)
	REG_L	a2, CF_ARGSZ+REGSZ(sp)
	and	a3, a1, 4095			# align on external page size
	mtc0	zero, COP_0_TAG_LO
	PTR_SUBU a1, a3
	PTR_ADDU a2, a3
1:
	blez	a2, SyncDone
	PTR_SUBU a2, 4096			# Fixed external cache page size

	cache	InvalidatePage_T, 0(a1)
	b	1b
	PTR_ADDU a1, 4096

SyncDone:
	sync
	j	ra
	PTR_ADDU sp, FRAMESZ(CF_SZ+2*REGSZ)
END(Mips5k_IOSyncDCache)
