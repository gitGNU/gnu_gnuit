/* window.h -- Data structures and function prototypes used by window.c.  */

/* Copyright (C) 1993-1999, 2006-2009 Free Software Foundation, Inc.

 This file is part of gnuit.

 gnuit is free software: you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published
 by the Free Software Foundation, either version 3 of the
 License, or (at your option) any later version.

 gnuit is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public
 License along with this program. If not, see
 http://www.gnu.org/licenses/. */

/* Written by Tudor Hulubei and Andrei Pitis.  */

#ifndef _GIT_WINDOW_H
#define _GIT_WINDOW_H

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPE_H */
#include <stddef.h>
#include "stdc.h"

#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif

#ifdef HAVE_NCURSESW_CURSES_H
#include <ncursesw/curses.h>
#else
#ifdef HAVE_CURSES_H
#include <curses.h>
#endif	/* HAVE_CURSES_H */
#endif	/* HAVE_NCURSESW_CURSES_H */

typedef struct
{
    int x;		/* window's x origin in the tty screen.  */
    int y;		/* window's y origin in the tty screen.  */
    int wlines;		/* window's number of lines.  */
    int wcolumns;	/* window's number of columns.  */
    int cursor_x;	/* window's cursor current x position.  */
    int cursor_y;	/* window's cursor current y position.  */
} window_t;


extern window_t *window_init PROTO ((int, int, int, int));
extern void window_end PROTO ((window_t *));
extern void window_resize PROTO ((window_t *, int, int, int, int));
extern int window_puts PROTO ((window_t *, wchar_t *, int));
extern int window_putc PROTO ((window_t *, wchar_t));
extern void window_goto PROTO ((window_t *, int, int));
extern int window_x PROTO ((window_t *));
extern int window_y PROTO ((window_t *));
extern int window_lines PROTO ((window_t *));
extern int window_columns PROTO ((window_t *));

#endif  /* _GIT_WINDOW_H */
