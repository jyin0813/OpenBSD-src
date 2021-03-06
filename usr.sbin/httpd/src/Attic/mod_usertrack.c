/* ====================================================================
 * Copyright (c) 1995-1997 The Apache Group.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group and was originally based
 * on public domain software written at the National Center for
 * Supercomputing Applications, University of Illinois, Urbana-Champaign.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 */

/* User Tracking Module (Was mod_cookies.c)
 *
 * This Apache module is designed to track users paths through a site.
 * It uses the client-side state ("Cookie") protocol developed by Netscape.
 * It is known to work on Netscape browsers, Microsoft Internet 
 * Explorer and others currently being developed.
 *
 * Each time a page is requested we look to see if the browser is sending
 * us a Cookie: header that we previously generated.
 *
 * If we don't find one then the user hasn't been to this site since
 * starting their browser or their browser doesn't support cookies.  So
 * we generate a unique Cookie for the transaction and send it back to
 * the browser (via a "Set-Cookie" header)
 * Future requests from the same browser should keep the same Cookie line.
 *
 * By matching up all the requests with the same cookie you can
 * work out exactly what path a user took through your site.  To log
 * the cookie use the " %{Cookie}n " directive in a custom access log;
 *
 * Example 1 : If you currently use the standard Log file format (CLF)
 * and use the command "TransferLog somefilename", add the line
 *       LogFormat "%h %l %u %t \"%r\" %s %b %{Cookie}n" 
 * to your config file.
 *
 * Example 2 : If you used to use the old "CookieLog" directive, you
 * can emulate it by adding the following command to your config file
 *       CustomLog filename "%{Cookie}n \"%r\" %t"
 *
 * Notes:
 * 1.  This code now logs the initial transaction (the one that created
 *     the cookie to start with).  
 * 2.  This module has been designed to not interfere with other Cookies
 *     your site may be using; just avoid sending out cookies with
 *     the name "Apache=" or things will get confused.
 * 3.  If you want you can modify the Set-Cookie line so that the Cookie
 *     never expires.  You would then get the same Cookie each time the
 *     user revisits your site.
 *
 * Mark Cox, mark@ukweb.com, 6 July 95
 *
 * This file replaces mod_cookies.c
 */

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#ifndef MPE
#include <sys/time.h>
#endif

module usertrack_module;

typedef struct {
    int always;
    time_t expires;
} cookie_log_state;

/* Define this to allow post-2000 cookies. Cookies use two-digit dates,
 * so it might be dicey. (Netscape does it correctly, but others may not)
 */
#define MILLENIAL_COOKIES

/* Make Cookie: Now we have to generate something that is going to be
 * pretty unique.  We can base it on the pid, time, hostip */

#define COOKIE_NAME "Apache="

void make_cookie(request_rec *r)
{
    cookie_log_state *cls = get_module_config (r->server->module_config,
					       &usertrack_module);
#ifdef MPE
    clock_t mpe_times;
    struct tms mpe_tms;
#else
    struct timeval tv;
    struct timezone tz = { 0 , 0 };
#endif
    /* 1024 == hardcoded constants */
    char *new_cookie = palloc( r->pool, 1024);	
    char *cookiebuf = palloc( r->pool, 1024);
    char *dot;
    const char *rname = pstrdup(r->pool, 
		       	    get_remote_host(r->connection, r->per_dir_config,
						REMOTE_NAME));

    if ((dot = strchr(rname,'.'))) *dot='\0';	/* First bit of hostname */

#ifdef MPE
/* MPE lacks gettimeofday(), so we must use time() to obtain the epoch
   seconds, and then times() to obtain CPU clock ticks (milliseconds).
   Combine this together to obtain a hopefully unique cookie ID. */

    mpe_times=times(&mpe_tms);

    ap_snprintf(cookiebuf, 1024, "%s%d%ld%ld", rname, (int)getpid(),
              (long)time(NULL), (long)mpe_tms.tms_utime);
#else
    gettimeofday(&tv, &tz);

    ap_snprintf(cookiebuf, 1024, "%s%d%ld%d", rname, (int)getpid(),
	      (long)tv.tv_sec, (int)tv.tv_usec/1000);	    
#endif

    if (cls->expires) {
      static const char *const days[7]=
          {"Sun","Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
      struct tm *tms;
      time_t when = time(NULL) + cls->expires;

#ifndef MILLENIAL_COOKIES      
      /* Only two-digit date string, so we can't trust "00" or more.
       * Therefore, we knock it all back to just before midnight on
       * 1/1/2000 (which is 946684799)
       */

      if (when > 946684799)
	when = 946684799;
#endif
      tms = gmtime(&when);

      /* Cookie with date; as strftime '%a, %d-%h-%y %H:%M:%S GMT' */
      ap_snprintf(new_cookie, 1024,
	   "%s%s; path=/; expires=%s, %.2d-%s-%.2d %.2d:%.2d:%.2d GMT",
	      COOKIE_NAME, cookiebuf, days[tms->tm_wday],
	      tms->tm_mday, month_snames[tms->tm_mon],
	      (tms->tm_year >= 100) ? tms->tm_year - 100 : tms->tm_year,
	      tms->tm_hour, tms->tm_min, tms->tm_sec);
    }
    else
      ap_snprintf(new_cookie, 1024, "%s%s; path=/", COOKIE_NAME, cookiebuf);

    table_set(r->headers_out,"Set-Cookie",new_cookie);
    table_set(r->notes, "cookie", cookiebuf); /* log first time */
    return;
}

int spot_cookie(request_rec *r)
{
    int *enable = (int *)get_module_config(r->per_dir_config,
					    &usertrack_module);
    char *cookie;
    char *value;

    if (!*enable) return DECLINED;

    if ((cookie = table_get (r->headers_in, "Cookie")))
      if ((value=strstr(cookie,COOKIE_NAME))) {
	char *cookiebuf, *cookieend;

	value+=strlen(COOKIE_NAME);
	cookiebuf=pstrdup( r->pool, value );
	cookieend=strchr(cookiebuf,';');
	if (cookieend) *cookieend='\0';	/* Ignore anything after a ; */

	/* Set the cookie in a note, for logging */
	table_set(r->notes, "cookie", cookiebuf);

	return DECLINED;          /* Theres already a cookie, no new one */
      }
    make_cookie(r);
    return OK;                        /* We set our cookie */
}

void *make_cookie_log_state (pool *p, server_rec *s)
{
    cookie_log_state *cls =
      (cookie_log_state *)palloc (p, sizeof (cookie_log_state));

    cls->expires = 0;

    return (void *)cls;
}

void *make_cookie_dir (pool *p, char *d) {
    return (void *)pcalloc(p, sizeof(int));
}

const char *set_cookie_enable (cmd_parms *cmd, int *c, int arg)
{
    *c = arg;
    return NULL;
}

const char *set_cookie_exp (cmd_parms *parms, void *dummy, const char *arg)
{
    cookie_log_state *cls = get_module_config (parms->server->module_config,
                           &usertrack_module);
    time_t factor, modifier = 0;
    time_t num = 0;
    char *word;

    /* The simple case first - all numbers (we assume) */
    if (isdigit(arg[0]) && isdigit(arg[strlen(arg)-1])) {
      cls->expires = atol(arg);
      return NULL;
    }

    /* The harder case - stolen from mod_expires
     * CookieExpires "[plus] {<num> <type>}*"
     */

    word = getword_conf( parms->pool, &arg );
    if ( !strncasecmp( word, "plus", 1 ) ) {
        word = getword_conf( parms->pool, &arg );
    };

    /* {<num> <type>}* */
    while ( word[0] ) {
        /* <num> */
        if ( strchr("0123456789", word[0]) != NULL )
	  num = atoi( word );
	else
	  return "bad expires code, numeric value expected.";
      
	/* <type> */
	word = getword_conf( parms->pool, &arg );
	if (!word[0] )
	  return "bad expires code, missing <type>";
	  
	factor = 0;
	if ( !strncasecmp( word, "years", 1 ) )
	  factor = 60*60*24*365;
	else if ( !strncasecmp( word, "months", 2 ) )
	  factor = 60*60*24*30;
	else if ( !strncasecmp( word, "weeks", 1 ) )
	  factor = 60*60*24*7;
	else if ( !strncasecmp( word, "days", 1 ) )
	  factor = 60*60*24;
	else if ( !strncasecmp( word, "hours", 1 ) )
	  factor = 60*60;
	else if ( !strncasecmp( word, "minutes", 2 ) )
	  factor = 60;
	else if ( !strncasecmp( word, "seconds", 1 ) )
	  factor = 1;
	else
	  return "bad expires code, unrecognized type";

	modifier = modifier + factor * num;

	/* next <num> */
	word = getword_conf( parms->pool, &arg );
    }

    cls->expires = modifier;

    return NULL;
}

command_rec cookie_log_cmds[] = {
{ "CookieExpires", set_cookie_exp, NULL, RSRC_CONF, TAKE1,
    "an expiry date code" },
{ "CookieTracking", set_cookie_enable, NULL, OR_FILEINFO, FLAG,
    "whether or not to enable cookies" },
{ NULL }
};

module usertrack_module = {
   STANDARD_MODULE_STUFF,
   NULL,                   /* initializer */
   make_cookie_dir,    	   /* dir config creater */
   NULL,	      	   /* dir merger --- default is to override */
   make_cookie_log_state,  /* server config */
   NULL,		   /* merge server configs */
   cookie_log_cmds,	   /* command table */
   NULL,		   /* handlers */
   NULL,		   /* filename translation */
   NULL,		   /* check_user_id */
   NULL,		   /* check auth */
   NULL,		   /* check access */
   NULL,		   /* type_checker */
   spot_cookie,		   /* fixups */
   NULL,                   /* logger */
   NULL			   /* header parser */
};
