/*	$OpenBSD: handle_value_request.c,v 1.9 2002/06/10 19:58:20 espie Exp $	*/

/*
 * Copyright 1997-2000 Niels Provos <provos@citi.umich.edu>
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
 *      This product includes software developed by Niels Provos.
 * 4. The name of the author may not be used to endorse or promote products
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
 */
/*
 * handle_value_request:
 * receive a VALUE_REQUEST packet; return (-1) on failure, 0 on success
 *
 */

#ifndef lint
static char rcsid[] = "$OpenBSD: handle_value_request.c,v 1.9 2002/06/10 19:58:20 espie Exp $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ssl/bn.h>
#include "config.h"
#include "photuris.h"
#include "packets.h"
#include "state.h"
#include "cookie.h"
#include "config.h"
#include "buffer.h"
#include "scheme.h"
#include "packet.h"
#include "exchange.h"
#include "secrets.h"
#include "server.h"
#include "log.h"

int
handle_value_request(u_char *packet, int size,
		     char *address, u_short port,
		     u_int8_t *schemes, u_int16_t ssize)

{
        struct packet_sub parts[] = {
	     { "Exchange Value", FLD_VARPRE, 0, 0, },
	     { "Offered Attributes", FLD_ATTRIB, FMD_ATT_FILL, 0, },
	     { NULL }
	};
	struct packet vr_msg = {
	     "Value Request",
	     VALUE_REQUEST_MIN, 0, parts
	};
	struct value_request *header;
	struct stateob *st;
	BIGNUM *test, *gen, *mod;
	u_int8_t *p, *modp, *refp, *genp = NULL;
	size_t sstart, vsize, modsize, modpsize, refpsize;
	int modflag;
	u_int8_t scheme_ref[2];
	u_int8_t rcookie[COOKIE_SIZE];

	if (size < VALUE_REQUEST_MIN)
	     return (-1);	/* packet too small  */

	if (packet_check(packet, size, &vr_msg) == -1) {
	     log_print("bad packet structure in handle_value_request()");
	     return (-1);
	}

	header = (struct value_request *) packet;

	st = state_find_cookies(address, header->icookie, header->rcookie);
	if (st == NULL) {
	     struct stateob tempst;
	     bzero((char *)&tempst, sizeof(tempst)); /* Set up temp. state */
	     tempst.initiator = 0;                   /* We are the Responder */
	     bcopy(header->icookie, tempst.icookie, COOKIE_SIZE);
	     strncpy(tempst.address, address, 15);
	     tempst.port = global_port;
	     tempst.counter = header->counter;
	
	     cookie_generate(&tempst, rcookie, COOKIE_SIZE, schemes, ssize);

	     /* Check for invalid cookie */
	     if (bcmp(rcookie, header->rcookie, COOKIE_SIZE)) {
		  packet_size = PACKET_BUFFER_SIZE;
		  photuris_error_message(&tempst, packet_buffer, &packet_size,
					 header->icookie, header->rcookie,
					 header->counter, BAD_COOKIE);
		  send_packet();
		  return (0);
	     }

	     /* Check exchange value - XXX doesn't check long form */
	     vsize = parts[0].size;

	     /* Check schemes - selected length is in exchange value*/
	     sstart = 0;
	     modflag = 0;
	     refp = modp = NULL;
	     *(u_int16_t *)scheme_ref = htons(scheme_get_ref(header->scheme));
	     while (sstart < ssize) {
		  p = scheme_get_mod(schemes + sstart);
		  modsize = varpre2octets(p);
		  if (!bcmp(header->scheme, schemes + sstart, 2)) {
		       modflag = 1;
		       if (modsize == vsize) {
			    genp = scheme_get_gen(schemes+sstart);
			    modp = p;
			    modpsize = modsize;
			    break;  /* On right scheme + right size */
		       } else if (modsize <= 2 && refp != NULL) {
			    modp = refp;
			    modpsize = refpsize;
			    break;
		       }
		  } else if (!bcmp(scheme_ref, schemes + sstart, 2) &&
			     modsize == vsize) {
		       genp = scheme_get_gen(schemes + sstart);
		       if (modflag) {
			    modp = p;
			    modpsize = modsize;
			    break;
		       }
		       refp = p;
		       refpsize = modsize;
		  }
		
		  sstart += scheme_get_len(schemes+sstart);
	     }
	     if (sstart >= ssize)
		  return (-1);   /* Did not find a scheme - XXX log */

	     /* now check the exchange value */
	     test = BN_new();
	     if (BN_varpre2bn(parts[0].where, parts[0].size, test) == NULL) {
		     BN_free(test);
		     return (-1);
	     }

	     mod = BN_new();
	     if (BN_varpre2bn(modp, modpsize, mod) == NULL) {
		     BN_free(test);
		     BN_free(mod);
		     return (-1);
	     }

	     gen = BN_new();
	     if (exchange_set_generator(gen, header->scheme, genp) == -1 ||
		 !exchange_check_value(test, gen, mod)) {
		  BN_free(test);
		  BN_free(gen);
		  BN_free(mod);
		  return 0;
	     }
	     BN_free(test);
	     BN_free(gen);
	     BN_free(mod);

	     if ((st = state_new()) == NULL)
		     goto resourcefail;

	     /* Default options */
	     st->flags = IPSEC_OPT_ENC|IPSEC_OPT_AUTH;

	     /* Fill the state object */
	     st->uSPIoattrib = calloc(parts[1].size, sizeof(u_int8_t));
             if (st->uSPIoattrib == NULL) {
                  state_value_reset(st);
		  goto resourcefail;
	     }
             bcopy(parts[1].where, st->uSPIoattrib, parts[1].size);
             st->uSPIoattribsize = parts[1].size;

	     /* Save scheme, which will be used by both parties */
	     vsize = 2 + varpre2octets(modp);

	     /* XXX - VPN - only support two octets */
	     if (genp != NULL)
		  vsize += 2 + varpre2octets(genp);

	     st->scheme = calloc(vsize, sizeof(u_int8_t));
	     if (st->scheme == NULL) {
                  state_value_reset(st);
                  goto resourcefail;
             }
             bcopy(header->scheme, st->scheme, 2);
	     if (genp != NULL) {
		  st->scheme[2] = (vsize-4) >> 8;
		  st->scheme[3] = (vsize-4) & 0xFF;
		  bcopy(genp, st->scheme+2+2, varpre2octets(genp));
	     }
	     bcopy(modp, st->scheme + 2 + (genp == NULL ? 0 : 2 + varpre2octets(genp)),
		   varpre2octets(modp));;
		
             st->schemesize = vsize;

#ifdef DEBUG
	     {
		  int i = BUFFER_SIZE;
		  bin2hex(buffer, &i, parts[0].where, varpre2octets(VALUE_REQUEST_VALUE(header)));
		  printf("Got exchange value 0x%s\n", buffer);
	     }
#endif

	     /* Set exchange value */
	     st->texchangesize = parts[0].size;
	     st->texchange = calloc(st->texchangesize, sizeof(u_int8_t));
	     if (st->texchange == NULL) {
		  log_error("calloc() in handle_value_request()");
		  state_value_reset(st);
		  goto resourcefail;
	     }
	     bcopy(parts[0].where, st->texchange, st->texchangesize);


	     /* Fill in the state object with generic data */
             strncpy(st->address, address, 15);
             st->port = port;
	     st->counter = header->counter;
             bcopy(header->icookie, st->icookie, COOKIE_SIZE);
             bcopy(header->rcookie, st->rcookie, COOKIE_SIZE);
	     bcopy(&header->counter, st->uSPITBV, 3);

	     if ((st->roschemes = calloc(ssize, sizeof(u_int8_t))) == NULL) {
		  log_error("calloc() in handle_value_request()");
		  state_value_reset(st);
		  goto resourcefail;
	     }
	     bcopy(schemes, st->roschemes, ssize);
	     st->roschemesize = ssize;

	     if (pick_attrib(st, &(st->oSPIoattrib),
			     &(st->oSPIoattribsize)) == -1) {
		  state_value_reset(st);
		  goto resourcefail;
	     }

	     st->lifetime = exchange_timeout + time(NULL);

	     /* Now put the filled state object in the chain */
	     state_insert(st);
	} else if (st->phase != VALUE_RESPONSE) {
		LOG_DBG((LOG_PROTOCOL, 55, 
			 "%s: value request from %s, but we are in state %d",
			 __func__,
			 st->address, st->phase));
		return (-1);
	}
	
	packet_size = PACKET_BUFFER_SIZE;
	if (photuris_value_response(st, packet_buffer, &packet_size) == -1)
	     return (-1);

	send_packet();

        /* Compute the shared secret now */
        compute_shared_secret(st, &(st->shared), &(st->sharedsize));
#ifdef DEBUG
	{
	     int i = BUFFER_SIZE;
	     bin2hex(buffer, &i, st->shared, st->sharedsize);
	     printf("Shared secret is: 0x%s\n", buffer);
	}
#endif

	if (st->oSPIprivacyctx == NULL) {
	     /* Initialize Privacy Keys from Exchange Values */
	     init_privacy_key(st, 0);   /* User -> Owner direction */
	     init_privacy_key(st, 1);   /* Owner -> User direction */
	}

	st->retries = 0;
	st->phase = VALUE_RESPONSE;
	return (0);

 resourcefail:
	packet_size = PACKET_BUFFER_SIZE;
	photuris_error_message(st, packet_buffer, &packet_size,
			       header->icookie, header->rcookie,
			       header->counter, RESOURCE_LIMIT);
	send_packet();
	return (0);
}
