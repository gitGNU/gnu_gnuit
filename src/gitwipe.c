/* gitwipe.c -- An utility that deletes the contents of a file in order to
   make it impossible for someone to recover it.  */

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
/* $Id: gitwipe.c,v 1.1.1.1 2004-11-10 17:44:38 ianb Exp $ */

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

#include "xtime.h"

#include <sys/types.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "file.h"
#include <fcntl.h>
#include <limits.h>


#define WIPE_BUFFER_SIZE        64*1024
#undef min
#define min(a, b) ((a) <= (b) ? (a) : (b))


char *program;


void
usage()
{
    fprintf(stderr, "%s file1 [file2 [...]]\n", program);
    exit(1);
}


int
file_length(fd)
    int fd;
{
    int current, length;

    current = lseek(fd, 0, SEEK_CUR);
    length  = lseek(fd, 0, SEEK_END);
    lseek(fd, current, SEEK_SET);
    return length;
}


int
wipe(file)
    char *file;
{
    int fd;
    unsigned char *buf;
    size_t len, i, j, bytes_to_write;

    fd = open(file, O_RDWR);

    if (fd == -1)
    {
	fprintf(stderr, "%s: can't open file '%s'.\n", program, file);
	return 1;
    }

    len = file_length(fd);

    if (len == 0)
	return 0;

    buf = (unsigned char *)malloc(WIPE_BUFFER_SIZE);

    if (buf == NULL)
    {
	fprintf(stderr, "%s: virtual memory exhausted.\n", program);
	return 1;
    }

    for (i = 0; i < len; i += WIPE_BUFFER_SIZE)
    {
	bytes_to_write = min(len - i, WIPE_BUFFER_SIZE);

	for (j = 0; j < bytes_to_write; j++)
	    buf[j] = rand() % 0xFF;

	if (write(fd, buf, bytes_to_write) != (int)bytes_to_write)
	{
	    fprintf(stderr, "%s: can't write to file %s.\n", program, file);
	    return 1;
	}
    }

    close(fd);
    sync();

    /* Don't delete the file! The file system might notice that the blocks
       in this file are no longer used and never write them back to disk.
       And there is more: sync() may return before the actual writing is
       done.  See the Linux sync(2) & sync(8) manual pages for more detail.
       If you want to be sure, REBOOT NOW ! :-))))))))))))))  */

    return 0;
}


/*
 * argv[1...]   = files
 * return value = no of errors (unprocessed files)
 */

int
main(argc, argv)
    int argc;
    char *argv[];
{
    int i, errors = 0;

    program = argv[0];

    if (argc < 2)
	usage();

    srand(time(NULL));

    for (i = 1; i < argc; i++)
	errors += wipe(argv[i]);

    return errors;
}
