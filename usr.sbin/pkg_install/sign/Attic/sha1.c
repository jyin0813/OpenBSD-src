/* $OpenBSD: sha1.c,v 1.6 2003/08/16 17:31:56 deraadt Exp $ */
/*-
 * Copyright (c) 1999 Marc Espie.
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
 * THIS SOFTWARE IS PROVIDED BY THE OPENBSD PROJECT AND CONTRIBUTORS 
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OPENBSD
 * PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sha1.h>
#include "stand.h"
#include "gzip.h"
#include "extern.h"

/* private context for sha1 signature checker */
struct sha1_checker {
	SHA1_CTX context;
	const char *id;
	const char *filename;
};

#define SHA1_TEMPLATE "SHA1 (%s) = "
#define BUFSIZE	(MAXID+sizeof(SHA1_TEMPLATE)+2*SHA1_DIGESTSIZE+1)

/* Finalize SHA1 checksum for our sha1_context into result 
	(size at least bufsize).  Returns the length of the checksum
   marker, e.g.,   SHA1 (id) = xxxxxxxxx
                               ^here 
	Return 0 for errors.
 */
size_t 
sha1_build_checksum(result, n, bufsize)
	char *result;
	struct sha1_checker *n;
	size_t bufsize;
{
	size_t length;

	snprintf(result, bufsize, "SHA1 (%s) = ", n->id);
	length = strlen(result);
	SHA1End(&n->context, result + length);
	strlcat(result, "\n", bufsize);
	free(n);	
	return length;
}

void *
new_sha1_checker(h, sign, userid, envp, filename)
	struct mygzip_header *h;
	struct signature *sign;
	const char *userid;	
	char *envp[];
	/*@observer@*/const char *filename;
{
	struct sha1_checker *n;

	assert(sign->type == TAG_SHA1);
	/* make sure data conforms to what we can handle */
	if (sign->length > MAXID || sign->data[sign->length-1] != '\0') {
		pwarnx("Corrupted SHA1 header in %s", filename);
		return 0;
	}

	n = malloc(sizeof *n);
	if (n == NULL) {
		pwarnx("Can't allocate sha1_checker");
		return NULL;
	}
	SHA1Init(&n->context);
	n->id = sign->data;
	n->filename = filename;

	/* copy header, as this is a checksum, we don't strip our own marker */
	if (gzip_copy_header(h, sign, sha1_add, n) == 0) {
		pwarnx("Unexpected header in %s", filename);
		free(n);
		return 0;
	}
	return n;
}
	
void 
sha1_add(arg, buffer, length)
	void *arg;
	const char *buffer;
	size_t length;
{
	struct sha1_checker *n = arg;
	SHA1Update(&n->context, buffer, length);
}

int
sha1_sign_ok(arg)
	void *arg;
{
	struct sha1_checker *n = arg;
	char buffer[BUFSIZE];
	char scan[BUFSIZE];
	size_t length;
	FILE *f;
	int tag_found;

	length = sha1_build_checksum(buffer, n, sizeof(buffer));
	f= fopen(SHA1_DB_NAME, "r");
	tag_found = 0;

	if (f == NULL) {
		warn("Can't access checksum file %s", SHA1_DB_NAME);
		return PKG_BADSIG;
	}
	while (fgets(scan, sizeof(scan), f) != NULL) {
		if (strcmp(scan, buffer) == 0) {
			fprintf(stderr, "Checksum ok\n");
			return PKG_GOODSIG;
		}
		if (strncmp(scan, buffer, length) == 0)
			tag_found = 1;
	}

	if (tag_found) {
		pwarnx("Checksum incorrect for %s (%s)", n->filename, n->id);
		return PKG_BADSIG;
	} else {
		pwarnx("No checksum found for %s (%s)", n->filename, n->id);
		return PKG_SIGUNKNOWN;
	}
}

int 
retrieve_sha1_marker(filename, sign, userid)
	const char *filename;
	struct signature **sign;
	const char *userid;
{
	struct signature *n;
	struct mygzip_header h;
	FILE *f;
	char buffer[1024];
	char result[BUFSIZE];
	ssize_t length;
	struct sha1_checker *checker;
	struct signature *old;

	*sign = NULL;
	if (userid == NULL)
		return 0;

	n = malloc(sizeof *n);
	if (n == NULL) 
		return 0;
	n->data = (char *)userid;
	n->length = strlen(n->data)+1;
	n->type = TAG_SHA1;
	memcpy(n->tag, sha1tag, sizeof sha1tag);
	sign_fill_tag(n);

	f = fopen(filename, "r");
	if (f == NULL) {
		free(n);
		return 0;
	}
	if (gzip_read_header(f, &h, sign) == GZIP_NOT_GZIP) {
		pwarnx("File %s is not a gzip file", filename);
		fclose(f);
		free(n);
		return 0;
	}
	n->next = *sign;
	*sign = n;

	checker = new_sha1_checker(&h, *sign, NULL, NULL, filename);
	while ((length = fread(buffer, 1, sizeof buffer, f)) > 0)
		sha1_add(checker, buffer, length);
	if (fclose(f) != 0 || length == -1) {
		warn("Problem checksumming %s", filename);
		*sign = n->next;
		free(n);
		return 0;
	}

	(void)sha1_build_checksum(result, checker, sizeof(result));
	fputs(result, stderr);
	return 1;
}

