/*  Adapted for ELF dynamic linker - Mitch
 *
 *  vsprintf.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds and Lars Wirzenius
 *
 *  Wirzenius wrote this portably, Torvalds fucked it up :-)
 */
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include "hash.h"
#include "string.h"
#include "syscall.h"


/* we use this so that we can do without the ctype library */
#define is_digit(c)	((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s)
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define SMALL	64		/* use 'abcdef' instead of 'ABCDEF' */

static char * number(char * str, int remain, int num, int base,
			int size, int precision ,int type)
{
	char c,sign,tmp[36];
	const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	if (type&SMALL) digits="0123456789abcdefghijklmnopqrstuvwxyz";
	if (type&LEFT) type &= ~ZEROPAD;
	if (base<2 || base>36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ' ;
	if (type&SIGN && num<0) {
		sign='-';
		num = -num;
	} else
		sign=(type&PLUS) ? '+' : ((type&SPACE) ? ' ' : 0);
	if (sign) size--;
	if (type&SPECIAL)
		if (base==16) size -= 2;
		else if (base==8) size--;
	i=0;
	if (num==0)
		tmp[i++]='0';
	else while (num!=0) {
		tmp[i++]=digits[(unsigned)num % base];
		(unsigned)num /= base;
	}
	if (i>precision) precision=i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0 && --remain > 0)
			*str++ = ' ';
	if (sign && --remain > 0)
		*str++ = sign;
	if (type & SPECIAL)
		if (base == 8 && --remain > 0)
			*str++ = '0';
		else if (base == 16 && --remain > 0 && --remain > 0) {
			*str++ = '0';
			*str++ = digits[33];
		}
	if (!(type & LEFT))
		while(size-- > 0 && --remain > 0)
			*str++ = c;
	while(i < precision-- && --remain > 0)
		*str++ = '0';
	while(i-- > 0 && --remain > 0)
		*str++ = tmp[i];
	while(size-- > 0 && --remain > 0)
		*str++ = ' ';
	return str;
}

int _dl_fdprintf(int fd, const char *fmt, ...)
{
	int len;
	int i;
	char * str;
	char *s;
	int *ip;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */
	int remain;
	char buf[1024];
	va_list args;

	va_start(args, fmt);
	for (str=buf, remain=sizeof(buf) ; *fmt ; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			remain--;
			continue;
		}
			
		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}
		
		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;	
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0 && --remain > 0) 
					*str++ = ' ';
			if(--remain > 0)
				*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0 && --remain > 0)
				*str++ = ' ';
			break;

		case 's':
			s = va_arg(args, char *);
			len = _dl_strlen(s);
			if (precision < 0)
				precision = len;
			else if (len > precision)
				len = precision;

			if (!(flags & LEFT))
				while (len < field_width-- && --remain > 0)
					*str++ = ' ';
			for (i = 0; i < len && remain > 1; ++i, --remain)
				*str++ = *s++;
			while (len < field_width-- && --remain > 0)
				*str++ = ' ';
			break;

		case 'o':
			str = number(str, remain,
				va_arg(args, unsigned long), 8,
				field_width, precision, flags);
			remain = buf + sizeof(buf) - 1 - str;
			break;

		case 'p':
			if (field_width == -1) {
				field_width = 8;
				flags |= ZEROPAD;
			}
			str = number(str, remain,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			remain = buf + sizeof(buf) - 1 - str;
			break;

		case 'x':
			flags |= SMALL;
		case 'X':
			str = number(str, remain,
				va_arg(args, unsigned long), 16,
				field_width, precision, flags);
			remain = buf + sizeof(buf) - 1 - str;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			str = number(str, remain,
				va_arg(args, unsigned long), 10,
				field_width, precision, flags);
			remain = buf + sizeof(buf) - 1 - str;
			break;

		case 'n':
			ip = va_arg(args, int *);
			*ip = (str - buf);
			break;

		default:
			if (*fmt != '%')
				*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			remain--;
			break;
		}
	}
	*str = '\0';
	_dl_write(fd, buf, str-buf);
	return str-buf;
}

