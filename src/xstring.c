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
/* $Id: xstring.c,v 1.1.1.1 2004-11-10 17:44:38 ianb Exp $ */

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


