/*	$NetBSD: if_epreg.h,v 1.14 1995/11/10 19:39:25 christos Exp $	*/

/*
 * Copyright (c) 1995 Herb Peyerl <hpeyerl@novatel.ca>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Herb Peyerl.
 * 4. The name of Herb Peyerl may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * These define the EEPROM data structure.  They are used in the probe
 * function to verify the existance of the adapter after having sent
 * the ID_Sequence.
 *
 * There are others but only the ones we use are defined here.
 */
#define EEPROM_NODE_ADDR_0	0x0	/* Word */
#define EEPROM_NODE_ADDR_1	0x1	/* Word */
#define EEPROM_NODE_ADDR_2	0x2	/* Word */
#define EEPROM_PROD_ID		0x3	/* 0x9[0-f]50 */
#define EEPROM_MFG_ID		0x7	/* 0x6d50 */
#define EEPROM_ADDR_CFG		0x8	/* Base addr */
#define EEPROM_RESOURCE_CFG	0x9     /* IRQ. Bits 12-15 */

/*
 * These are the registers for the 3Com 3c509 and their bit patterns when
 * applicable.  They have been taken out the the "EtherLink III Parallel
 * Tasking EISA and ISA Technical Reference" "Beta Draft 10/30/92" manual
 * from 3com.
 */
#define EP_COMMAND		0x0e    /* Write. BASE+0x0e is always a command reg. */
#define EP_STATUS		0x0e    /* Read. BASE+0x0e is always status reg. */
#define EP_WINDOW		0x0f    /* Read. BASE+0x0f is always window reg. */

/*
 * Window 0 registers. Setup.
 */
	/* Write */
#define EP_W0_EEPROM_DATA	0x0c
#define EP_W0_EEPROM_COMMAND	0x0a
#define EP_W0_RESOURCE_CFG	0x08
#define EP_W0_ADDRESS_CFG	0x06
#define EP_W0_CONFIG_CTRL	0x04
	/* Read */
#define EP_W0_PRODUCT_ID	0x02
#define EP_W0_MFG_ID		0x00

/*
 * Window 1 registers. Operating Set.
 */
	/* Write */
#define EP_W1_TX_PIO_WR_2	0x02
#define EP_W1_TX_PIO_WR_1	0x00
	/* Read */
#define EP_W1_FREE_TX		0x0c
#define EP_W1_TX_STATUS		0x0b    /* byte */
#define EP_W1_TIMER		0x0a    /* byte */
#define EP_W1_RX_STATUS		0x08
#define EP_W1_RX_PIO_RD_2	0x02
#define EP_W1_RX_PIO_RD_1	0x00

/*
 * Window 2 registers. Station Address Setup/Read
 */
	/* Read/Write */
#define EP_W2_ADDR_5		0x05
#define EP_W2_ADDR_4		0x04
#define EP_W2_ADDR_3		0x03
#define EP_W2_ADDR_2		0x02
#define EP_W2_ADDR_1		0x01
#define EP_W2_ADDR_0		0x00

/* 
 * Window 3 registers.  FIFO Management.
 */
	/* Read */
#define EP_W3_FREE_TX		0x0c
#define EP_W3_FREE_RX		0x0a

/*
 * Window 4 registers. Diagnostics.
 */
	/* Read/Write */
#define EP_W4_MEDIA_TYPE	0x0a
#define EP_W4_CTRLR_STATUS	0x08
#define EP_W4_NET_DIAG		0x06
#define EP_W4_FIFO_DIAG		0x04
#define EP_W4_HOST_DIAG		0x02
#define EP_W4_TX_DIAG		0x00

/*
 * Window 5 Registers.  Results and Internal status.
 */
	/* Read */
#define EP_W5_READ_0_MASK	0x0c
#define EP_W5_INTR_MASK		0x0a
#define EP_W5_RX_FILTER		0x08
#define EP_W5_RX_EARLY_THRESH	0x06
#define EP_W5_TX_AVAIL_THRESH	0x02
#define EP_W5_TX_START_THRESH	0x00

/*
 * Window 6 registers. Statistics.
 */
	/* Read/Write */
#define TX_TOTAL_OK		0x0c
#define RX_TOTAL_OK		0x0a
#define TX_DEFERRALS		0x08
#define RX_FRAMES_OK		0x07
#define TX_FRAMES_OK		0x06
#define RX_OVERRUNS		0x05
#define TX_COLLISIONS		0x04
#define TX_AFTER_1_COLLISION	0x03
#define TX_AFTER_X_COLLISIONS	0x02
#define TX_NO_SQE		0x01
#define TX_CD_LOST		0x00

/*
 * Register definitions.
 */

/*
 * Command register. All windows.
 *
 * 16 bit register.
 *     15-11:  5-bit code for command to be executed.
 *     10-0:   11-bit arg if any. For commands with no args;
 *	      this can be set to anything.
 */
#define GLOBAL_RESET		(u_short) 0x0000   /* Wait at least 1ms after issuing */
#define WINDOW_SELECT		(u_short) (0x1<<11)
#define START_TRANSCEIVER	(u_short) (0x2<<11) /* Read ADDR_CFG reg to determine
						      whether this is needed. If so;
						      wait 800 uSec before using trans-
						      ceiver. */
#define RX_DISABLE		(u_short) (0x3<<11) /* state disabled on power-up */
#define RX_ENABLE		(u_short) (0x4<<11)
#define RX_RESET		(u_short) (0x5<<11)
#define RX_DISCARD_TOP_PACK	(u_short) (0x8<<11)
#define TX_ENABLE		(u_short) (0x9<<11)
#define TX_DISABLE		(u_short) (0xa<<11)
#define TX_RESET		(u_short) (0xb<<11)
#define REQ_INTR		(u_short) (0xc<<11)

/*
 * The following C_* acknowledge the various interrupts.
 * Some of them don't do anything.  See the manual.
 */
#define ACK_INTR		(u_short) (0x6800)
#      define C_INTR_LATCH	(u_short) (ACK_INTR|0x01)
#      define C_CARD_FAILURE	(u_short) (ACK_INTR|0x02)
#      define C_TX_COMPLETE	(u_short) (ACK_INTR|0x04)
#      define C_TX_AVAIL	(u_short) (ACK_INTR|0x08)
#      define C_RX_COMPLETE	(u_short) (ACK_INTR|0x10)
#      define C_RX_EARLY	(u_short) (ACK_INTR|0x20)
#      define C_INT_RQD		(u_short) (ACK_INTR|0x40)
#      define C_UPD_STATS	(u_short) (ACK_INTR|0x80)
#define SET_INTR_MASK		(u_short) (0x0e<<11)
#define SET_RD_0_MASK		(u_short) (0x0f<<11)
#define SET_RX_FILTER		(u_short) (0x10<<11)
#      define FIL_INDIVIDUAL	(u_short) (0x01)
#      define FIL_MULTICAST	(u_short) (0x02)
#      define FIL_BRDCST	(u_short) (0x04)
#      define FIL_PROMISC	(u_short) (0x08)
#define SET_RX_EARLY_THRESH	(u_short) (0x11<<11)
#define SET_TX_AVAIL_THRESH	(u_short) (0x12<<11)
#define SET_TX_START_THRESH	(u_short) (0x13<<11)
#define STATS_ENABLE		(u_short) (0x15<<11)
#define STATS_DISABLE		(u_short) (0x16<<11)
#define STOP_TRANSCEIVER	(u_short) (0x17<<11)

/*
 * Status register. All windows.
 *
 *     15-13:  Window number(0-7).
 *     12:     Command_in_progress.
 *     11:     reserved.
 *     10:     reserved.
 *     9:      reserved.
 *     8:      reserved.
 *     7:      Update Statistics.
 *     6:      Interrupt Requested.
 *     5:      RX Early.
 *     4:      RX Complete.
 *     3:      TX Available.
 *     2:      TX Complete.
 *     1:      Adapter Failure.
 *     0:      Interrupt Latch.
 */
#define S_INTR_LATCH		(u_short) (0x0001)
#define S_CARD_FAILURE		(u_short) (0x0002)
#define S_TX_COMPLETE		(u_short) (0x0004)
#define S_TX_AVAIL		(u_short) (0x0008)
#define S_RX_COMPLETE		(u_short) (0x0010)
#define S_RX_EARLY		(u_short) (0x0020)
#define S_INT_RQD		(u_short) (0x0040)
#define S_UPD_STATS		(u_short) (0x0080)
#define S_COMMAND_IN_PROGRESS	(u_short) (0x1000)

/*
 * FIFO Registers.  RX Status.
 *
 *     15:     Incomplete or FIFO empty.
 *     14:     1: Error in RX Packet   0: Incomplete or no error.
 *     14-11:  Type of error. [14-11]
 *	      1000 = Overrun.
 *	      1011 = Run Packet Error.
 *	      1100 = Alignment Error.
 *	      1101 = CRC Error.
 *	      1001 = Oversize Packet Error (>1514 bytes)
 *	      0010 = Dribble Bits.
 *	      (all other error codes, no errors.)
 *
 *     10-0:   RX Bytes (0-1514)
 */
#define ERR_INCOMPLETE  (u_short) (0x8000)
#define ERR_RX		(u_short) (0x4000)
#define ERR_MASK	(u_short) (0x7800)
#define ERR_OVERRUN	(u_short) (0x4000)
#define ERR_RUNT	(u_short) (0x5800)
#define ERR_ALIGNMENT	(u_short) (0x6000)
#define ERR_CRC		(u_short) (0x6800)
#define ERR_OVERSIZE	(u_short) (0x4800)
#define ERR_DRIBBLE	(u_short) (0x1000)

/*
 * TX Status
 *
 *   Reports the transmit status of a completed transmission. Writing this
 *   register pops the transmit completion stack.
 *
 *   Window 1/Port 0x0b.
 *
 *     7:      Complete
 *     6:      Interrupt on successful transmission requested.
 *     5:      Jabber Error (TP Only, TX Reset required. )
 *     4:      Underrun (TX Reset required. )
 *     3:      Maximum Collisions.
 *     2:      TX Status Overflow.
 *     1-0:    Undefined.
 *
 */
#define TXS_COMPLETE		0x80
#define TXS_INTR_REQ		0x40
#define TXS_JABBER		0x20
#define TXS_UNDERRUN		0x10
#define TXS_MAX_COLLISION	0x08
#define TXS_STATUS_OVERFLOW	0x04

/*
 * FIFO Status (Window 4)
 *
 *   Supports FIFO diagnostics
 *
 *   Window 4/Port 0x04.1
 *
 *     15:	1=RX receiving (RO). Set when a packet is being received
 *		into the RX FIFO.
 *     14:	Reserved
 *     13:	1=RX underrun (RO). Generates Adapter Failure interrupt.
 *		Requires RX Reset or Global Reset command to recover.
 *		It is generated when you read past the end of a packet -
 *		reading past what has been received so far will give bad
 *		data.
 *     12:	1=RX status overrun (RO). Set when there are already 8
 *		packets in the RX FIFO. While this bit is set, no additional
 *		packets are received. Requires no action on the part of
 *		the host. The condition is cleared once a packet has been
 *		read out of the RX FIFO.
 *     11:	1=RX overrun (RO). Set when the RX FIFO is full (there
 *		may not be an overrun packet yet). While this bit is set,
 *		no additional packets will be received (some additional
 *		bytes can still be pending between the wire and the RX
 *		FIFO). Requires no action on the part of the host. The
 *		condition is cleared once a few bytes have been read out
 *		from the RX FIFO.
 *     10:	1=TX overrun (RO). Generates adapter failure interrupt.
 *		Requires TX Reset or Global Reset command to recover.
 *		Disables Transmitter.
 *     9-8:	Unassigned.
 *     7-0:	Built in self test bits for the RX and TX FIFO's.
 */
#define	FIFOS_RX_RECEIVING	(u_short) 0x8000
#define	FIFOS_RX_UNDERRUN	(u_short) 0x2000
#define	FIFOS_RX_STATUS_OVERRUN	(u_short) 0x1000
#define	FIFOS_RX_OVERRUN	(u_short) 0x0800
#define	FIFOS_TX_OVERRUN	(u_short) 0x0400

/*
 * Misc defines for various things.
 */
#define TAG_ADAPTER 			0xd0
#define ACTIVATE_ADAPTER_TO_CONFIG 	0xff
#define ENABLE_DRQ_IRQ			0x0001
#define MFG_ID				0x506d	/* `TCM' */
#define PROD_ID				0x5090
#define BASE 				sc->ep_iobase
#define GO_WINDOW(x) 			outw(BASE+EP_COMMAND, WINDOW_SELECT|x)
#define AUI 				0x1
#define BNC 				0x2
#define UTP 				0x4
#define IS_AUI 				(1<<13)
#define IS_BNC 				(1<<12)
#define IS_UTP 				(1<<9)
#define EEPROM_BUSY			(1<<15)
#define EEPROM_TST_MODE			(1<<14)
#define READ_EEPROM			(1<<7)
#define ENABLE_UTP			0xc0
#define DISABLE_UTP			0x0
#define RX_BYTES_MASK			(u_short) (0x07ff)

#define IS_PCI_AUI 			(1<<5)
#define IS_PCI_BNC 			(1<<4)
#define IS_PCI_UTP 			(1<<3)

/*
 * EISA registers (offset from slot base)
 */
#define	EISA_VENDOR		0x0c80	/* vendor ID (2 ports) */
#define	EISA_MODEL		0x0c82	/* model number (2 ports) */
#define	EISA_CONTROL		0x0c84
#define	 EISA_RESET		0x04
#define	 EISA_ERROR		0x02
#define	 EISA_ENABLE		0x01
