/* common.h -- Function prototypes implemented by multiple git* tools.  */

/* Copyright (C) 1997-1999, 2006-2009 Free Software Foundation, Inc.

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

/* Written by Tudor Hulubei, Andrei Pitis and Ian Beckwith.  */

#ifndef _GIT_COMMON_H
#define _GIT_COMMON_H

#include "stdc.h"
extern void clean_up PROTO ((void));
extern void fatal PROTO ((char *));
extern void clock_refresh(int signum);
extern void unhide PROTO ((int signum));
extern void hide PROTO ((void));

#endif  /* _GIT_COMMON_H */
