/*	$OpenBSD: ipt.h,v 1.11 2001/01/17 06:01:23 fgsch Exp $	*/

/*
 * Copyright (C) 1993-2000 by Darren Reed.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and due credit is given
 * to the original author and the contributors.
 * $IPFilter: ipt.h,v 2.2 2000/03/13 22:10:25 darrenr Exp $
 */

#ifndef	__IPT_H__
#define	__IPT_H__

#ifndef	__P
# define P_DEF
# ifdef	__STDC__
#  define	__P(x) x
# else
#  define	__P(x) ()
# endif
#endif

#include <fcntl.h>


struct	ipread	{
	int	(*r_open) __P((char *));
	int	(*r_close) __P((void));
	int	(*r_readip) __P((char *, int, char **, int *));
};

extern	void	debug __P((char *, ...));
extern	void	verbose __P((char *, ...));

#ifdef P_DEF
# undef	__P
# undef	P_DEF
#endif

#endif /* __IPT_H__ */
