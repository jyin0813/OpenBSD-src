#	from: @(#)Makefile.inc	5.21 (Berkeley) 5/24/91
#	$Id: GNUmakefile.inc,v 1.1 1998/07/21 13:19:44 peter Exp $

# gen sources
VPATH:= ${VPATH}:${srcdir}/net

SRCS:=	gethostbyaddr.c gethostbyname.c gethostent.c getnetbyaddr.c \
	getnetbyname.c getnetent.c getproto.c getprotoent.c getprotoname.c \
	getservbyname.c getservbyport.c getservent.c herror.c inet_addr.c \
	inet_lnaof.c inet_makeaddr.c inet_netof.c inet_network.c inet_ntoa.c \
	net_internal.c proto_internal.c res_comp.c res_init.c res_internal.c \
	res_mkquery.c res_query.c res_querydomain.c res_search.c res_send.c \
	res_debug.c serv_internal.c $(SRCS)

