/* status.h -- The #defines and function prototypes used in status.c.  */

/* Copyright (C) 1993-1999, 2006-2007 Free Software Foundation, Inc.

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

#ifndef _GIT_STATUS_H
#define _GIT_STATUS_H


#include "stdc.h"


#define STATUS_OK		0
#define STATUS_WARNING		1
#define STATUS_ERROR		2

#define STATUS_CENTERED		0
#define STATUS_LEFT		1


extern void status_init PROTO ((char *));
extern void status_end PROTO (());
extern void status_resize PROTO ((size_t, size_t));
extern void status_update PROTO (());
extern void status PROTO ((char *, int, int));
extern void status_default PROTO (());


#endif  /* _GIT_STATUS_H */
