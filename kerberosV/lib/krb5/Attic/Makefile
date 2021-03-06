# $OpenBSD: Makefile,v 1.26 2003/05/14 08:01:02 mho Exp $

KRBDEPTH=../..
.include <${.CURDIR}/../../Makefile.inc>

LIB=	krb5

INCLUDE_FILES=	${KRB5SRC}/lib/krb5/{krb5.h,krb5-protos.h,krb5-private.h} \
		${KRB5DIR}/include/krb5-types.h \
		krb5_err.h heim_err.h k524_err.h

includes:	krb5_err.h heim_err.h k524_err.h

MAN=	krb5.conf.5

MAN+=	krb5_init_context.3
MLINKS+=krb5_init_context.3 krb5_free_context.3

MAN+=	krb5_warn.3
MLINKS+=krb5_warn.3 krb5_warnx.3
MLINKS+=krb5_warn.3 krb5_vwarn.3
MLINKS+=krb5_warn.3 krb5_vwarnx.3
MLINKS+=krb5_warn.3 krb5_err.3
MLINKS+=krb5_warn.3 krb5_errx.3
MLINKS+=krb5_warn.3 krb5_verr.3
MLINKS+=krb5_warn.3 krb5_verrx.3
MLINKS+=krb5_warn.3 krb5_set_warn_dest.3

MAN+=	krb5_425_conv_principal.3
MLINKS+=krb5_425_conv_principal.3 krb5_425_conv_principal_ext.3
MLINKS+=krb5_425_conv_principal.3 krb5_524_conv_principal.3

MAN+=	krb5_appdefault.3
MLINKS+=krb5_appdefault.3 krb5_appdefault_boolean.3
MLINKS+=krb5_appdefault.3 krb5_appdefault_string.3
MLINKS+=krb5_appdefault.3 krb5_appdefault_time.3

MAN+=	krb5_build_principal.3
MLINKS+=krb5_build_principal.3 krb5_build_principal_ext.3
MLINKS+=krb5_build_principal.3 krb5_build_principal_va.3
MLINKS+=krb5_build_principal.3 krb5_build_principal_va_ext.3
MLINKS+=krb5_build_principal.3 krb5_make_principal.3

MAN+=	krb5_config.3
MLINKS+=krb5_config.3 krb5_config_get_bool_default.3
MLINKS+=krb5_config.3 krb5_config_get_int_default.3
MLINKS+=krb5_config.3 krb5_config_get_string_default.3
MLINKS+=krb5_config.3 krb5_config_get_time_default.3

MAN+=	krb5_openlog.3
MLINKS+=krb5_openlog.3 krb5_initlog.3
MLINKS+=krb5_openlog.3 krb5_closelog.3
MLINKS+=krb5_openlog.3 krb5_addlog_dest.3
MLINKS+=krb5_openlog.3 krb5_addlog_func.3
MLINKS+=krb5_openlog.3 krb5_log.3
MLINKS+=krb5_openlog.3 krb5_vlog.3
MLINKS+=krb5_openlog.3 krb5_log_msg.3
MLINKS+=krb5_openlog.3 krb5_vlog_msg.3

MAN+=	krb5_sname_to_principal.3
MLINKS+=krb5_sname_to_principal.3 krb5_sock_to_principal.3

MAN+=	krb5_auth_context.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_init.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_free.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_setflags.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_getflags.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_setaddrs.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_setaddrs_from_fd.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_getaddrs.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_genaddrs.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_getkey.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_setkey.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_getuserkey.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_setuserkey.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_getlocalsubkey.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_setlocalsubkey.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_getremotesubkey.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_setremotesubkey.3
MLINKS+=krb5_auth_context.3 krb5_auth_setcksumtype.3
MLINKS+=krb5_auth_context.3 krb5_auth_getcksumtype.3
MLINKS+=krb5_auth_context.3 krb5_auth_setkeytype.3
MLINKS+=krb5_auth_context.3 krb5_auth_getkeytype.3
MLINKS+=krb5_auth_context.3 krb5_auth_getlocalseqnumber.3
MLINKS+=krb5_auth_context.3 krb5_auth_setlocalseqnumber.3
MLINKS+=krb5_auth_context.3 krb5_auth_getremoteseqnumber.3
MLINKS+=krb5_auth_context.3 krb5_auth_setremoteseqnumber.3
MLINKS+=krb5_auth_context.3 krb5_auth_getauthenticator.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_getrcache.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_setrcache.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_initivector.3
MLINKS+=krb5_auth_context.3 krb5_auth_con_setivector.3

MAN+=	krb5_context.3

MAN+=	krb5_create_checksum.3
MLINKS+=krb5_create_checksum.3 krb5_checksum_is_collision_proof.3
MLINKS+=krb5_create_checksum.3 krb5_checksum_is_keyed.3
MLINKS+=krb5_create_checksum.3 krb5_checksumsize.3
MLINKS+=krb5_create_checksum.3 krb5_verify_checksum.3

MAN+=	krb5_crypto_init.3
MLINKS+=krb5_crypto_init.3 krb5_crypto_destroy.3

MAN+=	krb5_encrypt.3
MLINKS+=krb5_encrypt.3 krb5_decrypt.3
MLINKS+=krb5_encrypt.3 krb5_decrypt_EncryptedData.3
MLINKS+=krb5_encrypt.3 krb5_encrypt_EncryptedData.3

MAN+=	krb5_free_principal.3

MAN+=	krb5_keytab.3
MLINKS+=krb5_keytab.3 krb5_kt_ops.3
MLINKS+=krb5_keytab.3 krb5_keytab_entry.3
MLINKS+=krb5_keytab.3 krb5_kt_cursor.3
MLINKS+=krb5_keytab.3 krb5_kt_add_entry.3
MLINKS+=krb5_keytab.3 krb5_kt_close.3
MLINKS+=krb5_keytab.3 krb5_kt_compare.3
MLINKS+=krb5_keytab.3 krb5_kt_copy_entry_contents.3
MLINKS+=krb5_keytab.3 krb5_kt_default.3
MLINKS+=krb5_keytab.3 krb5_kt_default_name.3
MLINKS+=krb5_keytab.3 krb5_kt_end_seq_get.3
MLINKS+=krb5_keytab.3 krb5_kt_free_entry.3
MLINKS+=krb5_keytab.3 krb5_kt_get_entry.3
MLINKS+=krb5_keytab.3 krb5_kt_get_name.3
MLINKS+=krb5_keytab.3 krb5_kt_next_entry.3
MLINKS+=krb5_keytab.3 krb5_kt_read_service_key.3
MLINKS+=krb5_keytab.3 krb5_kt_register.3
MLINKS+=krb5_keytab.3 krb5_kt_remove_entry.3
MLINKS+=krb5_keytab.3 krb5_kt_resolve.3
MLINKS+=krb5_keytab.3 krb5_kt_start_seq_get.3

MAN+=	krb5_parse_name.3

MAN+=	krb5_unparse_name.3

MAN+=	krb5_verify_user.3
MLINKS+=krb5_verify_user.3 krb5_verify_user_lrealm.3

SRCS=	add_et_list.c \
	addr_families.c \
	aname_to_localname.c \
	appdefault.c \
	asn1_glue.c \
	auth_context.c \
	build_ap_req.c \
	build_auth.c \
	cache.c \
	changepw.c \
	codec.c \
	config_file.c \
	convert_creds.c \
	constants.c \
	context.c \
	copy_host_realm.c \
	crc.c \
	creds.c \
	crypto.c \
	data.c \
	expand_hostname.c \
	eai_to_heim_errno.c \
	error_string.c \
	fcache.c \
	free.c \
	free_host_realm.c \
	generate_seq_number.c \
	generate_subkey.c \
	get_addrs.c \
	get_cred.c \
	get_default_principal.c \
	get_default_realm.c \
	get_for_creds.c \
	get_host_realm.c \
	get_in_tkt.c \
	get_in_tkt_pw.c \
	get_in_tkt_with_keytab.c \
	get_in_tkt_with_skey.c \
	get_port.c \
	init_creds.c \
	init_creds_pw.c \
	keyblock.c \
	keytab.c \
	keytab_any.c \
	keytab_file.c \
	keytab_memory.c \
	keytab_krb4.c \
	keytab_keyfile.c \
	krbhst.c \
	kuserok.c \
	log.c \
	mcache.c \
	mk_error.c \
	mk_priv.c \
	mk_rep.c \
	mk_req.c \
	mk_req_ext.c \
	mk_safe.c \
	net_read.c \
	net_write.c \
	n-fold.c \
	padata.c \
	principal.c \
	prog_setup.c \
	prompter_posix.c \
	rd_cred.c \
	rd_error.c \
	rd_priv.c \
	rd_rep.c \
	rd_req.c \
	rd_safe.c \
	read_message.c \
	recvauth.c \
	replay.c \
	send_to_kdc.c \
	sendauth.c \
	set_default_realm.c \
	sock_principal.c \
	store.c \
	store_emem.c \
	store_fd.c \
	store_mem.c \
	ticket.c \
	time.c \
	transited.c \
	verify_init.c \
	verify_user.c \
	version.c \
	warn.c \
	write_message.c \
	krb5_err.c \
	heim_err.c \
	k524_err.c

# These are files in libroken that are needed in libkrb5 to make it independant
# from libroken.
SRCS+=	base64.c		\
	issuid.c		\
	getaddrinfo_hostspec.c	\
	getarg.c		\
	get_window_size.c	\
	parse_time.c		\
	parse_units.c		\
	resolve.c		\
	roken_gethostby.c	\
	roken_net_write.c	\
	roken_net_read.c	\
	strupr.c 		\
	strlwr.c		\
	strsep_copy.c		\
	warnerr.c

# libkafs
.PATH:	${.CURDIR}/../kafs ${KRB5SRC}/lib/kafs
CFLAGS+=-I${KRB5SRC}/lib/kafs -DROKEN_RENAME
SRCS+=	afskrb5.c \
	afssys_openbsd.c \
	common.c
MAN+=	kafs.3
MLINKS+=kafs.3 k_hasafs.3 \
	kafs.3 k_pioctl.3 \
	kafs.3 k_unlog.3 \
	kafs.3 k_setpag.3 \
	kafs.3 k_afs_cell_of_file.3 \
	kafs.3 krb_afslog.3 \
	kafs.3 krb_afslog_uid.3


# There is a net_write.c and net_read.c both in lib/roken and lib/krb5.
# This stinks.
roken_net_write.c:	${KRB5SRC}/lib/roken/net_write.c
	ln -sf ${KRB5SRC}/lib/roken/net_write.c roken_net_write.c
roken_net_read.c:	${KRB5SRC}/lib/roken/net_read.c
	ln -sf ${KRB5SRC}/lib/roken/net_read.c roken_net_read.c


CLEANFILES+=	krb5_err.[ch] heim_err.[ch] k524_err.[ch]

.PATH:	${KRB5SRC}/lib/krb5 ${KRB5SRC}/lib/roken ${KRB5SRC}/lib/kafs
CFLAGS+=-I${KRB5SRC}/lib/krb5 -I${KRB5SRC}/lib/ \
	-I${KRB5SRC}/lib/asn1 -I../asn1 -I. \
	-DSHA1_CTX=SHA_CTX

krb5_err.c krb5_err.h:	${KRB5SRC}/lib/krb5/krb5_err.et
	compile_et ${KRB5SRC}/lib/krb5/krb5_err.et

heim_err.c heim_err.h:	${KRB5SRC}/lib/krb5/heim_err.et
	compile_et ${KRB5SRC}/lib/krb5/heim_err.et

k524_err.c k524_err.h:	${KRB5SRC}/lib/krb5/k524_err.et
	compile_et ${KRB5SRC}/lib/krb5/k524_err.et

.include <bsd.lib.mk>
