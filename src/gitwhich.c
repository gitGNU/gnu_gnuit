/* gitwhich.c -- A simplified version of `which'.  */

/* Copyright (C) 1999 Free Software Foundation, Inc.

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

/* Written by Tudor Hulubei, based on the `which' code by Paul Vixie.  */
/* $Id: gitwhich.c,v 1.2 1999/05/31 20:15:14 tudor Exp $ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else /* !HAVE_STDLIB_H */
#include "ansi_stdlib.h"
#endif /* !HAVE_STDLIB_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "xmalloc.h"
#include "xstring.h"


void
fatal(postmsg)
    char *postmsg;
{
    postmsg = NULL;
    exit(1);
}


/*
 * Return 1 if the executable exists, 0 otherwise.
 */
int
find(name, path)
    char *name;
    char *path;
{
    int found = 0;
    char *pc = path;

    while (*pc != '\0' && found == 0)
    {
	char save;
	int len = 0;
	char *tmp;

	while (*pc != ':' && *pc != '\0')
	{
	    len++;
	    pc++;
	}

	save = *pc;
	*pc = '\0';
	tmp = xmalloc(strlen(pc - len) + 1 + strlen(name) + 1);
	sprintf(tmp, "%s/%s", pc - len, name);
	*pc = save;
	if (*pc)
	    pc++;

	found = (access(tmp, 1) == 0);
	xfree(tmp);
    }

    return found;
}


/*
 * Some systems don't have `which'...
 */

int
main(argc, argv)
    int argc;
    char *argv[];
{
    char *path = getenv("PATH");

    for (argc--, argv++;  argc;  argc--, argv++)
	if (find(*argv, path))
	    return 0;

    return 1;
}
