# $OpenBSD: Makefile,v 1.8 2003/05/11 22:41:10 hin Exp $

LIB=	asn1

KRBDEPTH=../..
.include <${.CURDIR}/../../Makefile.inc>

INCLUDE_FILES=	$(.CURDIR)/krb5_asn1.h asn1_err.h ${KRB5SRC}/lib/asn1/{der.h,asn1-common.h}

includes:	krb5_asn1.h asn1_err.h

.PATH:	${KRB5SRC}/lib/asn1

gen_files= \
	krb5_asn1.h \
	asn1_APOptions.c                \
	asn1_AP_REP.c                   \
	asn1_AP_REQ.c                   \
	asn1_AS_REP.c                   \
	asn1_AS_REQ.c                   \
	asn1_Authenticator.c            \
	asn1_AuthorizationData.c        \
	asn1_CKSUMTYPE.c		\
	asn1_Checksum.c                 \
	asn1_ETYPE_INFO.c		\
	asn1_ETYPE_INFO_ENTRY.c		\
	asn1_ENCTYPE.c			\
	asn1_EncAPRepPart.c             \
	asn1_EncASRepPart.c             \
	asn1_EncKDCRepPart.c            \
	asn1_EncKrbCredPart.c           \
	asn1_EncKrbPrivPart.c           \
	asn1_EncTGSRepPart.c            \
	asn1_EncTicketPart.c            \
	asn1_EncryptedData.c            \
	asn1_EncryptionKey.c            \
	asn1_HostAddress.c              \
	asn1_HostAddresses.c            \
	asn1_KDCOptions.c               \
	asn1_KDC_REP.c                  \
	asn1_KDC_REQ.c                  \
	asn1_KDC_REQ_BODY.c             \
	asn1_KRB_CRED.c                 \
	asn1_KRB_ERROR.c                \
	asn1_KRB_PRIV.c                 \
	asn1_KRB_SAFE.c                 \
	asn1_KRB_SAFE_BODY.c            \
	asn1_KerberosTime.c             \
	asn1_KrbCredInfo.c              \
	asn1_LastReq.c                  \
	asn1_LR_TYPE.c			\
	asn1_MESSAGE_TYPE.c		\
	asn1_METHOD_DATA.c              \
	asn1_NAME_TYPE.c		\
	asn1_PADATA_TYPE.c		\
	asn1_PA_DATA.c                  \
	asn1_PA_ENC_TS_ENC.c            \
	asn1_Principal.c                \
	asn1_PrincipalName.c            \
	asn1_Realm.c                    \
	asn1_TGS_REP.c                  \
	asn1_TGS_REQ.c                  \
	asn1_Ticket.c                   \
	asn1_TicketFlags.c              \
	asn1_TransitedEncoding.c	\
	asn1_UNSIGNED.c

CLEANFILES=	asn1_err.[ch]

BUILT_SOURCES= \
	${gen_files} \
	asn1_err.h \
	asn1_err.c

SRCS=	der_get.c \
	der_put.c \
	der_free.c \
	der_length.c \
	der_copy.c \
	${BUILT_SOURCES}

CFLAGS+=	-I${KRB5SRC}/lib/asn1 -I.

asn1_err.h asn1_err.c:	${KRB5SRC}/lib/asn1/asn1_err.et
	compile_et ${KRB5SRC}/lib/asn1/asn1_err.et

regen:	${KRB5SRC}/lib/asn1/k5.asn1
	asn1_compile ${KRB5SRC}/lib/asn1/k5.asn1 krb5_asn1

.include <bsd.lib.mk>
