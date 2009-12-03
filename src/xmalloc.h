/* xmalloc.h -- Prototypes for the functions in xmalloc.c.  */

/* Copyright (C) 1993-1999, 2009 Free Software Foundation, Inc.

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

#ifndef _GIT_XMALLOC_H
#define _GIT_XMALLOC_H

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPE_H */
#include <stddef.h>
#include <stdlib.h>
#include "stdc.h"

#include "xalloc.h"

extern void  xalloc_die PROTO ((void));
extern void  xfree PROTO ((void *));


#endif  /* _GIT_XMALLOC_H */
