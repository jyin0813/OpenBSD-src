#objdump: -dr --prefix-addresses -mmips:3000
#name: MIPS la
#as: -mips1

# Test the la macro.

.*: +file format .*mips.*

Disassembly of section .text:
[0-9a-f]+ <[^>]*> li	a0,0
[0-9a-f]+ <[^>]*> li	a0,1
[0-9a-f]+ <[^>]*> li	a0,0x8000
[0-9a-f]+ <[^>]*> li	a0,-32768
[0-9a-f]+ <[^>]*> lui	a0,0x1
[0-9a-f]+ <[^>]*> lui	a0,0x1
[0-9a-f]+ <[^>]*> ori	a0,a0,0xa5a5
[0-9a-f]+ <[^>]*> addiu	a0,a1,0
[0-9a-f]+ <[^>]*> addiu	a0,a1,1
[0-9a-f]+ <[^>]*> li	a0,0x8000
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> addiu	a0,a1,-32768
[0-9a-f]+ <[^>]*> lui	a0,0x1
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x1
[0-9a-f]+ <[^>]*> ori	a0,a0,0xa5a5
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,gp,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,gp,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,gp,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,gp,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,gp,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,gp,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> addiu	a0,gp,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> addiu	a0,gp,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> addiu	a0,gp,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> addiu	a0,gp,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> addiu	a0,gp,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> addiu	a0,gp,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*GPREL[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,0x0
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,0
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.data.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_data_label
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	big_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	small_external_common
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.bss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
[0-9a-f]+ <[^>]*> lui	a0,[-0-9x]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*HI[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addiu	a0,a0,[-0-9]+
[ 	]*[0-9a-f]+: [A-Z0-9_]*LO[A-Z0-9_]*	.sbss.*
[0-9a-f]+ <[^>]*> addu	a0,a0,a1
	...
