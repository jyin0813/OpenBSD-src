/* $OpenBSD: pgp.h,v 1.4 2002/02/17 19:42:38 millert Exp $ */
/* Estimate size of pgp signature */
#define MAXPGPSIGNSIZE	1024

#ifndef PGP
#define PGP "/usr/local/bin/pgp"
#endif

struct mygzip_header;
struct signature;

extern void *new_pgp_checker(struct mygzip_header *h, \
	struct signature *sign, const char *userid, char *envp[], \
	const char *filename);

extern void pgp_add(void *arg, const char *buffer, size_t length);

extern int pgp_sign_ok(void *arg);

extern void handle_pgp_passphrase(void);

extern int retrieve_pgp_signature(const char *filename, \
	struct signature **sign, const char *userid, char *envp[]);
