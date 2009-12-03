/* title.h -- Function prototypes for title.c.  */

/* Copyright (C) 1997-1999, 2006-2007 Free Software Foundation, Inc.

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

#ifndef _GIT_TITLE_H
#define _GIT_TITLE_H

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPE_H */
#include <stddef.h>
#include "stdc.h"


#define TITLE_FIELDS 8
extern char *TitleFields[TITLE_FIELDS];
extern int TitleColors[TITLE_FIELDS];

extern void title_init PROTO ((void));
extern void title_end PROTO ((void));
extern void title_resize PROTO ((size_t, size_t));
extern void title_update PROTO ((void));

#endif  /* _GIT_TITLE_H */
