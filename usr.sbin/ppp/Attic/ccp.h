/*
 *	    Written by Toshiharu OHNO (tony-o@iij.ad.jp)
 *
 *   Copyright (C) 1993, Internet Initiative Japan, Inc. All rights reserverd.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the Internet Initiative Japan.  The name of the
 * IIJ may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * $Id: ccp.h,v 1.6 1998/06/28 09:41:35 brian Exp $
 *
 *	TODO:
 */

#define	CCP_MAXCODE	CODE_RESETACK

#define	TY_OUI		0	/* OUI */
#define	TY_PRED1	1	/* Predictor type 1 */
#define	TY_PRED2	2	/* Predictor type 2 */
#define	TY_PUDDLE	3	/* Puddle Jumper */
#define	TY_HWPPC	16	/* Hewlett-Packard PPC */
#define	TY_STAC		17	/* Stac Electronics LZS */
#define	TY_MSPPC	18	/* Microsoft PPC */
#define	TY_GAND		19	/* Gandalf FZA */
#define	TY_V42BIS	20	/* V.42bis compression */
#define	TY_BSD		21	/* BSD LZW Compress */
#define	TY_PPPD_DEFLATE	24	/* Deflate (gzip) - (mis) numbered by pppd */
#define	TY_DEFLATE	26	/* Deflate (gzip) - rfc 1979 */

struct ccpstate {
  int his_proto;		/* peer's compression protocol */
  int my_proto;			/* our compression protocol */

  int reset_sent;		/* If != -1, ignore compressed 'till ack */
  int last_reset;		/* We can receive more (dups) w/ this id */

  u_int32_t his_reject;		/* Request codes rejected by peer */
  u_int32_t my_reject;		/* Request codes I have rejected */

  int out_init;			/* Init called for out algorithm */
  int in_init;			/* Init called for in algorithm */

  u_long uncompout, compout;	/* Outgoing bytes before/after compression */
  u_long uncompin, compin;	/* Incoming bytes after/before decompression */
};

extern struct ccpstate CcpInfo;

struct ccp_algorithm {
  int id;
  int Conf;					/* A Conf value from vars.h */
  const char *(*Disp)(struct lcp_opt *);
  struct {
    void (*Get)(struct lcp_opt *);
    int (*Set)(struct lcp_opt *);
    int (*Init)(void);
    void (*Term)(void);
    void (*Reset)(void);
    struct mbuf *(*Read)(u_short *, struct mbuf *);
    void (*DictSetup)(u_short, struct mbuf *);
  } i;
  struct {
    void (*Get)(struct lcp_opt *);
    int (*Set)(struct lcp_opt *);
    int (*Init)(void);
    void (*Term)(void);
    void (*Reset)(void);
    int (*Write)(int, u_short, struct mbuf *);
  } o;
};

extern struct fsm CcpFsm;

extern void CcpRecvResetReq(struct fsm *);
extern void CcpSendResetReq(struct fsm *);
extern void CcpInput(struct mbuf *);
extern void CcpUp(void);
extern void CcpOpen(void);
extern void CcpInit(void);
extern int ReportCcpStatus(struct cmdargs const *);
extern void CcpResetInput(u_char);
extern int CcpOutput(int, u_short, struct mbuf *);
extern struct mbuf *CompdInput(u_short *, struct mbuf *);
extern void CcpDictSetup(u_short, struct mbuf *);
