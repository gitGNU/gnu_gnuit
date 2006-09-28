/* title.h -- Function prototypes for title.c.  */

/* Copyright (C) 1997-1999 Free Software Foundation, Inc.

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
/* $Id: title.h,v 1.4 1999/01/16 22:37:24 tudor Exp $ */

#ifndef _GIT_TITLE_H
#define _GIT_TITLE_H


#include <sys/types.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "stdc.h"


#define TITLE_FIELDS 8
extern char *TitleFields[TITLE_FIELDS];
extern int TitleColors[TITLE_FIELDS];

extern void title_init PROTO (());
extern void title_end PROTO (());
extern void title_resize PROTO ((size_t, size_t));
extern void title_update PROTO (());


#endif  /* _GIT_TITLE_H */
