/*-
 * Copyright (c) 1997 Brian Somers <brian@Awfulhak.org>
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$Id: loadalias.h,v 1.2 1997/12/21 14:27:08 brian Exp $
 */

struct aliasHandlers {
  char *(*PacketAliasGetFragment)(char *);
  void (*PacketAliasInit)(void);
  int (*PacketAliasIn)(char *, int);
  int (*PacketAliasOut)(char *, int);
  struct alias_link *(*PacketAliasRedirectAddr)(struct in_addr, struct in_addr);
  struct alias_link *(*PacketAliasRedirectPort)
    (struct in_addr, u_short, struct in_addr, u_short,
     struct in_addr, u_short, u_char);
  int (*PacketAliasSaveFragment)(char *);
  void (*PacketAliasSetAddress)(struct in_addr);
  unsigned (*PacketAliasSetMode)(unsigned, unsigned);
  void (*PacketAliasFragmentIn)(char *, char *);
};

extern int loadAliasHandlers(struct aliasHandlers *);
extern void unloadAliasHandlers(void);
