/* signal.h -- Function prototypes for those stupid functions in signal.c.  */

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
/* $Id: signals.h,v 1.6 1999/01/16 22:37:24 tudor Exp $ */

#ifndef _GIT_SIGNAL_H
#define _GIT_SIGNAL_H


#include <signal.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "stdc.h"


extern int user_heart_attack;


extern void signals_init PROTO (());
extern void signal_handlers PROTO ((int));
extern void signals PROTO ((int));
extern void service_pending_signals PROTO (());


#endif  /* _GIT_SIGNAL_H */
