/*	$OpenBSD: common.c,v 1.6 1999/02/28 14:14:12 art Exp $	*/
/*	$KTH: common.c,v 1.10 1998/04/04 13:08:31 assar Exp $	*/

/*
 * Copyright (c) 1997, 1998 Kungliga Tekniska H�gskolan
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
 *      This product includes software developed by Kungliga Tekniska 
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

#include "kafs_locl.h"
#include <resolve.h>

#define AUTH_SUPERUSER "afs"

/*
 * Here only ASCII characters are relevant.
 */

#define IsAsciiLower(c) ('a' <= (c) && (c) <= 'z')

#define ToAsciiUpper(c) ((c) - 'a' + 'A')

static void
foldup(char *a, const char *b)
{
  for (; *b; a++, b++)
    if (IsAsciiLower(*b))
      *a = ToAsciiUpper(*b);
    else
      *a = *b;
  *a = '\0';
}

int
kafs_settoken(const char *cell, uid_t uid, CREDENTIALS *c)
{
    struct ViceIoctl parms;
    struct ClearToken ct;
    int32_t sizeof_x;
    char buf[2048], *t;
    int ret;
    
    /*
     * Build a struct ClearToken
     */
    ct.AuthHandle = c->kvno;
    memcpy (ct.HandShakeKey, c->session, sizeof(c->session));
    ct.ViceId = uid;
    ct.BeginTimestamp = c->issue_date;
    ct.EndTimestamp = krb_life_to_time(c->issue_date, c->lifetime);
    if(ct.EndTimestamp < time(NULL))
	return 0; /* don't store tokens that has expired (and possibly
		     overwriting valid tokens)*/

#define ODD(x) ((x) & 1)
    /* According to Transarc conventions ViceId is valid iff
     * (EndTimestamp - BeginTimestamp) is odd. By decrementing EndTime
     * the transformations:
     *
     * (issue_date, life) -> (StartTime, EndTime) -> (issue_date, life)
     * preserves the original values.
     */
    if (uid != 0)		/* valid ViceId */
      {
	if (!ODD(ct.EndTimestamp - ct.BeginTimestamp))
	  ct.EndTimestamp--;
      }
    else			/* not valid ViceId */
      {
	if (ODD(ct.EndTimestamp - ct.BeginTimestamp))
	  ct.EndTimestamp--;
      }

    t = buf;
    /*
     * length of secret token followed by secret token
     */
    sizeof_x = c->ticket_st.length;
    memcpy(t, &sizeof_x, sizeof(sizeof_x));
    t += sizeof(sizeof_x);
    memcpy(t, c->ticket_st.dat, sizeof_x);
    t += sizeof_x;
    /*
     * length of clear token followed by clear token
     */
    sizeof_x = sizeof(ct);
    memcpy(t, &sizeof_x, sizeof(sizeof_x));
    t += sizeof(sizeof_x);
    memcpy(t, &ct, sizeof_x);
    t += sizeof_x;

    /*
     * do *not* mark as primary cell
     */
    sizeof_x = 0;
    memcpy(t, &sizeof_x, sizeof(sizeof_x));
    t += sizeof(sizeof_x);
    /*
     * follow with cell name
     */
    sizeof_x = strlen(cell) + 1;
    memcpy(t, cell, sizeof_x);
    t += sizeof_x;

    /*
     * Build argument block
     */
    parms.in = buf;
    parms.in_size = t - buf;
    parms.out = 0;
    parms.out_size = 0;
    ret = k_pioctl(0, VIOCSETTOK, &parms, 0);
    return ret;
}

/* Try to get a db-server for an AFS cell from a AFSDB record */

static int
dns_find_cell(const char *cell, char *dbserver, size_t len)
{
    struct dns_reply *r;
    int ok = -1;
    r = dns_lookup(cell, "afsdb");
    if(r){
	struct resource_record *rr = r->head;
	while(rr){
	    if(rr->type == T_AFSDB && rr->u.afsdb->preference == 1){
		strncpy(dbserver, rr->u.afsdb->domain, len);
		dbserver[len - 1] = '\0';
		ok = 0;
		break;
	    }
	    rr = rr->next;
	}
	dns_free_data(r);
    }
    return ok;
}


/*
 * Try to find the cells we should try to klog to in "file".
 */
static void
find_cells(char *file, char ***cells, int *index)
{
    FILE *f;
    char cell[64];
    int i;
    int ind = *index;

    f = fopen(file, "r");
    if (f == NULL)
	return;
    while (fgets(cell, sizeof(cell), f)) {
	char *nl = strchr(cell, '\n');
	if (nl) *nl = 0;

	/* skip blank lines */
	if (!cell[0]) continue;

	for(i = 0; i < ind; i++)
	    if(strcmp((*cells)[i], cell) == 0)
		break;
	if(i == ind){
	    *cells = realloc(*cells, (ind + 1) * sizeof(**cells));
	    if (*cells == NULL)
		break;
	    (*cells)[ind] = strdup(cell);
	    if ((*cells)[ind] == NULL)
		break;
	    ++ind;
	}
    }
    fclose(f);
    *index = ind;
}

/*
 * Get tokens for all cells[]
 */
static int
afslog_cells(kafs_data *data, char **cells, int max, uid_t uid)
{
    int ret = 0;
    int i;
    for(i = 0; i < max; i++)
	ret = (*data->afslog_uid)(data, cells[i], uid);
    return ret;
}

int
_kafs_afslog_all_local_cells(kafs_data *data, uid_t uid)
{
    int ret;
    char **cells = NULL;
    int index = 0;

    char *p;
    
    if ((p = getenv("HOME"))) {
	char home[MAXPATHLEN];
	snprintf(home, sizeof(home), "%s/.TheseCells", p);
	find_cells(home, &cells, &index);
    }
    find_cells(_PATH_THESECELLS, &cells, &index);
    find_cells(_PATH_THISCELL, &cells, &index);
    find_cells(_PATH_ARLA_THESECELLS, &cells, &index);
    find_cells(_PATH_ARLA_THISCELL, &cells, &index);
    
    ret = afslog_cells(data, cells, index, uid);
    while(index > 0) {
	free(cells[--index]);
	cells[index] = NULL;
    }
    free(cells);
    cells = NULL;
    return ret;
}


/* Find the realm associated with cell. Do this by opening
   /usr/vice/etc/CellServDB and getting the realm-of-host for the
   first VL-server for the cell.

   This does not work when the VL-server is living in one realm, but
   the cell it is serving is living in another realm.

   Return 0 on success, -1 otherwise.
   */

static int
realm_of_cell(kafs_data *data, const char *cell, char **realm)
{
    FILE *F;
    char buf[1024];
    char *p;
    int ret = -1;

    if ((F = fopen(_PATH_CELLSERVDB, "r"))
	|| (F = fopen(_PATH_ARLA_CELLSERVDB, "r"))) {
	while (fgets(buf, sizeof(buf), F)) {
	    if (buf[0] != '>')
	      continue;		/* Not a cell name line, try next line */
	    if (strncmp(buf + 1, cell, strlen(cell)) == 0) {
		/*
		 * We found the cell name we're looking for.
		 * Read next line on the form ip-address '#' hostname
		 */
		if (fgets(buf, sizeof(buf), F) == NULL)
		  break;	/* Read failed, give up */
		p = strchr(buf, '#');
		if (p == NULL)
		  break;	/* No '#', give up */
		p++;
		if (buf[strlen(buf) - 1] == '\n')
		    buf[strlen(buf) - 1] = '\0';
		*realm = (*data->get_realm)(data, p);
		if (*realm && **realm != '\0')
		  ret = 0;
		break;		/* Won't try any more */
	      }
	  }
	fclose(F);
      }
    if (*realm == NULL && dns_find_cell(cell, buf, sizeof(buf)) == 0)
	*realm = strdup(krb_realmofhost(buf));
    return ret;
}

int
_kafs_get_cred(kafs_data *data,
	       const char *cell, 
	       const char *krealm, 
	       const char *lrealm,
	       CREDENTIALS *c)
{
    int ret = -1;
    char *vl_realm;
    char CELL[64];

    /* We're about to find the the realm that holds the key for afs in
     * the specified cell. The problem is that null-instance
     * afs-principals are common and that hitting the wrong realm might
     * yield the wrong afs key. The following assumptions were made.
     *
     * Any realm passed to us is preferred.
     *
     * If there is a realm with the same name as the cell, it is most
     * likely the correct realm to talk to.
     *
     * In most (maybe even all) cases the database servers of the cell
     * will live in the realm we are looking for.
     *
     * Try the local realm, but if the previous cases fail, this is
     * really a long shot.
     *
     */
  
    /* comments on the ordering of these tests */

    /* If the user passes a realm, she probably knows something we don't
     * know and we should try afs@krealm (otherwise we're talking with a
     * blondino and she might as well have it.)
     */
  
    if (krealm != NULL) {
	ret = (*data->get_cred)(data, AUTH_SUPERUSER, cell, krealm, c);
	if (ret == 0) return 0;
	ret = (*data->get_cred)(data, AUTH_SUPERUSER, "", krealm, c);
    }
    if (ret == 0) return 0;

    foldup(CELL, cell);

    ret = (*data->get_cred)(data, AUTH_SUPERUSER, cell, CELL, c);
    if (ret == 0) return 0;

    ret = (*data->get_cred)(data, AUTH_SUPERUSER, "", CELL, c);
    if (ret == 0) return 0;
    
    /* this might work in some cases */
    if (realm_of_cell(data, cell, &vl_realm) == 0) {
	ret = (*data->get_cred)(data, AUTH_SUPERUSER, cell, vl_realm, c);
	if (ret)
	    ret = (*data->get_cred)(data, AUTH_SUPERUSER, "", vl_realm, c);
	free(vl_realm);
	if (ret == 0) return 0;
    }
    
    if (lrealm)
	ret = (*data->get_cred)(data, AUTH_SUPERUSER, cell, lrealm, c);
    return ret;
}


