/*	$OpenBSD$	*/
/*
 * Copyright (c) 1995, 1996 Erik Theisen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
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
#include "exec_sup.h"

/*
 * __elf_is_okay__ - Determine if ehdr really
 * is ELF and valid for the target platform.
 *
 * WARNING:  This is NOT a ELF ABI function and
 * as such it's use should be restricted.
 */
int
__elf_is_okay__(ehdr)
        register Elf32_Ehdr *ehdr;
{
        register int retval = 0;
        
        /*
         * We need to check magic, class size, endianess,
         * and version before we look at the rest of the
         * Elf32_Ehdr structure.  These few elements are
         * represented in a machine independant fashion.
         */
        if(IS_ELF(*ehdr) &&
           ehdr->e_ident[EI_CLASS] == ELF_TARG_CLASS &&
           ehdr->e_ident[EI_DATA] == ELF_TARG_DATA &&
           ehdr->e_ident[EI_VERSION] == ELF_TARG_VER) {

                /* Now check the machine dependant header */
                if(ehdr->e_machine == ELF_TARG_MACH &&
                   ehdr->e_version == ELF_TARG_VER)
                        retval = 1;
        }
        return retval;
} /* end __elf_is_okay__() */
