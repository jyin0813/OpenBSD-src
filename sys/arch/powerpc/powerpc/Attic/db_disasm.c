/*	$NetBSD: db_disasm.c,v 1.8 2001/06/12 05:31:44 simonb Exp $	*/
/*	$OpenBSD: db_disasm.c,v 1.7 2001/08/21 02:49:14 drahn Exp $	*/
/*
 * Copyright (c) 1996 Dale Rahn. All rights reserved.
 *
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
 *	This product includes software developed by Dale Rahn.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

#include <sys/param.h>
#include <sys/proc.h>
#include <sys/systm.h>

#include <machine/db_machdep.h>

#include <ddb/db_access.h>
#include <ddb/db_sym.h>
#include <ddb/db_variables.h>
#include <ddb/db_interface.h>
#include <ddb/db_output.h>

enum function_mask {
	Op_A    =	0x00000001,
	Op_B    =	0x00000002,
	Op_BI   =	0x00000004,
	Op_BO   =	0x00000008,
	Op_CRM  =	0x00000010,
	Op_D    =	0x00000020, /* yes, Op_S and Op_D are the same */
	Op_S    =	0x00000020,
	Op_FM   =	0x00000040,
	Op_IMM  =	0x00000080,
	Op_LK   =	0x00000100,
	Op_Rc   =	0x00000200,
	Op_AA	=	Op_LK | Op_Rc, /* kludge (reduce Op_s) */
	Op_LKM	=	Op_AA,
	Op_RcM	=	Op_AA,
	Op_OE   =	0x00000400,
	Op_SR   =	0x00000800,
	Op_TO   =	0x00001000,
	Op_sign =	0x00002000,
	Op_const =	0x00004000,
	Op_SIMM =	Op_const | Op_sign,
	Op_UIMM =	Op_const,
	Op_d	=	Op_const | Op_sign,
	Op_crbA =	0x00008000,
	Op_crbB =	0x00010000,
	Op_crbD =	0x00020000,
	Op_crfD =	0x00040000,
	Op_crfS =	0x00080000,
	Op_ds   =	0x00100000,
	Op_me   =	0x00200000,
	Op_spr  =	0x00400000,
	Op_tbr  =	0x00800000,

	Op_L	=	0x01000000,
	Op_BD	=	0x02000000,
	Op_LI	=	0x04000000,
	Op_C	=	0x08000000,

	Op_NB	=	0x10000000,

	Op_sh_mb_sh =	0x20000000,
	Op_sh   =	0x40000000,
	Op_SH	=	Op_sh | Op_sh_mb_sh,
	Op_mb	=	0x80000000,
	Op_MB	=	Op_mb | Op_sh_mb_sh,
	Op_ME	=	Op_MB,

};

struct opcode {
	char *name;
	u_int32_t mask;
	u_int32_t code;
	enum function_mask func;
};

typedef u_int32_t instr_t;
typedef void (op_class_func) (instr_t);

u_int32_t extract_field(u_int32_t value, u_int32_t base, u_int32_t width);
void disasm_fields(const struct opcode *popcode, instr_t instr,
    char *disasm_str);
void dis_ppc(const struct opcode *opcodeset, instr_t instr);

op_class_func op_ill, op_base;
op_class_func op_cl_x13, op_cl_x1e, op_cl_x1f;
op_class_func op_cl_x3a, op_cl_x3b;
op_class_func op_cl_x3e, op_cl_x3f;

op_class_func *opcodes_base[] = {
/*x00*/	op_ill,		op_ill,		op_base,	op_ill,
/*x04*/	op_ill,		op_ill,		op_ill,		op_base,
/*x08*/	op_base,	op_base,	op_ill,		op_base,	
/*x0C*/ op_base,	op_base,	op_base/*XXX*/,	op_base/*XXX*/,
/*x10*/ op_base,	op_base,	op_base,	op_cl_x13,
/*x14*/	op_base,	op_base,	op_ill,		op_base,
/*x18*/	op_base,	op_base,	op_base,	op_base,
/*x1C*/ op_base,	op_base,	op_cl_x1e,	op_cl_x1f,
/*x20*/	op_base,	op_base,	op_base,	op_base,
/*x24*/	op_base,	op_base,	op_base,	op_base,
/*x28*/	op_base,	op_base,	op_base,	op_base,
/*x2C*/	op_base,	op_base,	op_base,	op_base,
/*x30*/	op_base,	op_base,	op_base,	op_base,
/*x34*/	op_base,	op_base,	op_base,	op_base,
/*x38*/ op_ill,		op_ill,		op_cl_x3a,	op_cl_x3b,
/*x3C*/	op_ill,		op_ill,		op_cl_x3e,	op_cl_x3f
};


/* This table could be modified to make significant the "reserved" fields
 * of the opcodes, But I didn't feel like it when typing in the table,
 * I would recommend that this table be looked over for errors, 
 * This was derived from the table in Appendix A.2 of (Mot part # MPCFPE/AD)
 * PowerPC Microprocessor Family: The Programming Environments
 */
	
const struct opcode opcodes[] = {
	{ "tdi",	0xfc000000, 0x08000000, Op_TO | Op_A | Op_SIMM },
	{ "twi",	0xfc000000, 0x0c000000, Op_TO | Op_A | Op_SIMM },
	{ "mulli",	0xfc000000, 0x1c000000, Op_D | Op_A | Op_SIMM },
	{ "subfic",	0xfc000000, 0x20000000, Op_D | Op_A | Op_SIMM },
	{ "cmpli",	0xfc000000, 0x28000000, Op_crfD | Op_L | Op_A | Op_SIMM },
	{ "cmpi",	0xfc000000, 0x2c000000, Op_crfD | Op_L | Op_A | Op_SIMM },
	{ "addic",	0xfc000000, 0x30000000, Op_D | Op_A | Op_SIMM },
	{ "addic.",	0xfc000000, 0x34000000, Op_D | Op_A | Op_SIMM },
	{ "addi",	0xfc000000, 0x38000000, Op_D | Op_A | Op_SIMM },
	{ "addis",	0xfc000000, 0x3c000000, Op_D | Op_A | Op_SIMM },
	{ "bc",		0xfc000000, 0x40000000, Op_BO | Op_BI | Op_BD | Op_AA | Op_LK },
	{ "sc",		0xffffffff, 0x44000002, Op_BO | Op_BI | Op_BD | Op_AA | Op_LK },
	{ "b",		0xfc000000, 0x48000000, Op_LI | Op_AA | Op_LK },

	{ "rlwimi",	0xfc000000, 0x50000000, Op_S | Op_A | Op_SH | Op_MB | Op_ME | Op_Rc },
	{ "rlwinm",	0xfc000000, 0x54000000, Op_S | Op_A | Op_SH | Op_MB | Op_ME | Op_Rc },
	{ "rlwnm",	0xfc000000, 0x5c000000, Op_S | Op_A | Op_SH | Op_MB | Op_ME | Op_Rc },

	{ "ori",	0xfc000000, 0x60000000, Op_S | Op_A | Op_UIMM },
	{ "oris",	0xfc000000, 0x64000000, Op_S | Op_A | Op_UIMM },
	{ "xori",	0xfc000000, 0x68000000, Op_S | Op_A | Op_UIMM },
	{ "xoris",	0xfc000000, 0x6c000000, Op_S | Op_A | Op_UIMM },

	{ "andi.",	0xfc000000, 0x70000000, Op_S | Op_A | Op_UIMM },
	{ "andis.",	0xfc000000, 0x74000000, Op_S | Op_A | Op_UIMM },

	{ "lwz",	0xfc000000, 0x80000000, Op_D | Op_A | Op_d },
	{ "lwzu",	0xfc000000, 0x84000000, Op_D | Op_A | Op_d },
	{ "lbz",	0xfc000000, 0x88000000, Op_D | Op_A | Op_d },
	{ "lbzu",	0xfc000000, 0x8c000000, Op_D | Op_A | Op_d },
	{ "stw",	0xfc000000, 0x90000000, Op_S | Op_A | Op_d },
	{ "stwu",	0xfc000000, 0x94000000, Op_S | Op_A | Op_d },
	{ "stb",	0xfc000000, 0x98000000, Op_S | Op_A | Op_d },
	{ "stbu",	0xfc000000, 0x9c000000, Op_S | Op_A | Op_d },

	{ "lhz",	0xfc000000, 0xa0000000, Op_D | Op_A | Op_d },
	{ "lhzu",	0xfc000000, 0xa4000000, Op_D | Op_A | Op_d },
	{ "lha",	0xfc000000, 0xa8000000, Op_D | Op_A | Op_d },
	{ "lhau",	0xfc000000, 0xac000000, Op_D | Op_A | Op_d },
	{ "sth",	0xfc000000, 0xb0000000, Op_S | Op_A | Op_d },
	{ "sthu",	0xfc000000, 0xb4000000, Op_S | Op_A | Op_d },
	{ "lmw",	0xfc000000, 0xb8000000, Op_D | Op_A | Op_d },
	{ "stmw",	0xfc000000, 0xbc000000, Op_S | Op_A | Op_d },

	{ "lfs",	0xfc000000, 0xc0000000, Op_D | Op_A | Op_d },
	{ "lfsu",	0xfc000000, 0xc4000000, Op_D | Op_A | Op_d },
	{ "lfd",	0xfc000000, 0xc8000000, Op_D | Op_A | Op_d },
	{ "lfdu",	0xfc000000, 0xcc000000, Op_D | Op_A | Op_d },

	{ "stfs",	0xfc000000, 0xd0000000, Op_S | Op_A | Op_d },
	{ "stfsu",	0xfc000000, 0xd4000000, Op_S | Op_A | Op_d },
	{ "stfd",	0xfc000000, 0xd8000000, Op_S | Op_A | Op_d },
	{ "stfdu",	0xfc000000, 0xdc000000, Op_S | Op_A | Op_d },
	{ "",		0x0,		0x0, 0 }

};
/* 13 * 4 = 4c */
const struct opcode opcodes_13[] = {
/* 0x13 << 2 */
	{ "mcrf",	0xfc0007fe, 0x4c000000, Op_crfD | Op_crfS },
	{ "bclr",	0xfc0007fe, 0x4c000020, Op_BO | Op_BI | Op_LK },
	{ "crnor",	0xfc0007fe, 0x4c000042, Op_crbD | Op_crbA | Op_crbB },
	{ "rfi",	0xfc0007fe, 0x4c000064, 0 },
	{ "crandc",	0xfc0007fe, 0x4c000102, Op_BO | Op_BI | Op_LK },
	{ "isync",	0xfc0007fe, 0x4c00012c, 0 },
	{ "crxor",	0xfc0007fe, 0x4c000182, Op_crbD | Op_crbA | Op_crbB },
	{ "crnand",	0xfc0007fe, 0x4c0001c2, Op_crbD | Op_crbA | Op_crbB },
	{ "crand",	0xfc0007fe, 0x4c000202, Op_crbD | Op_crbA | Op_crbB },
	{ "creqv",	0xfc0007fe, 0x4c000242, Op_crbD | Op_crbA | Op_crbB },
	{ "crorc",	0xfc0007fe, 0x4c000342, Op_crbD | Op_crbA | Op_crbB },
	{ "cror",	0xfc0007fe, 0x4c000382, Op_crbD | Op_crbA | Op_crbB },
	{ "bcctr",	0xfc0007fe, 0x4c000420, Op_BO | Op_BI | Op_LK },
	{ "",		0x0,		0x0, 0 }
};

/* 1e * 4 = 78 */
const struct opcode opcodes_1e[] = {
	{ "rldicl",	0xfc00001c, 0x78000000, Op_S | Op_A | Op_sh | Op_mb | Op_Rc },
	{ "rldicr",	0xfc00001c, 0x78000004, Op_S | Op_A | Op_sh | Op_me | Op_Rc },
	{ "rldic",	0xfc00001c, 0x78000008, Op_S | Op_A | Op_sh | Op_mb | Op_Rc },
	{ "rldimi",	0xfc00001c, 0x7800000c, Op_S | Op_A | Op_sh | Op_mb | Op_Rc },
	{ "rldcl",	0xfc00003e, 0x78000010, Op_S | Op_A | Op_B | Op_mb | Op_Rc },
	{ "rldcr",	0xfc00003e, 0x78000012, Op_S | Op_A | Op_B | Op_me | Op_Rc },
	{ "",		0x0,		0x0, 0 }
};

/* 1f * 4 = 7c */
const struct opcode opcodes_1f[] = {
/* 1f << 2 */
	{ "cmp",	0xfc0007fe, 0x7c000000, Op_S | Op_A | Op_B | Op_me | Op_Rc },
	{ "tw",		0xfc0007fe, 0x7c000008, Op_TO | Op_A | Op_B },
	{ "subfc",	0xfc0003fe, 0x7c000010, Op_D | Op_A | Op_B | Op_OE | Op_Rc },
	{ "mulhdu",	0xfc0007fe, 0x7c000012, Op_D | Op_A | Op_B | Op_Rc },
	{ "addc",	0xfc0003fe, 0x7c000014, Op_D | Op_A | Op_B | Op_OE | Op_Rc },
	{ "mulhwu",	0xfc0007fe, 0x7c000016, Op_D | Op_A | Op_B | Op_Rc },

	{ "mfcr",	0xfc0007fe, 0x7c000026, Op_D },
	{ "lwarx",	0xfc0007fe, 0x7c000028, Op_D | Op_A | Op_B },
	{ "ldx",	0xfc0007fe, 0x7c00002a, Op_D | Op_A | Op_B },
	{ "lwzx",	0xfc0007fe, 0x7c00002e, Op_D | Op_A | Op_B },
	{ "slw",	0xfc0007fe, 0x7c000030, Op_D | Op_A | Op_B | Op_Rc },
	{ "cntlzw",	0xfc0007fe, 0x7c000034, Op_D | Op_A | Op_Rc },
	{ "sld",	0xfc0007fe, 0x7c000036, Op_D | Op_A | Op_B | Op_Rc },
	{ "and",	0xfc0007fe, 0x7c000038, Op_D | Op_A | Op_B | Op_Rc },
	{ "cmpl",	0xfc0007fe, 0x7c000040, Op_crfD | Op_L | Op_A | Op_B },
	{ "subf",	0xfc0003fe, 0x7c000050, Op_D | Op_A | Op_B | Op_OE | Op_Rc },
	{ "ldux",	0xfc0007fe, 0x7c00006a, Op_D | Op_A | Op_B },
	{ "dcbst",	0xfc0007fe, 0x7c00006c, Op_A | Op_B },
	{ "lwzux",	0xfc0007fe, 0x7c00006e, Op_D | Op_A | Op_B },
	{ "cntlzd",	0xfc0007fe, 0x7c000074, Op_S | Op_A | Op_Rc },
	{ "andc",	0xfc0007fe, 0x7c000078, Op_S | Op_A | Op_B | Op_Rc },
	{ "td",		0xfc0007fe, 0x7c000088, Op_TO | Op_A | Op_B },
	{ "mulhd",	0xfc0007fe, 0x7c000092, Op_D | Op_A | Op_B | Op_Rc },
	{ "mulhw",	0xfc0007fe, 0x7c000096, Op_D | Op_A | Op_B | Op_Rc },
	{ "mfmsr",	0xfc0007fe, 0x7c0000a6, Op_D },
	{ "ldarx",	0xfc0007fe, 0x7c0000a8, Op_D | Op_A | Op_B },
	{ "dcbf",	0xfc0007fe, 0x7c0000ac, Op_A | Op_B },
	{ "lbzx",	0xfc0007fe, 0x7c0000ae, Op_D | Op_A | Op_B },
	{ "neg",	0xfc0003fe, 0x7c0000d0, Op_D | Op_A | Op_OE | Op_Rc },
	{ "lbzux",	0xfc0007fe, 0x7c0000ee, Op_D | Op_A | Op_B },
	{ "nor",	0xfc0007fe, 0x7c0000f8, Op_S | Op_A | Op_B | Op_Rc },
	{ "subfe",	0xfc0003fe, 0x7c000110, Op_D | Op_A | Op_B | Op_OE | Op_Rc },
	{ "adde",	0xfc0003fe, 0x7c000114, Op_D | Op_A | Op_B | Op_OE | Op_Rc },
	{ "mtcrf",	0xfc0007fe, 0x7c000120, Op_S | Op_CRM },
	{ "mtmsr",	0xfc0007fe, 0x7c000124, Op_S },
	{ "stdx",	0xfc0007fe, 0x7c00012a, Op_S | Op_A | Op_B },
	{ "stwcx.",	0xfc0007ff, 0x7c00012d, Op_S | Op_A | Op_B },
	{ "stwx",	0xfc0007fe, 0x7c00012e, Op_S | Op_A | Op_B },
	{ "stdux",	0xfc0007fe, 0x7c00016a, Op_S | Op_A | Op_B },
	{ "stwux",	0xfc0007fe, 0x7c00016e, Op_S | Op_A | Op_B },
	{ "subfze",	0xfc0003fe, 0x7c000190, Op_D | Op_A | Op_OE | Op_Rc },
	{ "addze",	0xfc0003fe, 0x7c000194, Op_D | Op_A | Op_OE | Op_Rc },
	{ "mtsr",	0xfc0007fe, 0x7c0001a4, Op_S | Op_SR },
	{ "stdcx.",	0xfc0007ff, 0x7c0001ad, Op_S | Op_A | Op_B },
	{ "stbx",	0xfc0007fe, 0x7c0001ae, Op_S | Op_A | Op_B },
	{ "subfme",	0xfc0003fe, 0x7c0001d0, Op_D | Op_A | Op_OE | Op_Rc },
	{ "mulld",	0xfc0003fe, 0x7c0001d2, Op_D | Op_A | Op_B | Op_OE | Op_Rc },
	{ "addme",	0xfc0003fe, 0x7c0001d4, Op_D | Op_A | Op_OE | Op_Rc },
	{ "mullw",	0xfc0003fe, 0x7c0001d6, Op_D | Op_A | Op_B | Op_OE | Op_Rc },
	{ "mtsrin",	0xfc0007fe, 0x7c0001e4, Op_S | Op_B },
	{ "dcbtst",	0xfc0007fe, 0x7c0001ec, Op_A | Op_B },
	{ "stbux",	0xfc0007fe, 0x7c0001ee, Op_S | Op_A | Op_B },
	{ "add",	0xfc0003fe, 0x7c000214, Op_D | Op_A | Op_B | Op_OE | Op_Rc },
	{ "dcbt",	0xfc0007fe, 0x7c00022c, Op_A | Op_B },
	{ "lhzx",	0xfc0007ff, 0x7c00022e, Op_D | Op_A | Op_B },
	{ "eqv",	0xfc0007fe, 0x7c000238, Op_S | Op_A | Op_B | Op_Rc },
	{ "tlbie",	0xfc0007fe, 0x7c000264, Op_B },
	{ "eciwx",	0xfc0007fe, 0x7c00026c, Op_D | Op_A | Op_B },
	{ "lhzux",	0xfc0007fe, 0x7c00026e, Op_D | Op_A | Op_B },
	{ "xor",	0xfc0007fe, 0x7c000278, Op_S | Op_A | Op_B | Op_Rc },
	{ "mfspr",	0xfc0007fe, 0x7c0002a6, Op_D | Op_spr },
	{ "lwax",	0xfc0007fe, 0x7c0002aa, Op_D | Op_A | Op_B },
	{ "lhax",	0xfc0007fe, 0x7c0002ae, Op_D | Op_A | Op_B },
	{ "tlbia",	0xfc0007fe, 0x7c0002e4, 0 },
	{ "mftb",	0xfc0007fe, 0x7c0002e6, Op_D | Op_tbr },
	{ "lwaux",	0xfc0007fe, 0x7c0002ea, Op_D | Op_A | Op_B },
	{ "lhaux",	0xfc0007fe, 0x7c0002ee, Op_D | Op_A | Op_B },
	{ "sthx",	0xfc0007fe, 0x7c00032e, Op_S | Op_A | Op_B },
	{ "orc",	0xfc0007fe, 0x7c000338, Op_S | Op_A | Op_B | Op_Rc },
	{ "ecowx",	0xfc0007fe, 0x7c00036c, Op_S | Op_A | Op_B | Op_Rc },
	{ "slbie",	0xfc0007fc, 0x7c000364, Op_B },
	{ "sthux",	0xfc0007fe, 0x7c00036e, Op_S | Op_A | Op_B },
	{ "or",		0xfc0007fe, 0x7c000378, Op_S | Op_A | Op_B | Op_Rc },
	{ "divdu",	0xfc0003fe, 0x7c000392, Op_D | Op_A | Op_B | Op_OE | Op_Rc },
	{ "divwu",	0xfc0003fe, 0x7c000396, Op_D | Op_A | Op_B | Op_OE | Op_Rc },
	{ "mtspr",	0xfc0007fe, 0x7c0003a6, Op_S | Op_spr },
	{ "dcbi",	0xfc0007fe, 0x7c0003ac, Op_A | Op_B },
	{ "nand",	0xfc0007fe, 0x7c0003b8, Op_S | Op_A | Op_B | Op_Rc },
	{ "divd",	0xfc0003fe, 0x7c0003d2, Op_S | Op_A | Op_B | Op_OE | Op_Rc },
	{ "divw",	0xfc0003fe, 0x7c0003d6, Op_S | Op_A | Op_B | Op_OE | Op_Rc },
	{ "slbia",	0xfc0003fe, 0x7c0003e4, Op_S | Op_A | Op_B | Op_OE | Op_Rc },
	{ "mcrxr",	0xfc0007fe, 0x7c000400, Op_crfD },
	{ "lswx",	0xfc0007fe, 0x7c00042a, Op_D | Op_A | Op_B },
	{ "lwbrx",	0xfc0007fe, 0x7c00042c, Op_D | Op_A | Op_B },
	{ "lfsx",	0xfc0007fe, 0x7c00042e, Op_D | Op_A | Op_B },
	{ "srw",	0xfc0007fe, 0x7c000430, Op_S | Op_A | Op_B | Op_Rc },
	{ "srd",	0xfc0007fe, 0x7c000436, Op_S | Op_A | Op_B | Op_Rc },
	{ "tlbsync",	0xfc0007fe, 0x7c00046c, 0 },
	{ "lfsux",	0xfc0007fe, 0x7c00046e, Op_D | Op_A | Op_B },
	{ "mfsr",	0xfc0007fe, 0x7c0004a6, Op_D | Op_SR },
	{ "lswi",	0xfc0007fe, 0x7c0004aa, Op_D | Op_A | Op_NB },
	{ "sync",	0xfc0007fe, 0x7c0004ac, 0 },
	{ "lfdx",	0xfc0007fe, 0x7c0004ae, Op_D | Op_A | Op_B },
	{ "lfdux",	0xfc0007fe, 0x7c0004ee, Op_D | Op_A | Op_B },
	{ "mfsrin",	0xfc0007fe, 0x7c000526, Op_D | Op_B },
	{ "stswx",	0xfc0007fe, 0x7c00052a, Op_S | Op_A | Op_B },
	{ "stwbrx",	0xfc0007fe, 0x7c00052c, Op_S | Op_A | Op_B },
	{ "stfsx",	0xfc0007fe, 0x7c00052e, Op_S | Op_A | Op_B },
	{ "stfsux",	0xfc0007fe, 0x7c00056e, Op_S | Op_A | Op_B },
	{ "stswi",	0xfc0007fe, 0x7c0005aa, Op_S | Op_A | Op_NB },
	{ "stfdx",	0xfc0007fe, 0x7c0005ae, Op_S | Op_A | Op_B },
	{ "stfdux",	0xfc0007fe, 0x7c0005ee, Op_S | Op_A | Op_B },
	{ "lhbrx",	0xfc0007fe, 0x7c00062c, Op_D | Op_A | Op_B },
	{ "sraw",	0xfc0007fe, 0x7c000630, Op_S | Op_A | Op_B },
	{ "srad",	0xfc0007fe, 0x7c000634, Op_S | Op_A | Op_B | Op_Rc},
	{ "srawi",	0xfc0007fe, 0x7c000670, Op_S | Op_A | Op_B | Op_Rc},
	{ "sradi",	0xfc0007fc, 0x7c000674, Op_S | Op_A | Op_sh },
	{ "eieio",	0xfc0007fe, 0x7c0006ac, 0 },
	{ "sthbrx",	0xfc0007fe, 0x7c00072c, Op_S | Op_A | Op_B },
	{ "extsh",	0xfc0007fe, 0x7c000734, Op_S | Op_A | Op_B | Op_Rc },
	{ "extsb",	0xfc0007fe, 0x7c000774, Op_S | Op_A | Op_Rc },
	{ "icbi",	0xfc0007fe, 0x7c0007ac, Op_A | Op_B },

	{ "stfiwx",	0xfc0007fe, 0x7c0007ae, Op_S | Op_A | Op_B },
	{ "extsw",	0xfc0007fe, 0x7c0007b4, Op_S | Op_A | Op_Rc },
	{ "dcbz",	0xfc0007fe, 0x7c0007ec, Op_A | Op_B },
	{ "",		0x0,		0x0, 0 }
};

/* 3a * 4 = e8 */
const struct opcode opcodes_3a[] = {
	{ "ld",		0xfc000003, 0xe8000000, Op_D | Op_A | Op_ds },
	{ "ldu",	0xfc000003, 0xe8000001, Op_D | Op_A | Op_ds },
	{ "lwa",	0xfc000003, 0xe8000002, Op_D | Op_A | Op_ds },
	{ "",		0x0,		0x0, 0 }
};
/* 3b * 4 = ec */
const struct opcode opcodes_3b[] = {
	{ "fdivs",	0xfc00003e, 0xec000024, Op_D | Op_A | Op_B | Op_Rc },
	{ "fsubs",	0xfc00003e, 0xec000028, Op_D | Op_A | Op_B | Op_Rc },

	{ "fadds",	0xfc00003e, 0xec00002a, Op_D | Op_A | Op_B | Op_Rc },
	{ "fsqrts",	0xfc00003e, 0xec00002c, Op_D | Op_B | Op_Rc },
	{ "fres",	0xfc00003e, 0xec000030, Op_D | Op_B | Op_Rc },
	{ "fmuls",	0xfc00003e, 0xec000032, Op_D | Op_A | Op_C | Op_Rc },
	{ "fmsubs",	0xfc00003e, 0xec000038, Op_D | Op_A | Op_B | Op_C | Op_Rc },
	{ "fmadds",	0xfc00003e, 0xec00003a, Op_D | Op_A | Op_B | Op_C | Op_Rc },
	{ "fnmsubs",	0xfc00003e, 0xec00003c, Op_D | Op_A | Op_B | Op_C | Op_Rc },
	{ "fnmadds",	0xfc00003e, 0xec00003e, Op_D | Op_A | Op_B | Op_C | Op_Rc },
	{ "",		0x0,		0x0, 0 }
};
/* 3e * 4 = f8 */
const struct opcode opcodes_3e[] = {
	{ "std",	0xfc000003, 0xf8000000, Op_S | Op_A | Op_ds },
	{ "stdu",	0xfc000003, 0xf8000001, Op_S | Op_A | Op_ds },
	{ "",		0x0,		0x0, 0 }
};

/* 3f * 4 = fc */
const struct opcode opcodes_3f[] = {
	{ "fcmpu",	0xfc0007fe, 0xfc000000, Op_crfD | Op_A | Op_B },
	{ "frsp",	0xfc0007fe, 0xfc000018, Op_D | Op_B | Op_Rc },
	{ "fctiw",	0xfc0007fe, 0xfc00001c, Op_D | Op_B | Op_Rc },
	{ "fctiwz",	0xfc0007fe, 0xfc00001e, Op_D | Op_B | Op_Rc },

	{ "fdiv",	0xfc00003e, 0xfc000024, Op_D | Op_A | Op_B | Op_Rc },
	{ "fsub",	0xfc00003e, 0xfc000028, Op_D | Op_A | Op_B | Op_Rc },
	{ "fadd",	0xfc00003e, 0xfc00002a, Op_D | Op_A | Op_B | Op_Rc },
	{ "fsqrt",	0xfc00003e, 0xfc00002c, Op_D | Op_B | Op_Rc },
	{ "fsel",	0xfc00003e, 0xfc00002e, Op_D | Op_A | Op_B | Op_C | Op_Rc },
	{ "fmul",	0xfc00003e, 0xfc000032, Op_D | Op_A | Op_C | Op_Rc },
	{ "frsqrte",	0xfc00003e, 0xfc000034, Op_D | Op_B | Op_Rc },
	{ "fmsub",	0xfc00003e, 0xfc000038, Op_D | Op_A | Op_B | Op_C | Op_Rc },
	{ "fmadd",	0xfc00003e, 0xfc00003a, Op_D | Op_A | Op_B | Op_C | Op_Rc },
	{ "fnmsub",	0xfc00003e, 0xfc00003c, Op_D | Op_A | Op_B | Op_C | Op_Rc },
	{ "fnmadd",	0xfc00003e, 0xfc00003e, Op_D | Op_A | Op_B | Op_C | Op_Rc },

	{ "fcmpo",	0xfc0007fe, 0xfc000040, Op_crfD | Op_A | Op_B },
	{ "mtfsb1",	0xfc0007fe, 0xfc00004c, Op_crfD | Op_Rc },
	{ "fneg",	0xfc0007fe, 0xfc000050, Op_D | Op_B | Op_Rc },
	{ "mcrfs",	0xfc0007fe, 0xfc000080, Op_D | Op_B | Op_Rc },
	{ "mtfsb0",	0xfc0007fe, 0xfc00008c, Op_crfD | Op_Rc },
	{ "fmr",	0xfc0007fe, 0xfc000090, Op_D | Op_B | Op_Rc },
	{ "mtfsfi",	0xfc0007fe, 0xfc00010c, Op_crfD | Op_IMM | Op_Rc },

	{ "fnabs",	0xfc0007fe, 0xfc000110, Op_D | Op_B | Op_Rc },
	{ "fabs",	0xfc0007fe, 0xfc000210, Op_D | Op_B | Op_Rc },
	{ "mffs",	0xfc0007fe, 0xfc00048e, Op_D | Op_B | Op_Rc },
	{ "mtfsf",	0xfc0007fe, 0xfc00058e, Op_FM | Op_B | Op_Rc },
	{ "fctid",	0xfc0007fe, 0xfc00065c, Op_D | Op_B | Op_Rc },
	{ "fctidz",	0xfc0007fe, 0xfc00065e, Op_D | Op_B | Op_Rc },
	{ "fcfid",	0xfc0007fe, 0xfc00069c, Op_D | Op_B | Op_Rc },
	{ "",		0x0,		0x0, 0 }
};

/*
typedef void (op_class_func) (instr_t);
*/
void
op_ill(instr_t instr)
{
	db_printf("illegal instruction %x\n", instr);
}

u_int32_t
extract_field(u_int32_t value, u_int32_t base, u_int32_t width)
{
	u_int32_t mask = (1 << width) - 1;
	return ((value >> base) & mask);
}

const struct opcode * search_op(const struct opcode *);

void
disasm_fields(const struct opcode *popcode, instr_t instr, char *disasm_str)
{
	char * pstr;
	enum function_mask func;

	pstr = disasm_str;

	func =  popcode->func;
	if (func & Op_OE) {
		u_int OE;
		/* also for Op_S (they are the same) */
		OE = extract_field(instr, 31 - 21, 1);
		if (OE) {
			pstr += sprintf (pstr, "o");
		}
		func &= ~Op_D;
	}
	switch  (func & Op_LKM) {
	case Op_Rc:
		if (instr & 0x1) {
			pstr += sprintf (pstr,". ");
		}
		break;
	case Op_AA:
		if (instr & 0x2) {
			pstr += sprintf (pstr,"a");
		}
	case Op_LK:
		if (instr & 0x1) {
			pstr += sprintf (pstr,"l ");
		}
		break;
	default:
		func &= ~Op_LKM;
	}
	pstr += sprintf (pstr, " ");
	if (func & Op_D) {
		u_int D;
		/* also for Op_S (they are the same) */
		D = extract_field(instr, 31 - 10, 5);
		pstr += sprintf (pstr, "r%d, ", D);
		func &= ~Op_D;
	}
	if (func & Op_crbD) {
		u_int crbD;
		crbD = extract_field(instr, 31 - 10, 5);
		pstr += sprintf (pstr, "crb%d, ", crbD);
		func &= ~Op_crbD;
	}
	if (func & Op_crfD) {
		u_int crfD;
		crfD = extract_field(instr, 31 - 8, 3);
		pstr += sprintf (pstr, "crf%d, ", crfD);
		func &= ~Op_crfD;
	}
	if (func & Op_L) {
		u_int L;
		L = extract_field(instr, 31 - 10, 1);
		if (L) {
			pstr += sprintf (pstr, "L, ");
		}
		func &= ~Op_L;
	}
	if (func & Op_FM) {
		u_int FM;
		FM = extract_field(instr, 31 - 10, 8);
		pstr += sprintf (pstr, "%d, ", FM);
		func &= ~Op_FM;
	}
	if (func & Op_TO) {
		u_int TO;
		TO = extract_field(instr, 31 - 10, 1);
		pstr += sprintf (pstr, "%d, ", TO);
		func &= ~Op_TO;
	}
	if (func & Op_crfS) {
		u_int crfS;
		crfS = extract_field(instr, 31 - 13, 3);
		pstr += sprintf (pstr, "%d, ", crfS);
		func &= ~Op_crfS;
	}
	if (func & Op_BO) {
		u_int BO;
		BO = extract_field(instr, 31 - 10, 5);
		pstr += sprintf (pstr ,"%d, ", BO);
		func &= ~Op_BO;
	}
	if (func & Op_A) {
		u_int A;
		A = extract_field(instr, 31 - 15, 5);
		pstr += sprintf (pstr, "r%d, ", A);
		func &= ~Op_A;
	}
	if (func & Op_B) {
		u_int B;
		B = extract_field(instr, 31 - 20, 5);
		pstr += sprintf (pstr, "r%d, ", B);
		func &= ~Op_B;
	}
	if (func & Op_C) {
		u_int C;
		C = extract_field(instr, 31 - 25, 5);
		pstr += sprintf (pstr, "r%d, ", C);
		func &= ~Op_C;
	}
	if (func & Op_BI) {
		u_int BI;
		BI = extract_field(instr, 31 - 10, 5);
		pstr += sprintf (pstr, "%d, ", BI);
		func &= ~Op_BI;
	}
	if (func & Op_crbA) {
		u_int crbA;
		crbA = extract_field(instr, 31 - 15, 5);
		pstr += sprintf (pstr, "%d, ", crbA);
		func &= ~Op_crbA;
	}
	if (func & Op_crbB) {
		u_int crbB;
		crbB = extract_field(instr, 31 - 20, 5);
		pstr += sprintf (pstr, "%d, ", crbB);
		func &= ~Op_crbB;
	}
	if (func & Op_CRM) {
		u_int CRM;
		CRM = extract_field(instr, 31 - 19, 8);
		pstr += sprintf (pstr, "0x%x, ", CRM);
		func &= ~Op_CRM;
	}
	if (func & Op_LI) {
		u_int LI;
		LI = extract_field(instr, 31 - 29, 24);
		pstr += sprintf (pstr, "0x%x, ", LI);
		func &= ~Op_LI;
	}
	switch (func & Op_SIMM) {
		u_int IMM;
	case Op_SIMM: /* same as Op_d */
		IMM = extract_field(instr, 31 - 31, 16);
		if (IMM & 0x8000) {
			pstr += sprintf (pstr, "-");
		}
			/* no break */
		func &= ~Op_SIMM;
	case Op_UIMM:
		IMM = extract_field(instr, 31 - 31, 16);
		pstr += sprintf (pstr, "0x%x, ", IMM);
		func &= ~Op_UIMM;
		break;
	default:
	}
	if (func & Op_BD ) {
		u_int BD;
		BD = extract_field(instr, 31 - 29, 14);
		pstr += sprintf (pstr, "0x%x, ", BD);
		func &= ~Op_BD;
	}
	if (func & Op_ds ) {
		u_int ds;
		ds = extract_field(instr, 31 - 29, 14) << 2;
		pstr += sprintf (pstr, "0x%x, ", ds);
		func &= ~Op_ds;
	}
	if (func & Op_spr ) {
		u_int spr;
		u_int sprl;
		u_int sprh;
		char *reg;
		sprl = extract_field(instr, 31 - 15, 5);
		sprh = extract_field(instr, 31 - 20, 5);
		spr = sprh << 5 | sprl;

		/* this table could be written better */
		switch (spr) {
		case 	1:
			reg = "xer";
			break;
		case 	8:
			reg = "lr";
			break;
		case 	9:
			reg = "ctr";
			break;
		case 	18:
			reg = "dsisr";
			break;
		case 	19:
			reg = "dar";
			break;
		case 	22:
			reg = "dec";
			break;
		case 	25:
			reg = "sdr1";
			break;
		case 	26:
			reg = "srr0";
			break;
		case 	27:
			reg = "srr1";
			break;
		case 	272:
			reg = "SPRG0";
			break;
		case 	273:
			reg = "SPRG1";
			break;
		case 	274:
			reg = "SPRG3";
			break;
		case 	275:
			reg = "SPRG3";
			break;
		case 	280:
			reg = "asr";
			break;
		case 	282:
			reg = "aer";
			break;
		case 	287:
			reg = "pvr";
			break;
		case 	528:
			reg = "ibat0u";
			break;
		case 	529:
			reg = "ibat0l";
			break;
		case 	530:
			reg = "ibat1u";
			break;
		case 	531:
			reg = "ibat1l";
			break;
		case 	532:
			reg = "ibat2u";
			break;
		case 	533:
			reg = "ibat2l";
			break;
		case 	534:
			reg = "ibat3u";
			break;
		case 	535:
			reg = "ibat3l";
			break;
		case 	536:
			reg = "dbat0u";
			break;
		case 	537:
			reg = "dbat0l";
			break;
		case 	538:
			reg = "dbat1u";
			break;
		case 	539:
			reg = "dbat1l";
			break;
		case 	540:
			reg = "dbat2u";
			break;
		case 	541:
			reg = "dbat2l";
			break;
		case 	542:
			reg = "dbat3u";
			break;
		case 	543:
			reg = "dbat3l";
			break;
		case 	1013:
			reg = "dabr";
			break;
		default:
			reg = 0;
		}
		if (reg == 0) {
			pstr += sprintf (pstr, ", [unknown spr (%d)]", spr);
		} else {
			pstr += sprintf (pstr, ", %s", reg);
		}
		func &= ~Op_spr;
	}

	if (func & Op_me) {
		u_int me, mel, meh;
		mel = extract_field(instr, 31 - 25, 4);
		meh = extract_field(instr, 31 - 26, 1);
		me = meh << 4 | mel;
		pstr += sprintf (pstr, ", 0x%x", me);
		func &= ~Op_me;
	}
	if ((func & Op_MB ) && (func & Op_sh_mb_sh)) {
		u_int MB;
		u_int ME;
		MB = extract_field(instr, 31 - 20, 5);
		pstr += sprintf (pstr, ", %d", MB);
		ME = extract_field(instr, 31 - 25, 5);
		pstr += sprintf (pstr, ", %d", ME);
	}
	if ((func & Op_SH ) && (func & Op_sh_mb_sh)) {
		u_int SH;
		SH = extract_field(instr, 31 - 20, 5);
		pstr += sprintf (pstr, ", %d", SH);
	}
	if ((func & Op_sh ) && ! (func & Op_sh_mb_sh)) {
		u_int sh, shl, shh;
		shl = extract_field(instr, 31 - 19, 4);
		shh = extract_field(instr, 31 - 20, 1);
		sh = shh << 4 | shl;
		pstr += sprintf (pstr, ", %d", sh);
	}
	if ((func & Op_mb ) && ! (func & Op_sh_mb_sh)) {
		u_int mb, mbl, mbh;
		mbl = extract_field(instr, 31 - 25, 4);
		mbh = extract_field(instr, 31 - 26, 1);
		mb = mbh << 4 | mbl;
		pstr += sprintf (pstr, ", %d", mb);
	}
	if ((func & Op_me ) && ! (func & Op_sh_mb_sh)) {
		u_int me, mel, meh;
		mel = extract_field(instr, 31 - 25, 4);
		meh = extract_field(instr, 31 - 26, 1);
		me = meh << 4 | mel;
		pstr += sprintf (pstr, ", %d", me);
	}
	if (func & Op_tbr ) {
		u_int tbr;
		u_int tbrl;
		u_int tbrh;
		char *reg;
		tbrl = extract_field(instr, 31 - 15, 5);
		tbrh = extract_field(instr, 31 - 20, 5);
		tbr = tbrh << 5 | tbrl;

		switch (tbr) {
		case 268:
			reg = "tbl";
			break;
		case 269:
			reg = "tbu";
			break;
		default:
			reg = 0;
		}
		if (reg == 0) {
			pstr += sprintf (pstr, ", [unknown tbr %d ]", tbr);
		} else {
			pstr += sprintf (pstr, ", %s", reg);
		}
		func &= ~Op_tbr;
	}
	if (func & Op_SR) {
		u_int SR;
		SR = extract_field(instr, 31 - 15, 3);
		pstr += sprintf (pstr, ", sr%d", SR);
		func &= ~Op_SR;
	}
	if (func & Op_NB) {
		u_int NB;
		NB = extract_field(instr, 31 - 20, 5);
		if (NB == 0 ) {
			NB=32;
		}
		pstr += sprintf (pstr, ", %d", NB);
		func &= ~Op_SR;
	}
	if (func & Op_IMM) {
		u_int IMM;
		IMM = extract_field(instr, 31 - 19, 4);
		pstr += sprintf (pstr, ", %d", IMM);
		func &= ~Op_SR;
	}
}

void
op_base(instr_t instr)
{
	dis_ppc (opcodes,instr);
}

void
op_cl_x13(instr_t instr)
{
	dis_ppc (opcodes_13,instr);
}

void
op_cl_x1e(instr_t instr)
{
	dis_ppc (opcodes_1e,instr);
}

void
op_cl_x1f(instr_t instr)
{
	dis_ppc (opcodes_1f,instr);
}

void
op_cl_x3a(instr_t instr)
{
	dis_ppc (opcodes_3a,instr);
}

void
op_cl_x3b(instr_t instr)
{
	dis_ppc (opcodes_3b,instr);
}

void
op_cl_x3e(instr_t instr)
{
	dis_ppc (opcodes_3e,instr);
}

void
op_cl_x3f(instr_t instr)
{
	dis_ppc (opcodes_3f,instr);
}

void
dis_ppc(const struct opcode *opcodeset, instr_t instr)
{
	const struct opcode *op;
	int found = 0;
	int i;
	char disasm_str[30];

	for (   i=0, op = &opcodeset[0];
		found == 0 && op->mask != 0;
		i++, op= &opcodeset[i] )
	{
		if ((instr & op->mask) == op->code) {
			found = 1;
			disasm_fields(op, instr, disasm_str);
			db_printf("%s%s",op->name, disasm_str);
			return;
		}
	}
	op_ill(instr);
}

db_addr_t
db_disasm(db_addr_t loc, boolean_t extended)
{
	int class;
	instr_t opcode;
	opcode = *(instr_t *)(loc);
	class = opcode >> 26;
	(opcodes_base[class])(opcode);

	return loc + 4;
}
