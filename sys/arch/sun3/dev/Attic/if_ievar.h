/*	$OpenBSD: if_ievar.h,v 1.5 2002/03/14 01:26:46 millert Exp $	*/
/*	$NetBSD: if_ievar.h,v 1.6 1996/03/26 14:38:33 gwr Exp $	*/

/*
 * Machine-dependent glue for the Intel Ethernet (ie) driver.
 */

#define B_PER_F		3	/* number of buffers to allocate per frame */
#define	MXFRAMES	256	/* max number of frames to allow for receive */
#define	MXRXBUF (MXFRAMES*B_PER_F)	/* max number of buffers to allocate */
#define	IE_RBUF_SIZE	256	/* size of each buffer, MUST BE POWER OF TWO */
#define	NTXBUF		2	/* number of transmit buffer/command pairs */
#define	IE_TBUF_SIZE	(3*512)	/* length of transmit buffer */

enum ie_hardware {
	IE_VME,			/* multibus to VME ie card */
	IE_OBIO,		/* on board */
	IE_VME3E,		/* sun 3e VME card */
	IE_UNKNOWN
};

/*
 * Ethernet status, per interface.
 *
 * hardware addresses/sizes to know (all KVA):
 *   sc_iobase = base of chip's 24 bit address space
 *   sc_maddr  = base address of chip RAM as stored in ie_base of iscp
 *   sc_msize  = size of chip's RAM
 *   sc_reg    = address of card dependent registers
 *
 * the chip uses two types of pointers: 16 bit and 24 bit
 *   16 bit pointers are offsets from sc_maddr/ie_base
 *	KVA(16 bit offset) = offset + sc_maddr
 *   24 bit pointers are offset from sc_iobase in KVA
 *	KVA(24 bit address) = address + sc_iobase
 *
 * on the vme/multibus we have the page map to control where ram appears
 * in the address space.   we choose to have RAM start at 0 in the
 * 24 bit address space.   this means that sc_iobase == sc_maddr!
 * to get the phyiscal address of the board's RAM you must take the
 * top 12 bits of the physical address of the register address
 * and or in the 4 bits from the status word as bits 17-20 (remember that
 * the board ignores the chip's top 4 address lines).
 * For example:
 *   if the register is @ 0xffe88000, then the top 12 bits are 0xffe00000.
 *   to get the 4 bits from the the status word just do status & IEVME_HADDR.
 *   suppose the value is "4".	 Then just shift it left 16 bits to get
 *   it into bits 17-20 (e.g. 0x40000).	   Then or it to get the
 *   address of RAM (in our example: 0xffe40000).   see the attach routine!
 *
 * In the onboard ie interface, the 24 bit address space is hardwired
 * to be 0xff000000 -> 0xffffffff of KVA.   this means that sc_iobase
 * will be 0xff000000.	 sc_maddr will be where ever we allocate RAM
 * in KVA.    note that since the SCP is at a fixed address it means
 * that we have to use some memory at a fixed KVA for the SCP.
 * The Sun PROM leaves a page for us at the end of KVA space.
 */
struct ie_softc {
	struct device sc_dev;	/* device structure */

	struct arpcom sc_arpcom;/* system arpcom structure */
#define	sc_if	sc_arpcom.ac_if			/* network-visible interface */
#define	sc_addr	sc_arpcom.ac_enaddr		/* hardware Ethernet address */

	caddr_t sc_iobase;	/* KVA of base of 24bit addr space */
	caddr_t sc_maddr;	/* KVA of base of chip's RAM */
	u_int	sc_msize;	/* how much RAM we have/use */
	caddr_t sc_reg;		/* KVA of card's register */

	enum	ie_hardware hard_type;	/* card type */
	void	(*reset_586)(struct ie_softc *); /* three card */
	void	(*chan_attn)(struct ie_softc *); /* dependant */
	void	(*run_586)(struct ie_softc *);   /* functions */
	void	(*sc_bcopy)(const void *, void *, u_int);
	void	(*sc_bzero)(void *, u_int);

	int	want_mcsetup;	/* flag for multicast setup */
	int	promisc;	/* are we in promisc mode? */

	/*
	 * pointers to the 3 major control structures
	 */
	volatile struct ie_sys_conf_ptr *scp;
	volatile struct ie_int_sys_conf_ptr *iscp;
	volatile struct ie_sys_ctl_block *scb;

	/*
	 * pointer and size of a block of KVA where the buffers
	 * are to be allocated from
	 */
	caddr_t buf_area;
	int	buf_area_sz;

	/*
	 * the actual buffers (recv and xmit)
	 */
	volatile struct ie_recv_frame_desc *rframes[MXFRAMES];
	volatile struct ie_recv_buf_desc *rbuffs[MXRXBUF];
	volatile char *cbuffs[MXRXBUF];
	int	rfhead, rftail, rbhead, rbtail;

	volatile struct ie_xmit_cmd *xmit_cmds[NTXBUF];
	volatile struct ie_xmit_buf *xmit_buffs[NTXBUF];
	u_char *xmit_cbuffs[NTXBUF];
	int xmit_busy;
	int xmit_free;
	int xchead, xctail;

	struct ie_en_addr mcast_addrs[MAXMCAST + 1];
	int	mcast_count;

	int nframes;	  /* number of frames in use */
	int nrxbuf;	  /* number of recv buffs in use */

#ifdef IEDEBUG
	int	sc_debug;
#endif
};


extern void ie_attach(struct ie_softc *);
extern int  ie_intr(void *);
