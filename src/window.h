/* window.h -- Data structures and function prototypes used by window.c.  */

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
/* $Id: window.h,v 1.6 1999/04/22 02:30:56 tudor Exp $ */

#ifndef _GIT_WINDOW_H
#define _GIT_WINDOW_H


#include <sys/types.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "stdc.h"


typedef struct
{
    int x;		/* window's x origin in the tty screen.  */
    int y;		/* window's y origin in the tty screen.  */
    int lines;		/* window's number of lines.  */
    int columns;	/* window's number of columns.  */
    int cursor_x;	/* window's cursor current x position.  */
    int cursor_y;	/* window's cursor current y position.  */
} window_t;


extern window_t *window_init PROTO (());
extern void window_end PROTO ((window_t *));
extern void window_resize PROTO ((window_t *, int, int, int, int));
extern int window_puts PROTO ((window_t *, char *, int));
extern int window_putc PROTO ((window_t *, int));
extern void window_goto PROTO ((window_t *, int, int));
extern int window_x PROTO ((window_t *));
extern int window_y PROTO ((window_t *));
extern int window_lines PROTO ((window_t *));
extern int window_columns PROTO ((window_t *));


#endif  /* _GIT_WINDOW_H */
