/*	$OpenBSD: wresize.c,v 1.7 1998/09/13 19:16:31 millert Exp $	*/

/****************************************************************************
 * Copyright (c) 1998 Free Software Foundation, Inc.                        *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/

/****************************************************************************
 *  Author: Thomas E. Dickey <dickey@clark.net> 1996,1997                   *
 ****************************************************************************/

#include <curses.priv.h>
#include <term.h>

MODULE_ID("$From: wresize.c,v 1.10 1998/08/15 22:59:39 tom Exp $")

/*
 * Reallocate a curses WINDOW struct to either shrink or grow to the specified
 * new lines/columns.  If it grows, the new character cells are filled with
 * blanks.  The application is responsible for repainting the blank area.
 */

#define DOALLOC(p,t,n)  (t *)_nc_doalloc(p, sizeof(t)*(n))
#define	ld_ALLOC(p,n)	DOALLOC(p,struct ldat,n)
#define	c_ALLOC(p,n)	DOALLOC(p,chtype,n)

int
wresize(WINDOW *win, int ToLines, int ToCols)
{
	register int row;
	int size_x, size_y;
	struct ldat *pline;
	chtype blank;

#ifdef TRACE
	T((T_CALLED("wresize(%p,%d,%d)"), win, ToLines, ToCols));
	if (win) {
	  TR(TRACE_UPDATE, ("...beg (%d, %d), max(%d,%d), reg(%d,%d)",
			    win->_begy, win->_begx,
			    win->_maxy, win->_maxx,
			    win->_regtop, win->_regbottom));
	  if (_nc_tracing & TRACE_UPDATE)
	    _tracedump("...before", win);
	}
#endif

	if (!win || --ToLines < 0 || --ToCols < 0)
		returnCode(ERR);

	size_x = win->_maxx;
	size_y = win->_maxy;

	if (ToLines == size_y
	 && ToCols  == size_x)
		returnCode(OK);

	pline  = (win->_flags & _SUBWIN) ? win->_parent->_line : 0;

	/*
	 * If the number of lines has changed, adjust the size of the overall
	 * vector:
	 */
	if (ToLines != size_y) {
		if (! (win->_flags & _SUBWIN)) {
			for (row = ToLines+1; row <= size_y; row++)
				free((char *)(win->_line[row].text));
		}

		win->_line = ld_ALLOC(win->_line, ToLines+1);
		if (win->_line == 0)
			returnCode(ERR);

		for (row = size_y+1; row <= ToLines; row++) {
			win->_line[row].text      = 0;
			win->_line[row].firstchar = 0;
			win->_line[row].lastchar  = ToCols;
			if ((win->_flags & _SUBWIN)) {
				win->_line[row].text =
	    			&pline[win->_begy + row].text[win->_begx];
			}
		}
	}

	/*
	 * Adjust the width of the columns:
	 */
	blank = _nc_background(win);
	for (row = 0; row <= ToLines; row++) {
		chtype	*s	= win->_line[row].text;
		int	begin	= (s == 0) ? 0 : size_x + 1;
		int	end	= ToCols;

		if_USE_SCROLL_HINTS(win->_line[row].oldindex = row);

		if (ToCols != size_x || s == 0) {
			if (! (win->_flags & _SUBWIN)) {
				win->_line[row].text = s = c_ALLOC(s, ToCols+1);
				if (win->_line[row].text == 0)
					returnCode(ERR);
			} else if (s == 0) {
				win->_line[row].text = s =
	    			&pline[win->_begy + row].text[win->_begx];
			}

			if (end >= begin) {	/* growing */
				if (win->_line[row].firstchar < begin)
					win->_line[row].firstchar = begin;
				win->_line[row].lastchar = ToCols;
				do {
					s[end] = blank;
				} while (--end >= begin);
			} else {		/* shrinking */
				win->_line[row].firstchar = 0;
				win->_line[row].lastchar  = ToCols;
			}
		}
	}

	/*
	 * Finally, adjust the parameters showing screen size and cursor
	 * position:
	 */
	win->_maxx = ToCols;
	win->_maxy = ToLines;

	if (win->_regtop > win->_maxy)
		win->_regtop = win->_maxy;
	if (win->_regbottom > win->_maxy
	 || win->_regbottom == size_y)
		win->_regbottom = win->_maxy;

	if (win->_curx > win->_maxx)
		win->_curx = win->_maxx;
	if (win->_cury > win->_maxy)
		win->_cury = win->_maxy;

#ifdef TRACE
	TR(TRACE_UPDATE, ("...beg (%d, %d), max(%d,%d), reg(%d,%d)",
		win->_begy, win->_begx,
		win->_maxy, win->_maxx,
		win->_regtop, win->_regbottom));
	if (_nc_tracing & TRACE_UPDATE)
		_tracedump("...after:", win);
#endif
	returnCode(OK);
}
