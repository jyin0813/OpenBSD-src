#if defined(LIBC_SCCS) && !defined(lint)
/*static char *sccsid = "from: @(#)strlen.c	5.5 (Berkeley) 1/26/91";*/
static char *rcsid = "$Id: strchr.c,v 1.1 1997/03/25 17:07:13 rahnds Exp $";
#endif /* LIBC_SCCS and not lint */

#include <string.h>

char *
strchr(str, c)
	const char *str;
{
	index(str, c);
}
