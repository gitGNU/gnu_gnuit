/* xstring.h -- Prototypes for the functions in xstring.c.  */

/* Copyright (C) 1993-1999, 2006-2007 Free Software Foundation, Inc.

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

#ifndef _GIT_XSTRING_H
#define _GIT_XSTRING_H


#include <sys/types.h>
#include <stddef.h>

#if defined(STDC_HEADERS)

#include <string.h>
/* An ANSI string.h and pre-ANSI memory.h might conflict.  */

#else /* !STDC_HEADERS  */

#if defined(HAVE_MEMORY_H)
#include <memory.h>
#else /* !HAVE_MEMORY_H */
/* memory.h and strings.h conflict on some systems.  */
#include <strings.h>
#endif /* !HAVE_MEMORY_H */
#endif /* !STDC_HEADERS */

#include "stdc.h"
#include "xalloc.h"

#endif  /* _GIT_XSTRING_H */
