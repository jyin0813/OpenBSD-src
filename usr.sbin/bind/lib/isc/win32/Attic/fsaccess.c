/*
 * Copyright (C) 2000, 2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* $ISC: fsaccess.c,v 1.9 2001/07/09 21:06:07 gson Exp $ */

/*
 * Note that Win32 does not have the concept of files having access
 * and ownership bits.  The FAT File system only has a readonly flag
 * for everyone and that's all. NTFS uses ACL's which is a totally
 * different concept of controlling access.
 *
 * This code needs to be revisited to set up proper access control for
 * NTFS file systems.  Nothing can be done for FAT file systems.
 */

#include <config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <errno.h>

#include <isc/stat.h>

#include "errno2result.h"

/*
 * The OS-independent part of the API is in lib/isc.
 */
#include "../fsaccess.c"

isc_result_t
isc_fsaccess_set(const char *path, isc_fsaccess_t access) {
	struct stat statb;
	int mode;
	isc_boolean_t is_dir = ISC_FALSE;
	isc_fsaccess_t bits;
	isc_result_t result;

	if (stat(path, &statb) != 0)
		return (isc__errno2result(errno));

	if ((statb.st_mode & S_IFDIR) != 0)
		is_dir = ISC_TRUE;
	else if ((statb.st_mode & S_IFREG) == 0)
		return (ISC_R_INVALIDFILE);

	result = check_bad_bits(access, is_dir);
	if (result != ISC_R_SUCCESS)
		return (result);

	/*
	 * Done with checking bad bits.  Set mode_t.
	 */
	mode = 0;

#define SET_AND_CLEAR1(modebit) \
	if ((access & bits) != 0) { \
		mode |= modebit; \
		access &= ~bits; \
	}
#define SET_AND_CLEAR(user, group, other) \
	SET_AND_CLEAR1(user); \
	bits <<= STEP; \
	SET_AND_CLEAR1(group); \
	bits <<= STEP; \
	SET_AND_CLEAR1(other);

	bits = ISC_FSACCESS_READ | ISC_FSACCESS_LISTDIRECTORY;

	SET_AND_CLEAR(S_IRUSR, S_IRGRP, S_IROTH);

	bits = ISC_FSACCESS_WRITE |
	       ISC_FSACCESS_CREATECHILD |
	       ISC_FSACCESS_DELETECHILD;

	SET_AND_CLEAR(S_IWUSR, S_IWGRP, S_IWOTH);

#ifdef notyet
	/*
	 * WIN32 doesn't have the concept of execute bits. We leave this here
	 * for when we review this module.
	 */
	bits = ISC_FSACCESS_EXECUTE |
	       ISC_FSACCESS_ACCESSCHILD;

	SET_AND_CLEAR(S_IXUSR, S_IXGRP, S_IXOTH);
#endif
	INSIST(access == 0);

	if (_chmod(path, mode) < 0)
		return (isc__errno2result(errno));

	return (ISC_R_SUCCESS);
}
