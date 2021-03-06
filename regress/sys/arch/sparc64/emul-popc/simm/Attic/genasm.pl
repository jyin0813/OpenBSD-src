#!/bin/perl
#	$OpenBSD: genasm.pl,v 1.3 2003/07/10 15:21:12 jason Exp $
#
# Copyright (c) 2003 Jason L. Wright (jason@thought.net)
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
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

print "/* AUTOMATICALLY GENERATED, DO NOT EDIT */\n";
print "#include <machine/asm.h>\n";
print "#include <machine/trap.h>\n";
print "#include <machine/frame.h>\n";
print "\n";

for ($i = -4096; $i <= 4095; $i++) {
	print "ENTRY(popc";
	if ($i < 0) {
		$v =  -$i;
		print "__$v";
	} else {
		print "_$i";
	}
	print ")\n";

	print "\tretl\n";
	print "\t popc $i, %o0\n\n";
}
