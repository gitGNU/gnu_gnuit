/* configure.h -- The prototypes of functions used in config.c.  */

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
/* $Id: configure.h,v 1.4 1999/01/16 22:37:21 tudor Exp $ */

#ifndef _GIT_CONFIGURE_H
#define _GIT_CONFIGURE_H


#include "stdc.h"


#define MAXLINE         1024


#define NO_SEEK         0
#define DO_SEEK         1


#define IFS             ';'     /* internal field separator */
#define ICS             '#'     /* internal comment separator */
#define IAS             '='     /* internal assignment operator */


extern int configuration_init PROTO ((char *));
extern void configuration_end PROTO (());
extern int configuration_section PROTO ((char *));
extern void configuration_getvarinfo PROTO ((char *, char **, int, int));


#endif  /* _GIT_CONFIGURE_H */
