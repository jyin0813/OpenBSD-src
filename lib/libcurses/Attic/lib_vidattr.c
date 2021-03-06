/*	$OpenBSD: lib_vidattr.c,v 1.5 1998/08/03 17:02:46 millert Exp $	*/

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
 *  Author: Zeyd M. Ben-Halim <zmbenhal@netcom.com> 1992,1995               *
 *     and: Eric S. Raymond <esr@snark.thyrsus.com>                         *
 ****************************************************************************/

/*
 *	vidputs(newmode, outc)
 *
 *	newmode is taken to be the logical 'or' of the symbols in curses.h
 *	representing graphic renditions.  The terminal is set to be in all of
 *	the given modes, if possible.
 *
 *	if the new attribute is normal
 *		if exit-alt-char-set exists
 *			emit it
 *		emit exit-attribute-mode
 *	else if set-attributes exists
 *		use it to set exactly what you want
 *	else
 *		if exit-attribute-mode exists
 *			turn off everything
 *		else
 *			turn off those which can be turned off and aren't in
 *			newmode.
 *		turn on each mode which should be on and isn't, one by one
 *
 *	NOTE that this algorithm won't achieve the desired mix of attributes
 *	in some cases, but those are probably just those cases in which it is
 *	actually impossible, anyway, so...
 *
 * 	NOTE that we cannot assume that there's no interaction between color
 *	and other attribute resets.  So each time we reset color (or other
 *	attributes) we'll have to be prepared to restore the other.
 */

#include <curses.priv.h>
#include <term.h>

MODULE_ID("$From: lib_vidattr.c,v 1.21 1998/08/01 22:21:19 tom Exp $")

#define doPut(mode) TPUTS_TRACE(#mode); tputs(mode, 1, outc)

#define TurnOn(mask,mode) \
	if ((turn_on & mask) && mode) { doPut(mode); }

#define TurnOff(mask,mode) \
	if ((turn_off & mask) && mode) { doPut(mode); turn_off &= ~mask; }

	/* if there is no current screen, assume we *can* do color */
#define SetColorsIf(why,old_attr) \
	if ((!SP || SP->_coloron) && (why)) { \
		int old_pair = PAIR_NUMBER(old_attr); \
		T(("old pair = %d -- new pair = %d", old_pair, pair)); \
		if ((pair != old_pair) \
		 || (reverse ^ ((old_attr & A_REVERSE) != 0))) { \
			_nc_do_color(pair, reverse, outc); \
		} \
	}

int vidputs(attr_t newmode, int  (*outc)(int))
{
static attr_t previous_attr = A_NORMAL;
attr_t turn_on, turn_off;
int pair;
bool reverse = FALSE;
bool used_ncv = FALSE;

	T((T_CALLED("vidputs(%s)"), _traceattr(newmode)));

	/* this allows us to go on whether or not newterm() has been called */
	if (SP)
		previous_attr = SP->_current_attr;

	T(("previous attribute was %s", _traceattr(previous_attr)));

#if !USE_XMC_SUPPORT
	if (magic_cookie_glitch > 0)
		newmode &= ~(SP->_xmc_suppress);
#endif

	/*
	 * If we have a terminal that cannot combine color with video
	 * attributes, use the colors in preference.
	 */
	if ((newmode & A_COLOR)
	 && (no_color_video > 0)) {
		static const struct {
			attr_t video;
			unsigned bit;
		} table[] = {
			{ A_STANDOUT,		1 },
			{ A_UNDERLINE,		2 },
			{ A_REVERSE,		4 },
			{ A_BLINK,		8 },
			{ A_DIM,		16 },
			{ A_BOLD, 		32 },
			{ A_INVIS,		64 },
			{ A_PROTECT,		128 },
			{ A_ALTCHARSET,		256 },
		};
		size_t n;
		for (n = 0; n < SIZEOF(table); n++) {
			if ((table[n].bit & no_color_video)
			 && (table[n].video & newmode)) {
				used_ncv = TRUE;
				if (table[n].video == A_REVERSE)
					reverse = TRUE;
				else
					newmode &= ~table[n].video;
			}
		}
	}

	if (newmode == previous_attr)
		returnCode(OK);

	pair = PAIR_NUMBER(newmode);

	if (reverse) {
		newmode &= ~A_REVERSE;
	}

	turn_off = (~newmode & previous_attr) & ALL_BUT_COLOR;
	turn_on  = (newmode & ~previous_attr) & ALL_BUT_COLOR;

	SetColorsIf(pair == 0, previous_attr);

	if (newmode == A_NORMAL) {
		if((previous_attr & A_ALTCHARSET) && exit_alt_charset_mode) {
			doPut(exit_alt_charset_mode);
			previous_attr &= ~A_ALTCHARSET;
		}
		if (previous_attr) {
			doPut(exit_attribute_mode);
			previous_attr &= ~A_COLOR;
		}

		SetColorsIf(pair != 0, previous_attr);
	} else if (set_attributes && !used_ncv) {
		if (turn_on || turn_off) {
			TPUTS_TRACE("set_attributes");
			tputs(tparm(set_attributes,
				(newmode & A_STANDOUT) != 0,
				(newmode & A_UNDERLINE) != 0,
				(newmode & A_REVERSE) != 0,
				(newmode & A_BLINK) != 0,
				(newmode & A_DIM) != 0,
				(newmode & A_BOLD) != 0,
				(newmode & A_INVIS) != 0,
				(newmode & A_PROTECT) != 0,
				(newmode & A_ALTCHARSET) != 0), 1, outc);
			previous_attr &= ~A_COLOR;
		}
		SetColorsIf(pair != 0, previous_attr);
	} else {

		T(("turning %s off", _traceattr(turn_off)));

		TurnOff(A_ALTCHARSET,  exit_alt_charset_mode);
		TurnOff(A_UNDERLINE,   exit_underline_mode);
		TurnOff(A_STANDOUT,    exit_standout_mode);

		if (turn_off && exit_attribute_mode) {
			doPut(exit_attribute_mode);
			turn_on  |= (newmode & (chtype)(~A_COLOR));
			previous_attr &= ~A_COLOR;
		}
		SetColorsIf(pair != 0, previous_attr);

		T(("turning %s on", _traceattr(turn_on)));

		TurnOn (A_ALTCHARSET, enter_alt_charset_mode);
		TurnOn (A_BLINK,      enter_blink_mode);
		TurnOn (A_BOLD,       enter_bold_mode);
		TurnOn (A_DIM,        enter_dim_mode);
		TurnOn (A_REVERSE,    enter_reverse_mode);
		TurnOn (A_STANDOUT,   enter_standout_mode);
		TurnOn (A_PROTECT,    enter_protected_mode);
		TurnOn (A_INVIS,      enter_secure_mode);
		TurnOn (A_UNDERLINE,  enter_underline_mode);
		TurnOn (A_HORIZONTAL, enter_horizontal_hl_mode);
		TurnOn (A_LEFT,       enter_left_hl_mode);
		TurnOn (A_LOW,        enter_low_hl_mode);
		TurnOn (A_RIGHT,      enter_right_hl_mode);
		TurnOn (A_TOP,        enter_top_hl_mode);
		TurnOn (A_VERTICAL,   enter_vertical_hl_mode);
	}

	if (reverse)
		newmode |= A_REVERSE;

	if (SP)
		SP->_current_attr = newmode;
	else
		previous_attr = newmode;

	returnCode(OK);
}

int vidattr(attr_t newmode)
{
	T((T_CALLED("vidattr(%s)"), _traceattr(newmode)));

	returnCode(vidputs(newmode, _nc_outch));
}

chtype termattrs(void)
{
	chtype attrs = A_NORMAL;

	if (enter_alt_charset_mode)
		attrs |= A_ALTCHARSET;

	if (enter_blink_mode)
		attrs |= A_BLINK;

	if (enter_bold_mode)
		attrs |= A_BOLD;

	if (enter_dim_mode)
		attrs |= A_DIM;

	if (enter_reverse_mode)
		attrs |= A_REVERSE;

	if (enter_standout_mode)
		attrs |= A_STANDOUT;

	if (enter_protected_mode)
		attrs |= A_PROTECT;

	if (enter_secure_mode)
		attrs |= A_INVIS;

	if (enter_underline_mode)
		attrs |= A_UNDERLINE;

	if (SP->_coloron)
		attrs |= A_COLOR;

	return(attrs);
}

