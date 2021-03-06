/* $Id: sc7816.h,v 1.3 2003/06/02 01:29:37 deraadt Exp $ */

/*
copyright 1997, 2000
the regents of the university of michigan
all rights reserved

permission is granted to use, copy, create derivative works 
and redistribute this software and such derivative works 
for any purpose, so long as the name of the university of 
michigan is not used in any advertising or publicity 
pertaining to the use or distribution of this software 
without specific, written prior authorization.  if the 
above copyright notice or any other identification of the 
university of michigan is included in any copy of any 
portion of this software, then the disclaimer below must 
also be included.

this software is provided as is, without representation 
from the university of michigan as to its fitness for any 
purpose, and without warranty by the university of 
michigan of any kind, either express or implied, including 
without limitation the implied warranties of 
merchantability and fitness for a particular purpose. the 
regents of the university of michigan shall not be liable 
for any damages, including special, indirect, incidental, or 
consequential damages, with respect to any claim arising 
out of or in connection with the use of the software, even 
if it has been or is hereafter advised of the possibility of 
such damages.
*/

/* SCPERF - performance evaluation */
#ifdef SCPERF
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#endif /* SCPERF */

/* open flags - these must track the flags in sectok.h */
#define SCODSR		0x1	/* wait for dsr */
#define SCODCD		0x2	/* wait for dcd */
#define SCOHUP		0x4	/* send signal on card removal */
#define SCOXCTS		0x8	/* wait for no cts (todos reader) */
#define SCOXDTR		0x10	/* invert dtr (todos reader) */
#define SCOINVRT	0x20	/* inverse convention */

/* Reset flags - these must track the flags in sectok.h */
#define SCRV		0x1	/* be verbose */
#define SCRLEN		0x2	/* determine length by examing atr */
#define SCRTODOS	0x4	/* Todos reader */
#define SCRFORCE	0x8	/* Talk to card even if atr is bad */

/* error codes */
#define SCEOK		0
#define SCENOTTY	1	/* no such tty */
#define SCENOMEM	2	/* malloc (or similar) failed */
#define SCTIMEO		3	/* time out */
#define SCESLAG		4	/* slag (no atr) */
#define SCENOSUPP	5	/* card type not supported */
#define SCENOCARD	6	/* no card in reader */
#define SCENOIMPL	7
#define SCEDRVR 	8
#define SCECOMM 	9
#define SCECLOSED	10
#define SCENOFILE       11      /* wrong config path or driver path */
#define SCECNFFILES     12      /* both config path and driver path are
				   specified.  thus conflict. */
#define SCEUNKNOWN	13

extern char *scerrtab[];
extern struct scparam scparam[];

/* forward declarations */

int scopen(int rn, int flags, int *ep);
int scxopen(int rn, int flags, int *ep,
	char *config_path, char *driver_path); 
int scsetflags(int fd, int flags, int mask);
int scrw(int fd, int cla, int ins, int p1, int p2, int ilen, unsigned char *ibuf, int olen, unsigned char *obuf, int *sw1p, int *sw2p);
int scread(int fd, int cla, int ins, int p1, int p2, int p3, unsigned char *buf, int *sw1p, int *sw2p);
int scwrite(int fd, int cla, int ins, int p1, int p2, int p3, unsigned char *buf, int *sw1p, int *sw2p);
int sccardpresent(int fd);
int scdsr(int fd);
int scclose(int fd);
int screset(int fd, unsigned char *atr, int *ep);
int scxreset(int fd, int flags, unsigned char *atr, int *ep);
int scdtr(int fd, int cmd);
int scgetc(int fd, unsigned char *cp, int ms);
int scputc(int fd, int ic);
int scgetblk(int fd, unsigned char *bp, int n, int bwt, int cwt);
int scputblk(int fd, unsigned char *bp, int n);
void scsleep(int ms);
void scdrain(int fd);
int scioT1(int fd, int cla, int ins, int p1, int p2, int ilen, unsigned char *ibuf, int olen, unsigned char *obuf, int *sw1p, int *sw2p);
int scioT1Iblk(int fd, int ilen, unsigned char *ibuf, unsigned char *obuf);
int scioT1pkt(int fd, unsigned char *ibuf, unsigned char *obuf);
int parse_input(char *ibuf, unsigned char *obuf, int olen);
#ifndef __palmos__
int get_input(FILE *f, unsigned char *obuf, int omin, int olen);
int fdump_reply(FILE *f, unsigned char *p, int n, int r1, int r2);
int dump_reply(unsigned char *p, int n, int r1, int r2);
#endif
void print_r1r2(int r1, int r2);
char *get_r1r2s(int r1, int r2);

/* SCPERF - performance evaluation */
#ifdef SCPERF
#ifdef SCPERF_FIRST_APPEARANCE

#define MAX_EVENTS 1024

struct timeval perf_tv[MAX_EVENTS];
char *perf_buf[MAX_EVENTS];
int perf_num = 0; 

void print_time ()
{
    int i;

    for (i = 0 ; i < perf_num ; i ++ ) {
	printf("%ld.%06ld: %s\n",
	    perf_tv[i].tv_sec, perf_tv[i].tv_usec, perf_buf[i]);
    }
    return; 
}

#define SetTime(x) \
  gettimeofday(&(perf_tv[perf_num]), NULL); \
  perf_buf[perf_num] = x; \
  perf_num++; \
  if (perf_num >= MAX_EVENTS) {\
    fprintf (stderr, "SetTime overflow %d\n", MAX_EVENTS); \
    exit (1); \
  }

#else /* !SCPERF_FIRST_APPEARANCE */
extern struct timeval perf_tv[];
extern char *perf_buf[];
extern int perf_num;

#define MAX_EVENTS 1024

#define SetTime(x) \
  gettimeofday(&(perf_tv[perf_num]), NULL); \
  perf_buf[perf_num] = x; \
  perf_num++; \
  if (perf_num >= MAX_EVENTS) {\
    fprintf (stderr, "SetTime overflow %d\n", MAX_EVENTS); \
    exit (1); \
  }
#endif /* SCPERF_FIRST_APPEARANCE */
void print_time (); 
#else /* !SCPERF */
#define SetTime(x)
#define print_time() ;
#endif /* SCPERF */
