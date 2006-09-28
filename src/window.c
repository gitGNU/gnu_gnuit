/* window.c -- A *very* simple window management.  */

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

/* Written by Tudor Hulubei and Andrei Pitis.  */
/* $Id: window.c,v 1.6 1999/04/22 02:30:56 tudor Exp $ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else /* !HAVE_STDLIB_H */
#include "ansi_stdlib.h"
#endif /* !HAVE_STDLIB_H */

#include <sys/types.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include <assert.h>

#include "window.h"
#include "xmalloc.h"
#include "tty.h"


window_t *
window_init()
{
    window_t *window  = (window_t *)xmalloc(sizeof(window_t));

    window_resize(window, 0, 0, 0, 0);
    return window;
}


void
window_end(window)
    window_t *window;
{
    if (window)
	xfree(window);
}


void
window_resize(window, x, y, lines, columns)
    window_t *window;
    int x, y, lines, columns;
{
    window->x       = x;
    window->y       = y;
    window->lines   = lines;
    window->columns = columns;
}


int
window_puts(window, str, length)
    window_t *window;
    char *str;
    int length;
{
    int x = window->cursor_x;

    window->cursor_x += length;

    if (x >= window->columns)
	return 0;

    if (window->cursor_y >= window->lines)
	return 0;

    if (x + length <= window->columns)
	return tty_puts(str, length);

    /* Write the visible part of the string.  */
    return tty_puts(str, window->columns - x);
}


int
window_putc(window, c)
    window_t *window;
    int c;
{
    if (++window->cursor_x > window->columns)
	return 0;

    if (window->cursor_y >= window->lines)
	return 0;

    return tty_putc(c);
}


void
window_goto(window, y, x)
    window_t *window;
    int y, x;
{
    window->cursor_x = x;
    window->cursor_y = y;
    tty_goto(y + window->y, x + window->x);
}


int
window_x(window)
    window_t *window;
{
    return window->x;
}


int
window_y(window)
    window_t *window;
{
    return window->y;
}


int
window_lines(window)
    window_t *window;
{
    return window->lines;
}


int
window_columns(window)
    window_t *window;
{
    return window->columns;
}
