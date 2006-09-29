/* window.c -- A *very* simple window management.  */

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
/* $Id: title.c,v 1.1.1.1 2004-11-10 17:44:38 ianb Exp $ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else /* !HAVE_STDLIB_H */
#include "ansi_stdlib.h"
#endif /* !HAVE_STDLIB_H */

#include <sys/types.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include <assert.h>

#include "window.h"
#include "xmalloc.h"
#include "xstring.h"
#include "tty.h"
#include "title.h"
#include "misc.h"


char *TitleFields[TITLE_FIELDS] =
{
    "TitleForeground",
    "TitleBackground",
    "TitleBrightness",
    "UserName",
    "TtyName",
    "ClockForeground",
    "ClockBackground",
    "ClockBrightness",
};

#ifdef HAVE_LINUX
int TitleColors[TITLE_FIELDS] =
{
    CYAN, BLUE, ON, YELLOW, YELLOW
};
#else   /* !HAVE_LINUX */
int TitleColors[TITLE_FIELDS] =
{
    WHITE, BLACK, ON, WHITE, WHITE
};
#endif  /* !HAVE_LINUX */

#define TitleForeground TitleColors[0]
#define TitleBackground TitleColors[1]
#define TitleBrightness TitleColors[2]
#define UserName        TitleColors[3]
#define TtyName         TitleColors[4]
#define ClockForeground TitleColors[5]
#define ClockBackground TitleColors[6]
#define ClockBrightness TitleColors[7]


static char *product_name;

static int product_name_length;
static int login_name_length;
static int tty_device_length;

/* The length of the "User: tudor  tty: /dev/ttyp0" info string.
   It also includes 6 characters for the clock.  */
static int info_length;

static char login_string[] = "User:";
static char ttydev_string[] = "tty:";


static window_t *title_window;

extern int in_terminal_mode PROTO (());

void
title_init()
{
    product_name = xmalloc(1 + strlen(PRODUCT) + 1 + strlen(VERSION) + 1);
    sprintf(product_name, " %s %s", PRODUCT, VERSION);
    product_name_length = strlen(product_name);
    login_name_length = strlen(login_name);
    tty_device_length = strlen(tty_device);

    info_length = (sizeof(login_string)  - 1) + 1 + login_name_length + 2 +
		  (sizeof(ttydev_string) - 1) + 1 + tty_device_length + 1 +
		  6 + 1;

    title_window = window_init();
}


void
title_end()
{
    window_end(title_window);
}


void
title_resize(columns, line)
    size_t columns, line;
{
    window_resize(title_window, 0, line, 1, columns);
}


/*
 * Update the title clock only.  If signum is 0 it means that
 * clock_refresh() has been called synchronously and no terminal
 * flushing is necessary at this point.
 */

void
clock_refresh(signum)
    int signum;
{
    int hour;
    char buf[16];
    struct tm *time;
    int line, column;
    tty_status_t status;

    if (in_terminal_mode())
	return;

    if (product_name_length + 2 + info_length >= title_window->columns)
	return;

    time = get_local_time();

    tty_save(&status);
    tty_get_cursor(&line, &column);

    tty_cursor(OFF);

    if ((hour = time->tm_hour % 12) == 0)
	hour = 12;

    sprintf(buf, "%2d:%02d%c", hour, time->tm_min,
	    (time->tm_hour < 12) ? 'a' : 'p');
    window_goto(title_window, 0, title_window->columns - 7);
    tty_colors(ClockBrightness, ClockForeground, ClockBackground);
    window_puts(title_window, buf, strlen(buf));

    tty_goto(line, column);
    tty_restore(&status);

    if (signum)
	tty_update();
}


void
title_update()
{
    int length;
    char *buf;
    tty_status_t status;

    tty_save(&status);

    tty_colors(TitleBrightness, TitleForeground, TitleBackground);

    window_goto(title_window, 0, 0);
    window_puts(title_window, product_name, product_name_length);

    buf = xmalloc(title_window->columns + 1);

    if (product_name_length + 2 + info_length < title_window->columns)
    {
	length = title_window->columns - product_name_length - info_length;

	assert(length > 0);

	memset(buf, ' ', length);
	window_puts(title_window, buf, length);

	window_goto(title_window, 0, product_name_length + length);
	window_puts(title_window, login_string, sizeof(login_string) - 1);
	window_putc(title_window, ' ');

	tty_foreground(UserName);
	window_puts(title_window, login_name, login_name_length);
	window_putc(title_window, ' ');
	window_putc(title_window, ' ');

	tty_foreground(TitleForeground);
	window_puts(title_window, ttydev_string, sizeof(ttydev_string) - 1);
	window_putc(title_window, ' ');

	tty_foreground(TtyName);
	window_puts(title_window, tty_device, tty_device_length);

	tty_foreground(TitleForeground);
	window_putc(title_window, ' ');

	clock_refresh(0);

	window_goto(title_window, 0, title_window->columns - 1);
	window_putc(title_window, ' ');
    }
    else if (product_name_length < title_window->columns)
    {
	length = title_window->columns - product_name_length;
	memset(buf, ' ', length);
	window_puts(title_window, buf, length);
    }

    xfree(buf);

    tty_restore(&status);
}
