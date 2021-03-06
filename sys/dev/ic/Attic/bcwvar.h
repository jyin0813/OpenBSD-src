/*	$OpenBSD: bcwvar.h,v 1.37 2007/04/04 19:36:42 mglocker Exp $ */

/*
 * Copyright (c) 2007 Marcus Glocker <mglocker@openbsd.org>
 * Copyright (c) 2006 Jon Simola <jsimola@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Broadcom BCM43xx Wireless network chipsets (broadcom.com)
 * SiliconBackplane is technology from Sonics, Inc. (sonicsinc.com)
 */

#define BCW_ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

#define BCW_FLAGS_INITIALIZED		0x00000001

#define BCW_DEFAULT_SHORT_RETRY_LIMIT	7
#define BCW_DEFAULT_LONG_RETRY_LIMIT	4

struct fwheader {
	char	filename[64];
	int	filesize;
	int	fileoffset;
};

struct bcw_initval {
        uint16_t	offset;
        uint16_t	size;
        uint32_t	value;
} __packed;

#define BCW_NR_LEDS	4
struct bcw_led {
	uint8_t		behaviour:7;
	uint8_t		activelow:1;
	unsigned long	blink_interval;
};
enum {
	BCW_LED_OFF,
	BCW_LED_ON,
	BCW_LED_ACTIVITY,
	BCW_LED_RADIO_ALL,
	BCW_LED_RADIO_A,
	BCW_LED_RADIO_B,
	BCW_LED_MODE_BG,
	BCW_LED_TRANSFER,
	BCW_LED_APTRANSFER,
	BCW_LED_WEIRD, /* XXX */
	BCW_LED_ASSOC,
	BCW_LED_INACTIVE,

	/*
	 * Behaviour values for testing.
	 * With these values it is easier to figure out
	 * the real behaviour of leds, in case the SPROM
	 * is missing information.
	 */
	BCW_LED_TEST_BLINKSLOW,
	BCW_LED_TEST_BLINKMEDIUM,
	BCW_LED_TEST_BLINKFAST,

	/* misc values for BCM4303 */
	BCW_LED_BCM4303_0 = 0x2B,
	BCW_LED_BCM4303_1 = 0x78,
	BCW_LED_BCM4303_2 = 0x2E,
	BCW_LED_BCM4303_3 = 0x19,
};

#define BCW_RADIO_TXANTENNA_LASTPLCP	3
#define BCW_RADIO_TXANTENNA_DEFAULT	BCW_RADIO_TXANTENNA_LASTPLCP
#define BCW_RADIO_INTERFMODE_NONE	0
#define BCW_RADIO_INTERFMODE_NONWLAN	1
#define BCW_RADIO_INTERFMODE_MANUALWLAN	2
#define BCW_RADIO_INTERFMODE_AUTOWLAN	3
#define BCW_RADIO_DEFAULT_CHANNEL_A	36
#define BCW_RADIO_DEFAULT_CHANNEL_BG	6
#define BCW_RADIO_MAX			2
struct bcw_radio {
	uint16_t	id;
	uint16_t	info;
	u_char		enabled;
};

#define BCW_MAX_CORES		10
struct bcw_core {
	uint16_t	id;
	uint8_t		index;
	uint8_t		rev;
	uint8_t		backplane_flag;
};

/* number of descriptors used in a ring */
#define BCW_RX_RING_COUNT	128
#define BCW_TX_RING_COUNT	128
#define BCW_MAX_SCATTER		8	/* XXX unknown, wild guess */

struct bcw_rx_data {
	bus_dmamap_t		map;
	struct mbuf		*m;
};

struct bcw_tx_data {
	bus_dmamap_t		map;
	struct mbuf		*m;
	uint32_t		softstat;
	struct ieee80211_node	*ni;
};

struct bcw_rx_ring {
	bus_dmamap_t		map;
	bus_dma_segment_t	seg;
	bus_addr_t		physaddr;
	struct bcw_desc		*desc;
	struct bcw_rx_data	*data;
	int			count;
	int			cur;
	int			next;
};

struct bcw_tx_ring {
	bus_dmamap_t		map;
	bus_dma_segment_t	seg;
	bus_addr_t		physaddr;
	struct bcw_desc		*desc;
	struct bcw_tx_data	*data;
	int			count;
	int			queued;
	int			cur;
	int			next;
	int			stat;
};

struct bcw_desc {
	uint32_t	ctrl;
	uint32_t	addr;
};

/* ring descriptor */
struct bcw_dma_slot {
	uint32_t	ctrl;
	uint32_t	addr;
};

struct bcw_lopair {
	int8_t low;
	int8_t high;
	uint8_t used:1;
};
#define BCW_LO_COUNT	(14 * 4)

#define CTRL_BC_MASK	0x1fff		/* buffer byte count */
#define CTRL_EOT	0x10000000	/* end of descriptor table */
#define CTRL_IOC	0x20000000	/* interrupt on completion */
#define CTRL_EOF	0x40000000	/* end of frame */
#define CTRL_SOF	0x80000000	/* start of frame */

/* radio */
#define BCW_INTERFSTACK_SIZE		26
                
/* ilt */
#define BCW_ILT_FINEFREQA_SIZE		256
#define BCW_ILT_FINEFREQG_SIZE		256
#define BCW_ILT_NOISEA2_SIZE		8
#define BCW_ILT_NOISEA3_SIZE		8 
#define BCW_ILT_NOISEG1_SIZE		8
#define BCW_ILT_NOISEG2_SIZE		8
#define BCW_ILT_ROTOR_SIZE		53
#define BCW_ILT_RETARD_SIZE		53
#define BCW_ILT_SIGMASQR_SIZE		53
#define BCW_ILT_NOISESCALEG_SIZE	27

#if 0
/*
 * Mbuf pointers. We need these to keep track of the virtual addresses   
 * of our mbuf chains since we can only convert from physical to virtual,
 * not the other way around.
 *
 * The chip has 6 DMA engines, looks like we only need to use one each
 * for TX and RX, the others stay disabled.
 */
struct bcw_chain_data {
	struct mbuf    *bcw_tx_chain[BCW_NTXDESC];
	struct mbuf    *bcw_rx_chain[BCW_NRXDESC];
	bus_dmamap_t    bcw_tx_map[BCW_NTXDESC];  
	bus_dmamap_t    bcw_rx_map[BCW_NRXDESC];
};
#endif

struct bcw_rx_ring;
struct bcw_tx_ring;

#define BCW_SPROM_SIZE			64	/* in 16-bit words */
struct bcw_sprom {
	uint16_t	boardflags2;
	uint8_t		il0macaddr[6];
	uint8_t		et0macaddr[6];
	uint8_t		et1macaddr[6];
	uint8_t		et1phyaddr:5;
	uint8_t		et0phyaddr:5;
	uint8_t		et0mdcport:1;
	uint8_t		et1mdcport:1;
	uint8_t		boardrev;
	uint8_t		locale:4;
	uint8_t		antennas_aphy:2;
	uint8_t		antennas_bgphy:2;
	uint16_t	pa0b0;
	uint16_t	pa0b1;
	uint16_t	pa0b2;
	uint8_t		wl0gpio0;
	uint8_t		wl0gpio1;
	uint8_t		wl0gpio2;
	uint8_t		wl0gpio3;
	uint8_t		maxpower_aphy;
	uint8_t		maxpower_bgphy;
	uint16_t	pa1b0;
	uint16_t	pa1b1;
	uint16_t	pa1b2;
	uint8_t		idle_tssi_tgt_aphy;
	uint8_t		idle_tssi_tgt_bgphy;
	uint16_t	boardflags;
	uint16_t	antennagain_aphy;
	uint16_t	antennagain_bgphy;
};

struct bcw_softc {
	struct device		 sc_dev;
	struct ieee80211com	 sc_ic;
	struct bcw_rx_ring	 sc_rxring;
	struct bcw_tx_ring	 sc_txring;
	/* function pointers */
	int			 (*sc_newstate)(struct ieee80211com *,
				     enum ieee80211_state, int);
	int			 (*sc_enable)(struct bcw_softc *);
	void			 (*sc_disable)(struct bcw_softc *);
	void			 (*sc_power)(struct bcw_softc *, int);
	void			 (*sc_conf_write)(void *, uint32_t, uint32_t);
	uint32_t		 (*sc_conf_read)(void *, uint32_t);

	bus_dma_tag_t		 sc_dmat;
	bus_space_tag_t		 sc_iot;
	bus_space_handle_t	 sc_ioh;

	void			*sc_dev_softc;
	uint32_t		 sc_flags;
	uint32_t		 sc_intmask;		/* current intr mask */
	uint32_t		 sc_rxin;		/* last rx desc seen */
	uint32_t		 sc_txin;		/* last tx desc seen */
	int			 sc_txsfree;		/* no. tx slots avail */
	int			 sc_txsnext;		/* next avail tx slot */
	bus_dmamap_t		 sc_ring_map;
	struct bcw_dma_slot	*bcw_rx_ring;		/* receive ring */
	struct bcw_dma_slot	*bcw_tx_ring;		/* transmit ring */
	struct timeout		 sc_timeout;
	struct timeout		 sc_scan_to;
	//struct bcw_chain_data	 sc_cdata;		/* mbufs */

	uint16_t		 sc_board_vendor;	/* Board vendor */
	uint16_t		 sc_board_type;		/* Board type */
	uint16_t		 sc_board_rev;		/* Board revision */
	uint16_t		 sc_chip_id;		/* Chip ID */
	uint16_t		 sc_chip_rev;		/* Chip revision */
	uint16_t		 sc_chip_pkg;		/* Chip package */
	uint16_t		 sc_prodid;		/* Product ID */
	uint8_t			 sc_idletssi;
	uint16_t		 sc_boardflags;
	uint16_t		 sc_using_pio:1;
	struct bcw_sprom	 sc_sprom;
	struct bcw_led		 leds[BCW_NR_LEDS];
	/* cores */
	uint16_t		 sc_numcores;
	uint16_t		 sc_havecommon;
	int			 sc_currentcore;
	int			 sc_lastcore;
	uint32_t		 sc_chip_common_capa;
	struct bcw_core		 sc_core[BCW_MAX_CORES];
	struct bcw_core		*sc_core_common;
	struct bcw_core		*sc_core_80211;
	struct bcw_core		*sc_core_bus;		/* PCI or cardbus */
	/* PHY */
	uint16_t		 sc_phy_ver;
	uint16_t		 sc_phy_rev;
	uint16_t		 sc_phy_type;
	uint8_t			 sc_phy_connected:1,	/* XXX */
				     sc_phy_calibrated:1,
				     sc_phy_is_locked:1,
				     sc_phy_dyn_tssi_tbl:1;
	uint16_t		 sc_phy_loopback_gain[2];
	uint16_t		 sc_phy_savedpctlreg;
	uint16_t		 sc_phy_minlowsig[2];
	uint16_t		 sc_phy_minlowsigpos[2];
	int8_t			 sc_phy_idle_tssi;
	const int8_t		 sc_phy_tssi2dbm;
	uint16_t		 sc_phy_antenna_diversity;
	struct bcw_lopair	*sc_phy_lopairs;
	/* radio */
	uint16_t		 sc_radio_ver;
	uint16_t		 sc_radio_rev;
	uint32_t		 sc_radio_mnf;
	uint16_t		 sc_radio_initval;
	int16_t			 sc_radio_nrssi[2];
	int			 sc_radio_interfmode;
	uint8_t			 sc_radio_aci_enable:1,
				     sc_radio_aci_wlan_automatic:1,
				     sc_radio_aci_hw_rssi;
	int32_t			 sc_radio_nrssislope;
	uint16_t		 sc_radio_txpwr_offset;
	uint16_t		 sc_radio_lofcal;
	uint16_t		 sc_radio_txpower_desired;
	uint16_t		 sc_radio_pa0b0;
	uint16_t		 sc_radio_pa0b1;
	uint16_t		 sc_radio_pa0b2;
	uint16_t		 sc_radio_pa1b0;
	uint16_t		 sc_radio_pa1b1;
	uint16_t		 sc_radio_pa1b2;
	uint16_t		 sc_radio_baseband_atten;
	uint16_t		 sc_radio_radio_atten;
	uint16_t		 sc_radio_txctl1;
	uint16_t		 sc_radio_txctl2;
	uint8_t			 sc_radio_channel;
	int8_t			 sc_radio_nrssi_lt[64];
	uint32_t		 sc_radio_interfstack[BCW_INTERFSTACK_SIZE];
	//struct bcw_radio	 radio[BCW_RADIO_MAX];
};

void	bcw_attach(struct bcw_softc *);
int	bcw_detach(void *);
int	bcw_intr(void *);
void	bcw_powercontrol_crystal_on(struct bcw_softc *);

#define BCW_DEBUG
#ifdef BCW_DEBUG
//#define DPRINTF(x)	do { if (bcw_debug) printf x; } while (0)
//#define DPRINTFN(n,x)	do { if (bcwdebug >= (n)) printf x; } while (0)
#define DPRINTF(x)	do { if (1) printf x; } while (0)
#define DPRINTFN(n,x)	do { if (1 >= (n)) printf x; } while (0)
//int bcw_debug = 99;
#else
#define DPRINTF(x)
#define DPRINTFN(n,x)
#endif

/*
 * Some legacy stuff from bce and iwi to make this compile
 */
/* transmit buffer max frags allowed */
#define BCW_NTXFRAGS	16

/* Packet status is returned in a pre-packet header */
struct rx_pph {
	uint16_t	len;
	uint16_t	flags;
	uint16_t	pad[12];
};

#define BCW_PREPKT_HEADER_SIZE		30

/* packet status flags bits */
#define RXF_NO				0x8	/* odd number of nibbles */
#define RXF_RXER			0x4	/* receive symbol error */
#define RXF_CRC				0x2	/* crc error */
#define RXF_OV				0x1	/* fifo overflow */

#define BCW_TIMEOUT			100	/* # 10us for mii read/write */

/* for ring descriptors */
#define BCW_RXBUF_LEN			(MCLBYTES - 4)
#define BCW_INIT_RXDESC(sc, x)						\
do {									\
	struct bcw_dma_slot *__bcwd = &sc->bcw_rx_ring[x];		\
									\
	*mtod(sc->bcw_cdata.bcw_rx_chain[x], uint32_t *) = 0;		\
	__bcwd->addr =							\
	    htole32(sc->bcw_cdata.bcw_rx_map[x]->dm_segs[0].ds_addr	\
	    + 0x40000000);						\
	if (x != (BCW_NRXDESC - 1))					\
		__bcwd->ctrl = htole32(BCW_RXBUF_LEN);			\
	else								\
		__bcwd->ctrl = htole32(BCW_RXBUF_LEN | CTRL_EOT);	\
	bus_dmamap_sync(sc->bcw_dmatag, sc->bcw_ring_map,		\
	    sizeof(struct bcw_dma_slot) * x,				\
	    sizeof(struct bcw_dma_slot),				\
	    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);			\
} while (/* CONSTCOND */ 0)

#define BCW_NTXFRAGS   16
