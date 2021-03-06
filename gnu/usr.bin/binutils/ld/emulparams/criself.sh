# This is for embedded products (no MMU) with ELF.
MACHINE=
SCRIPT_NAME=elf
TEMPLATE_NAME=elf32

# Symbols have underscore prepended.
OUTPUT_FORMAT="elf32-us-cris"
ARCH=cris
MAXPAGESIZE=32
ENTRY=__start
EMBEDDED=yes
ALIGNMENT=32
TEXT_START_ADDR=0

# Put crt0 for flash/eprom etc. in this section.
INITIAL_READONLY_SECTIONS='.startup : { KEEP(*(.startup)) }'

# TEXT_START_SYMBOLS doesn't get what we want which is the start of
# all read-only sections; there's at least .init and .fini before it.
# We have to resort to trickery.
#
# The __start dance is to get us through assumptions about entry
# symbols, and to clear _start for normal use with sane programs.
EXECUTABLE_SYMBOLS='
PROVIDE (__Stext = .);
__start = DEFINED(__start) ? __start : 
  DEFINED(_start) ? _start : 
    DEFINED(start) ? start :
      DEFINED(.startup) ? .startup + 2 : 2;
'

# Smuggle an "OTHER_TEXT_END_SYMBOLS" here.
OTHER_READONLY_SECTIONS='PROVIDE (__Etext = .);'
DATA_START_SYMBOLS='PROVIDE (__Sdata = .);'

# Smuggle an "OTHER_DATA_END_SYMBOLS" here.
OTHER_GOT_SECTIONS='PROVIDE (__Edata = .);'

# If .bss does not immediately follow .data but has its own start
# address, we can't get to it with OTHER_BSS_SYMBOLS, neither can we
# use ADDR(.bss) there.  Instead, we use the symbol support for the
# end symbol.
OTHER_BSS_END_SYMBOLS='
 PROVIDE (__Ebss = .);
 PROVIDE (__end = .);
 __Sbss = SIZEOF (.sbss) != 0 ? ADDR (.sbss) : ADDR (.bss);
 PROVIDE (_bss_start = __Sbss);
'

INIT_START='
 . = ALIGN(2);
 ___init__start = .;
 PROVIDE (___do_global_ctors = .);
 SHORT (0xe1fc); /* push srp */
 SHORT (0xbe7e);
'

INIT_END='
 SHORT (0x0d3e); /* jump [sp+] */
 PROVIDE (__init__end = .);
 PROVIDE (___init__end = .);
'

FINI_START='
 . = ALIGN (2);
 ___fini__start = .;
 PROVIDE (___do_global_dtors = .);
 SHORT (0xe1fc); /* push srp */
 SHORT (0xbe7e);
'

FINI_END='
 SHORT (0x0d3e); /* jump [sp+] */
 PROVIDE (__fini__end = .);
 ___fini__end = .;
'

CTOR_START='
 PROVIDE (___ctors = .);
 ___elf_ctors_dtors_begin = .;
'

CTOR_END='
 PROVIDE (___ctors_end = .);
'

DTOR_START='
 PROVIDE (___dtors = .);
'

CTOR_END='
 PROVIDE (___dtors_end = .);
 ___elf_ctors_dtors_end = .;
'


# Smuggle an "OTHER_ALL_END_SYMBOLS" here.
# Also add the other symbols provided for rsim/xsim and elinux.
OTHER_RELOCATING_SECTIONS='
PROVIDE (__Eall = .);
PROVIDE (__Endmem = 0x10000000); 
PROVIDE (__Stacksize = 0);
'
