#!/usr/bin/sed -nf
#
#	$NetBSD: makewhatis.sed,v 1.8 1995/02/23 17:23:54 jtc Exp $
#
# Copyright (c) 1988 The Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#	This product includes software developed by the University of
#	California, Berkeley and its contributors.
# 4. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
#	@(#)makewhatis.sed      8.4 (Berkeley) 4/3/94
#

/[a-zA-Z][a-zA-Z0-9\._+\-]*(\([a-zA-Z0-9\._+\-]*\).*/ {
	s;^.*[a-zA-Z0-9\._+\-]*(\([a-zA-Z0-9\._+\-]*\).*;\1;
	h
	d
}

/^NNAAMMEE/!d

:name
	s;.*;;
	N
	s;\n;;
	# some twits underline the command name
	s;_;;g
	/^[^	 ]/b print
	H
	b name

:print
	x
	s;\n;;g
	/-/!d
	s;.;;g
	s;\([a-z][A-z]\)-[	 ][	 ]*;\1;g
	s;\([a-zA-Z0-9,\._+\-]\)[	 ][	 ]*;\1 ;g
	s;[^a-zA-Z0-9\._+\-]*\([a-zA-Z0-9\._+\-]*\)[^a-zA-Z0-9\._+\-]*\(.*\) - \(.*\);\2 (\1) - \3;
	p
	q
