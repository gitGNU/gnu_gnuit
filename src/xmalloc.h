/* xmalloc.h -- Prototypes for the functions in xmalloc.c.  */

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
/* $Id: xmalloc.h,v 1.1.1.1 2004-11-10 17:44:38 ianb Exp $ */

#ifndef _GIT_XMALLOC_H
#define _GIT_XMALLOC_H


#include <sys/types.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else /* !HAVE_STDLIB_H */
#include "ansi_stdlib.h"
#endif /* !HAVE_STDLIB_H */

#include "stdc.h"


extern char *xmalloc PROTO ((size_t));
extern char *xcalloc PROTO ((size_t, size_t));
extern char *xrealloc PROTO ((void *, size_t));
extern void  xfree PROTO ((void *));


#endif  /* _GIT_XMALLOC_H */
