/*
 * Copyright (c) 2004 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 */

#include "gssapi_locl.h"

RCSID("$KTH: ccache_name.c,v 1.1 2004/01/25 19:08:57 lha Exp $");

char *last_out_name;

OM_uint32
gss_krb5_ccache_name(OM_uint32 *minor_status, 
		     const char *name,
		     const char **out_name)
{
    krb5_error_code kret;

    *minor_status = 0;

    GSSAPI_KRB5_INIT();

    if (out_name) {
	const char *name;

	if (last_out_name) {
	    free(last_out_name);
	    last_out_name = NULL;
	}

	name = krb5_cc_default_name(gssapi_krb5_context);
	if (name == NULL) {
	    *minor_status = ENOMEM;
	    gssapi_krb5_set_error_string ();
	    return GSS_S_FAILURE;
	}
	last_out_name = strdup(name);
	if (last_out_name == NULL) {
	    *minor_status = ENOMEM;
	    return GSS_S_FAILURE;
	}
	*out_name = last_out_name;
    }

    kret = krb5_cc_set_default_name(gssapi_krb5_context, name);
    if (kret) {
	*minor_status = kret;
	gssapi_krb5_set_error_string ();
	return GSS_S_FAILURE;
    }
    return GSS_S_COMPLETE;
}
