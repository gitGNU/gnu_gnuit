/* xstring.c -- Code for needed string functions that might be missing.  */

/* Copyright (C) 1993-1999 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Written by Tudor Hulubei and Andrei Pitis.  strcasecmp() and strncasecmp()
   have been stolen from the GNU C library.  */
/* $Id: xstring.c,v 1.4 1999/01/16 22:37:24 tudor Exp $ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <ctype.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "xmalloc.h"
#include "xstring.h"


#ifndef HAVE_STRCASECMP

int
strcasecmp(s1, s2)
    const char *s1;
    const char *s2;
{
    unsigned char c1, c2;

    if (s1 == s2)
	return 0;

    do
    {
	c1 = tolower(*s1++);
	c2 = tolower(*s2++);

	if (c1 == 0)
	    break;
    }
    while (c1 == c2);

    return c1 - c2;
}

#endif /* !HAVE_STRCASECMP */


#ifndef HAVE_STRNCASECMP

/*
 * Compare no more than N characters of S1 and S2,
 * ignoring case, returning less than, equal to or
 * greater than zero if S1 is lexicographically less
 * than, equal to or greater than S2.
 */

int
strncasecmp(s1, s2, n)
    const char *s1;
    const char *s2;
    size_t n;
{
    unsigned char c1, c2;
    register const unsigned char *p1 = (const unsigned char *) s1;
    register const unsigned char *p2 = (const unsigned char *) s2;

    if (p1 == p2 || n == 0)
	return 0;

    do
    {
	c1 = tolower (*p1++);
	c2 = tolower (*p2++);
	if (c1 == '\0' || c1 != c2)
	    return c1 - c2;
    }
    while (--n > 0);

    return c1 - c2;
}

#endif /* !HAVE_STRNCASECMP */


#ifndef HAVE_STRSTR

/*
 * Return the first ocurrence of NEEDLE in HAYSTACK.
 */

char *
strstr(haystack, needle)
    const char *haystack;
    const char *needle;
{
    register char *needle_end   = strchr(needle, '\0');
    register char *haystack_end = strchr(haystack, '\0');
    register size_t needle_len  = needle_end - needle;
    register size_t needle_last = needle_len - 1;
    register char *begin;

    if (needle_len == 0)
	return (char *) haystack;       /* ANSI 4.11.5.7, line 25.  */

    if ((size_t) (haystack_end - haystack) < needle_len)
	return NULL;

    for (begin = &haystack[needle_last]; begin < haystack_end; ++begin)
    {
	register char *n = &needle[needle_last];
	register char *h = begin;

	do
	    if (*h != *n)
		goto loop;              /* continue for loop */
	while (--n >= needle && --h >= haystack);

	return (char *) h;

	loop:;
    }

    return NULL;
}

#endif /* !HAVE_STRSTR */


/*
 * A strdup() version that calls xmalloc instead of malloc, never returning
 * a NULL pointer.
 */

char *
xstrdup(s)
    const char *s;
{
    size_t len = strlen(s) + 1;
    char *new_s = xmalloc(len);

    memcpy(new_s, s, len);
    return new_s;
}


#ifndef HAVE_MEMMOVE

/* A slow but portable memmove function.  For those loosing systems that
   don't have one.  */

void *
memmove(dest, src, n)
    void *dest;
    const void *src;
    size_t n;
{
    char *temp = xmalloc(n);
    memcpy(temp, src, n);
    memcpy(dest, temp, n);
    xfree(temp);
    return dest;
}

#endif /* !HAVE_MEMMOVE */
