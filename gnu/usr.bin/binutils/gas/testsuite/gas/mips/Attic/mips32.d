#objdump: -dr --prefix-addresses --show-raw-insn -mmips:mips32
#name: MIPS MIPS32 instructions
#as: -mips32

# Check MIPS32 instruction assembly

.*: +file format .*mips.*

Disassembly of section .text:
0+0000 <[^>]*> 70410821 	clo	at,v0
0+0004 <[^>]*> 70831820 	clz	v1,a0
0+0008 <[^>]*> 70a60000 	madd	a1,a2
0+000c <[^>]*> 70e80001 	maddu	a3,t0
0+0010 <[^>]*> 712a0004 	msub	t1,t2
0+0014 <[^>]*> 716c0005 	msubu	t3,t4
0+0018 <[^>]*> 71cf6802 	mul	t5,t6,t7
0+001c <[^>]*> ce040000 	pref	0x4,0\(s0\)
0+0020 <[^>]*> ce247fff 	pref	0x4,32767\(s1\)
0+0024 <[^>]*> ce448000 	pref	0x4,-32768\(s2\)
0+0028 <[^>]*> 00000040 	ssnop
0+002c <[^>]*> 4900fff4 	bc2f	0+0000 <text_label>
0+0030 <[^>]*> 00000000 	nop
0+0034 <[^>]*> 4902fff2 	bc2fl	0+0000 <text_label>
0+0038 <[^>]*> 00000000 	nop
0+003c <[^>]*> 4901fff0 	bc2t	0+0000 <text_label>
0+0040 <[^>]*> 00000000 	nop
0+0044 <[^>]*> 4903ffee 	bc2tl	0+0000 <text_label>
0+0048 <[^>]*> 00000000 	nop
0+004c <[^>]*> 48411000 	cfc2	at,v0
0+0050 <[^>]*> 4b234567 	c2	0x1234567
0+0054 <[^>]*> 48c21800 	ctc2	v0,v1
0+0058 <[^>]*> 48032000 	mfc2	v1,a0
0+005c <[^>]*> 48042800 	mfc2	a0,a1
0+0060 <[^>]*> 48053007 	mfc2	a1,a2,7
0+0064 <[^>]*> 48863800 	mtc2	a2,a3
0+0068 <[^>]*> 48874000 	mtc2	a3,t0
0+006c <[^>]*> 48884807 	mtc2	t0,t1,7
0+0070 <[^>]*> bc250000 	cache	0x5,0\(at\)
0+0074 <[^>]*> bc457fff 	cache	0x5,32767\(v0\)
0+0078 <[^>]*> bc658000 	cache	0x5,-32768\(v1\)
0+007c <[^>]*> 42000018 	eret
0+0080 <[^>]*> 42000008 	tlbp
0+0084 <[^>]*> 42000001 	tlbr
0+0088 <[^>]*> 42000002 	tlbwi
0+008c <[^>]*> 42000006 	tlbwr
0+0090 <[^>]*> 42000020 	wait
0+0094 <[^>]*> 42000020 	wait
0+0098 <[^>]*> 4359e260 	wait	0x56789
0+009c <[^>]*> 0000000d 	break
0+00a0 <[^>]*> 0000000d 	break
0+00a4 <[^>]*> 0048d14d 	break	0x12345
0+00a8 <[^>]*> 7000003f 	sdbbp
0+00ac <[^>]*> 7000003f 	sdbbp
0+00b0 <[^>]*> 7159e27f 	sdbbp	0x56789
	...
