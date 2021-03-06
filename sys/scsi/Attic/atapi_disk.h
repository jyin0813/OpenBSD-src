/*	$OpenBSD: atapi_disk.h,v 1.4 2005/06/19 20:41:28 krw Exp $	*/
/*	$NetBSD: atapi_disk.h,v 1.3 1998/02/13 08:28:21 enami Exp $	*/

/*
 * Copyright 1998
 * Digital Equipment Corporation. All rights reserved.
 *
 * This software is furnished under license and may be used and
 * copied only in accordance with the following terms and conditions.
 * Subject to these conditions, you may download, copy, install,
 * use, modify and distribute this software in source and/or binary
 * form. No title or ownership is transferred hereby.
 *
 * 1) Any source code used, modified or distributed must reproduce
 *    and retain this copyright notice and list of conditions as
 *    they appear in the source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of
 *    Digital Equipment Corporation. Neither the "Digital Equipment
 *    Corporation" name nor any trademark or logo of Digital Equipment
 *    Corporation may be used to endorse or promote products derived
 *    from this software without the prior written permission of
 *    Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied
 *    warranties, including but not limited to, any implied warranties
 *    of merchantability, fitness for a particular purpose, or
 *    non-infringement are disclaimed. In no event shall DIGITAL be
 *    liable for any damages whatsoever, and in particular, DIGITAL
 *    shall not be liable for special, indirect, consequential, or
 *    incidental damages or damages for lost profits, loss of
 *    revenue or loss of use, whether such damages arise in contract,
 *    negligence, tort, under statute, in equity, at law or otherwise,
 *    even if advised of the possibility of such damage.
 */

/*
 * Definitions of commands and structures specific to ATAPI disks.
 *
 * Chris Demetriou, January 10, 1998.
 */

#define ATAPI_READ_FORMAT_CAPACITIES	0x23
struct atapi_read_format_capacities {
	u_int8_t opcode;
	u_int8_t byte2;
	u_int8_t reserved1[5];
	u_int8_t length[2];
	u_int8_t reserved2[3];
};

struct atapi_capacity_list_header {
	u_int8_t reserved[3];
	u_int8_t length;
};

/* codes only valid in the current/maximum capacity descriptor */
#define	ATAPI_CAP_DESC_CODE_MASK	0x3
/*	reserved			0x0 */
#define	ATAPI_CAP_DESC_CODE_UNFORMATTED	0x1
#define	ATAPI_CAP_DESC_CODE_FORMATTED	0x2
#define	ATAPI_CAP_DESC_CODE_NONE	0x3

#define	ATAPI_CAP_DESC_SIZE(n)						\
    (sizeof(struct atapi_capacity_list_header) +			\
    (n) * sizeof(struct scsi_direct_blk_desc))
#define ATAPI_CAP_DESC_OFFSET_HEADER	0
#define ATAPI_CAP_DESC_OFFSET_DESC(n)	ATAPI_CAP_DESC_SIZE(n)

#define ATAPI_FLEX_GEOMETRY_PAGE	0x05
