/* title.c -- title bar.  */

/* Copyright (C) 1993-1999, 2006-2007 Free Software Foundation, Inc.

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

/* Written by Tudor Hulubei and Andrei Pitis.  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <wchar.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else /* !HAVE_STDLIB_H */
#include "ansi_stdlib.h"
#endif /* !HAVE_STDLIB_H */

#include <sys/types.h>

#if HAVE_UTIME_H
#include <utime.h>
#else /* !HAVE_UTIME_H */
#include <sys/utime.h>
#endif /* !HAVE_UTIME_H */

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include <assert.h>
#include <ctype.h>

#include "window.h"
#include "xmalloc.h"
#include "xstring.h"
#include "xio.h"
#include "tty.h"
#include "title.h"
#include "misc.h"
#include "git.h"
#include "common.h"

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


static wchar_t *product_name;

static int product_name_length;
static int login_name_length;
static int tty_device_length;

/* The length of the "User: tudor  tty: /dev/ttyp0" info string.
   It also includes 6 characters for the clock.  */
static int info_length;

static wchar_t login_string[] = L"User:";
static wchar_t ttydev_string[] = L"tty:";

static wchar_t mail_have_none[] = L"";
static wchar_t mail_have_mail[] = L"(Mail)";
static wchar_t mail_have_new[]  = L"(New Mail)";

static wchar_t *mail_string = L"";
static char *mail_file=NULL;
static off64_t mail_size=0;
static time_t mail_mtime=0;

static window_t *title_window;

static int calc_info_length PROTO ((void));
static int mail_check PROTO ((void));


static int
calc_info_length()
{
    info_length = wcslen(login_string)  + 1 + login_name_length + 1 +
	          wcslen(mail_string)   + 1 +
		  wcslen(ttydev_string) + 1 + tty_device_length + 1 +
		  6 + 1;
    return(info_length);
}


void
title_init()
{
    int begy, begx, maxy, maxx;
    wchar_t *product=mbsduptowcs(PRODUCT);
    wchar_t *version=mbsduptowcs(VERSION);
    int namelen= 1 + wcslen(product) + 1 + wcslen(version) + 1; 
    product_name = xmalloc(namelen * sizeof(wchar_t));
    swprintf(product_name, namelen, L" %ls %ls", product, version);
    product_name_length = wcslen(product_name);
    login_name_length = wcslen(login_name);
    tty_device_length = wcslen(tty_device);

    mail_file=getenv("MAIL");
    if(mail_file)
    {
	struct stat s;
	if(xstat(mail_file,&s) != -1)
	{
	    mail_size=s.st_size;
	    mail_mtime=s.st_mtime;
	}
    }
    mail_check();
    info_length = calc_info_length();
    getbegyx(stdscr, begy, begx);
    getmaxyx(stdscr, maxy, maxx);
    title_window  = window_init(1, maxx, begy,   begx);
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
 * Update the title clock and check for mail.  If signum is 0 it means that
 * clock_refresh() has been called synchronously and no terminal
 * flushing is necessary at this point.
 */

void
clock_refresh(signum)
    int signum;
{
    int hour;
    wchar_t buf[16];
    struct tm *time;
    int line, column;
    tty_status_t status;

    if (in_terminal_mode())
	return;

    if (product_name_length + 2 + info_length >= title_window->columns)
	return;

    /* signum means we weren't called from title_update */
    if(signum && mail_check())
    {
	title_update();
    }

    time = get_local_time();

    tty_save(&status);
    tty_get_cursor(&line, &column);

    tty_cursor(OFF);

    if ((hour = time->tm_hour % 12) == 0)
	hour = 12;

    swprintf(buf, 16, L"%2d:%02d%c", hour, time->tm_min,
	     (time->tm_hour < 12) ? 'a' : 'p');
    window_goto(title_window, 0, title_window->columns - 7);
    tty_colors(ClockBrightness, ClockForeground, ClockBackground);
    window_puts(title_window, buf, wcslen(buf));

    tty_goto(title_window->window, line, column);
    tty_restore(&status);

    if (signum)
	tty_update();
}

static int
mail_check()
{
    wchar_t *old_mail=mail_string;
    mail_string=mail_have_none;
    int total=0;
    int read=0;
    int inheaders=0;
    int gotstatus=0;
    FILE *mbox;
#define TMPBUFSIZE 2048
    char line[TMPBUFSIZE];
    struct stat s;
    struct utimbuf utbuf;

    if(!mail_file)
	return 0;
    if(xstat(mail_file,&s) == -1)
	return 0;
    utbuf.actime=s.st_atime;
    utbuf.modtime=s.st_mtime;

    mbox=fopen(mail_file,"r");
    if(!mbox)
	return 0;

    while(fgets(line,TMPBUFSIZE,mbox))
    {
	if(strcmp(line,"")==0)
	    inheaders=0;
	else if(strncmp(line,"From ",strlen("From ")) == 0)
	{
	    inheaders=1;
	    gotstatus=0;
	    total++;
	}
	else if(inheaders && !gotstatus &&
		(strncmp(line,"Status:",strlen("Status:"))==0))
	{
	    char *status=strchr(line,':');
	    status++;
	    while(*status && isspace(*status))
		status++;
	    if(*status)
	    {
		gotstatus=1;
		if(strchr(status,'R'))
		    read++;
	    }
	}
    }
    fclose(mbox);
    utime(mail_file,&utbuf);

    if(total)
    {
	if(total-read)
	{
	    mail_string=mail_have_new;
	}
	else
	{
	    mail_string=mail_have_mail;
	}
    }
    info_length=calc_info_length();
    if(wcscmp(mail_string,old_mail) == 0)
	return 0; /* No change  */
    else
	return 1; /* need to update title */
}

void
title_update()
{
    int length;
    wchar_t *buf;
    tty_status_t status;

    tty_save(&status);

    tty_colors(TitleBrightness, TitleForeground, TitleBackground);

    window_goto(title_window, 0, 0);
    window_puts(title_window, product_name, product_name_length);

    buf = xmalloc((title_window->columns + 1) * sizeof(wchar_t));

    mail_check();
    if (product_name_length + 2 + info_length < title_window->columns)
    {
	length = title_window->columns - product_name_length - info_length;

	assert(length > 0);

	wmemset(buf, L' ', length);
	window_puts(title_window, buf, length);

	window_goto(title_window, 0, product_name_length + length);
	window_puts(title_window, login_string, wcslen(login_string));
	window_putc(title_window, L' ');

	tty_foreground(UserName);
	window_puts(title_window, login_name, login_name_length);
	window_putc(title_window, L' ');

	window_puts(title_window, mail_string, wcslen(mail_string));

	window_putc(title_window, L' ');

	tty_foreground(TitleForeground);
	window_puts(title_window, ttydev_string, wcslen(ttydev_string));
	window_putc(title_window, L' ');

	tty_foreground(TtyName);
	window_puts(title_window, tty_device, tty_device_length);

	tty_foreground(TitleForeground);
	window_putc(title_window, L' ');

	clock_refresh(0);

	window_goto(title_window, 0, title_window->columns - 1);
	window_putc(title_window, L' ');
    }
    else if (product_name_length < title_window->columns)
    {
	length = title_window->columns - product_name_length;
	wmemset(buf, L' ', length);
	window_puts(title_window, buf, length);
    }

    xfree(buf);

    tty_restore(&status);
}
