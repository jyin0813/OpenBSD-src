/*	$Id: error.c,v 1.1 1995/12/14 06:52:47 tholo Exp $	*/

/*-
 * Copyright 1987, 1988 by the Student Information Processing Board
 *	of the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is
 * hereby granted, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation,
 * and that the names of M.I.T. and the M.I.T. S.I.P.B. not be
 * used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * M.I.T. and the M.I.T. S.I.P.B. make no representations about
 * the suitability of this software for any purpose.  It is
 * provided "as is" without express or implied warranty.
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Our standalone dpANS environment on the RT doesn't include any
 * header files.
 */
#if defined(__STDC__) && !defined(ibm032)
#include <stdarg.h>
#define STDARG
#else
#include <varargs.h>
#define ss_error ss_error_external
#endif

#include <kerberosIV/com_err.h>
#include "ss_internal.h"

#undef ss_error

char *
ss_name(sci_idx)
    int sci_idx;
{
    register char *ret_val;
    register ss_data *infop;
    
    infop = ss_info(sci_idx);
    if (infop->current_request == (char const *)NULL) {
	ret_val = malloc((unsigned)
			 (strlen(infop->subsystem_name)+1)
			 * sizeof(char));
	if (ret_val == (char *)NULL)
	    return((char *)NULL);
	strcpy(ret_val, infop->subsystem_name);
	return(ret_val);
    }
    else {
	register char *cp;
	register char const *cp1;
	ret_val = malloc((unsigned)sizeof(char) * 
			 (strlen(infop->subsystem_name)+
			  strlen(infop->current_request)+
			  4));
	cp = ret_val;
	cp1 = infop->subsystem_name;
	while (*cp1)
	    *cp++ = *cp1++;
	*cp++ = ' ';
	*cp++ = '(';
	cp1 = infop->current_request;
	while (*cp1)
	    *cp++ = *cp1++;
	*cp++ = ')';
	*cp = '\0';
	return(ret_val);
    }
}

#ifdef STDARG
void
ss_error (int sci_idx, long code, const char * fmt, ...)
#else
void
ss_error (va_alist)
    va_dcl
#endif
{
    register char *whoami;
    va_list pvar;
#ifndef STDARG
    int sci_idx;
    long code;
    char * fmt;
    va_start (pvar);
    sci_idx = va_arg (pvar, int);
    code = va_arg (pvar, long);
    fmt = va_arg (pvar, char *);
#else
    va_start (pvar, fmt);
#endif
    whoami = ss_name (sci_idx);
    com_err_va (whoami, code, fmt, pvar);
    free (whoami);
    va_end(pvar);
}

void
ss_perror (sci_idx, code, msg) /* for compatibility */
    int sci_idx;
    long code;
    char const *msg;
{
    ss_error (sci_idx, code, "%s", msg);
}
