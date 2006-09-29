/* xid.h -- Prototypes for the functions in xid.c.  */

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
/* $Id: xid.h,v 1.1.1.1 2004-11-10 17:44:38 ianb Exp $ */

#ifndef _GIT_XID_H
#define _GIT_XID_H


#include <sys/types.h>

#include <pwd.h>
#include <grp.h>

#include "stdc.h"


extern char *xgetpwuid PROTO ((uid_t));
extern char *xgetgrgid PROTO ((gid_t));


#endif /* _GIT_XID_H */
