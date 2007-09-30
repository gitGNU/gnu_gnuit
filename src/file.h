/* file.h -- Backward compatibility SEEK_* constants.  */

/* Copyright (C) 1993-2000, 2006-2007 Free Software Foundation, Inc.

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

#ifndef _GIT_FILE_H
#define _GIT_FILE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/file.h>


/* Older BSD systems need this. */

#ifndef SEEK_SET
#ifdef L_SET
#define SEEK_SET L_SET
#else
#define SEEK_SET 0
#endif /* L_SET */
#endif /* SEEK_SET */

#ifndef SEEK_CUR
#ifdef L_INCR
#define SEEK_CUR L_INCR
#else
#define SEEK_CUR 1
#endif /* L_INCR */
#endif /* SEEK_CUR */

#ifndef SEEK_END
#ifdef L_XTND
#define SEEK_END L_XTND
#else
#define SEEK_END 2
#endif /* L_XTND */
#endif /* SEEK_END */

#ifndef _LARGEFILE64_SOURCE
#define off64_t off_t
#define fopen64 fopen
#define lseek64 lseek
#define open64 open
#endif // !_LARGEFILE64_SOURCE

#endif  /* _GIT_FILE_H */
