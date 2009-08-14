/* git.h -- Function prototypes for git.c.  */

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

#ifndef _GIT_GIT_H
#define _GIT_GIT_H

#include "stdc.h"

extern int in_terminal_mode PROTO ((void));
extern void resize PROTO ((int));
extern void screen_refresh PROTO ((int));

#endif  /* _GIT_GIT_H */
