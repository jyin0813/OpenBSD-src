/* $OpenBSD: new_pwd.c,v 1.7 2007/03/20 03:50:39 tedu Exp $ */
/* $KTH: new_pwd.c,v 1.11 1997/05/02 14:28:54 assar Exp $ */

/*
 * Copyright (c) 1995, 1996, 1997 Kungliga Tekniska H�gskolan
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Kungliga Tekniska
 *      H�gskolan and its contributors.
 *
 * 4. Neither the name of the Institute nor the names of its contributors
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <des.h>
#include <kerberosIV/krb.h>
#include <kerberosIV/kadm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef NOENCRYPTION
#define read_long_pw_string placebo_read_pw_string
#else
#define read_long_pw_string des_read_pw_string
#endif

static char *
check_pw(char *pword)
{
	char *t;

	if (strlen(pword) == 0)
		return "Null passwords are not allowed - Please enter a longer password.";

	if (strlen(pword) < MIN_KPW_LEN)
		return "Password is to short - Please enter a longer password.";

	if (strcmp(pword, "s/key") == 0)
		return "That password collides with a system feature. Choose another.\n";

	/* Don't allow all lower case passwords regardless of length */
	for (t = pword; islower(*t); t++)
		;
	if (*t == 0)
		return "Please don't use an all-lower case password.\n"
		    "\tUnusual capitalization, delimiter characters or "
		    "digits are suggested.";
	return NULL;
}

int
get_pw_new_pwd(char *pword, int pwlen, krb_principal *pr, int print_realm)
{
	char ppromp[40+ANAME_SZ+INST_SZ+REALM_SZ]; /* for the password prompt */
	char npromp[40+ANAME_SZ+INST_SZ+REALM_SZ]; /* for the password prompt */
	char p[MAX_K_NAME_SZ];
	char local_realm[REALM_SZ];
	int status;
	char *expl;
	char *q;

	/*
	 * We don't care about failure; this is to determine whether or
	 * not to print the realm in the prompt for a new password.
	 */
	krb_get_lrealm(local_realm, 1);

	if (strcmp(local_realm, pr->realm))
		print_realm++;
	krb_unparse_name_r(pr, p);
	if (print_realm == 0 && (q = strrchr(p, '@')))
		*q = 0;

	snprintf(ppromp, sizeof(ppromp), "Old password for %s:", p);
	if (read_long_pw_string(pword, pwlen-1, ppromp, 0)) {
		fprintf(stderr, "Error reading old password.\n");
		return -1;
	}

	status = krb_get_pw_in_tkt(pr->name, pr->instance, pr->realm,
	    PWSERV_NAME, KADM_SINST, 1, pword);
	if (status != KSUCCESS) {
		if (status == INTK_BADPW) {
			printf("Incorrect old password.\n");
			return -1;
		} else {
			fprintf(stderr, "Kerberos error: %s\n",
			    krb_get_err_text(status));
			return -1;
		}
	}

	memset(pword, 0, pwlen);

	do {
		char verify[MAX_KPW_LEN];
		snprintf(npromp, sizeof(npromp), "New Password for %s:",p);
		if (read_long_pw_string(pword, pwlen-1, npromp, 0)) {
			fprintf(stderr,
			    "Error reading new password, password unchanged.\n");
			return -1;
		}
		expl = check_pw (pword);
		if (expl) {
			printf("\n\t%s\n\n", expl);
			continue;
		}

		/* Now we got an ok password, verify it. */
		snprintf(npromp, sizeof(npromp),
		    "Verifying New Password for %s:", p);
		if (read_long_pw_string(verify, MAX_KPW_LEN-1, npromp, 0)) {
			fprintf(stderr,
			    "Error reading new password, password unchanged.\n");
			return -1;
		}
		if (strcmp(pword, verify) != 0) {
			printf("Verify failure - try again\n");
			expl = "";		/* continue */
		}
	} while (expl);
	return 0;
}
