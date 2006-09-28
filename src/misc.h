/* misc.h -- Prototypes for the functions in misc.c.  */

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
/* $Id: misc.h,v 1.7 1999/04/20 16:38:51 tudor Exp $ */

#ifndef _GIT_MISC_H
#define _GIT_MISC_H


#include <sys/types.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "xtime.h"

#include "stdc.h"


extern char *home;
extern char *program;
extern char *login_name;

extern char *day_name[];
extern char *month_name[];

extern char color_section[];
extern char monochrome_section[];


#undef max
#undef min
#define max(a, b) (((a) >= (b)) ? (a) : (b))
#define min(a, b) (((a) <= (b)) ? (a) : (b))


/* It looks like we need this for SunOS.  */
#ifndef RAND_MAX
#define RAND_MAX 2147483647
#endif


#define close2(fd1, fd2)        \
{                               \
     close(fd1);                \
     close(fd2);                \
}


#define xfree2(ptr1, ptr2)      \
{                               \
     xfree(ptr1);               \
     xfree(ptr2);               \
}


typedef struct file_type_info_tag
{
    char *pattern;
    int foreground;
    int background;
    int brightness;
    struct file_type_info_tag *next;
} file_type_info_t;

extern file_type_info_t * fti_head;


extern void compute_directories PROTO (());
extern void update_path PROTO (());
extern RETSIGTYPE fatal_signal PROTO ((int));
extern void display_exit_message PROTO ((char *));
extern void configuration_help PROTO ((char *));
extern void common_configuration_init PROTO (());
extern int specific_configuration_init PROTO (());
extern void use_section PROTO ((char *));
extern int get_int_var PROTO ((char *, int));
extern int get_const_var PROTO ((char *, char *[], int, int));
extern int get_flag_var PROTO ((char *, int));
extern char *get_string_var PROTO ((char *, char *));
extern void get_colorset_var PROTO ((int *, char *[], int));
extern char *minimize_path PROTO ((char *));
extern void get_login_name PROTO (());
extern void truncate_long_name PROTO ((char *, char *, int));
extern char *truncate_string PROTO ((char *, char *, int));
extern int get_file_length PROTO ((int));
extern struct tm *get_local_time PROTO (());
extern int is_a_bg_command PROTO ((char *));
extern int is_an_empty_command PROTO ((char *));
extern void get_file_type_info PROTO (());
extern void toprintable PROTO ((char *, size_t));
extern int needs_quotes PROTO ((char *, size_t));
extern int xsetenv PROTO ((char *, char *));


#endif  /* _GIT_MISC_H */
