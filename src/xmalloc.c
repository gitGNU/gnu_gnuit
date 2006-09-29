/* xmalloc.c -- Safe memory management routines.  Includes xmalloc, xcalloc,
   xrealloc and free.  fatal() is called when there is no more memory
   available.  */

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
/* $Id: xmalloc.c,v 1.1.1.1 2004-11-10 17:44:38 ianb Exp $ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "xmalloc.h"


extern void fatal PROTO ((char *));


char *
xmalloc(size)
    size_t size;
{
    void *pointer = malloc(size ? size : 1);

    if (pointer == NULL)
	fatal("xmalloc: virtual memory exhausted");

    return (char *)pointer;
}


char *
xcalloc(count, itemsize)
    size_t count, itemsize;
{
    void *pointer;

    if (count && itemsize)
	pointer = calloc(count, itemsize);
    else
	pointer = calloc(1, 1);

    if (pointer == NULL)
	fatal("xcalloc: virtual memory exhausted");

    return (char *)pointer;
}


char *
xrealloc(pointer, size)
    void *pointer;
    size_t size;
{
    /* I know realloc should call malloc if 'pointer' is NULL, but it seems
       to work better on suns. Strange... */
    void *new_pointer = pointer ? realloc(pointer, size ? size : 1) :
				  malloc(size ? size : 1);

    if (new_pointer == NULL)
	fatal("xrealloc: virtual memory exhausted");

    return (char *)new_pointer;
}


void
xfree(pointer)
    void *pointer;
{
    if (pointer)
	free(pointer);
    else
	fatal("xfree: trying to free NULL");
}
