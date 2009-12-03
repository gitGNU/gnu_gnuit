/* status.c -- A simple status line management file.  */

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
#include <stdlib.h>
#include <sys/types.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef HAVE_UTSNAME
#include <sys/utsname.h>
#endif

#include <assert.h>

#include "xtime.h"
#include "xio.h"

#include "xstring.h"
#include "xmalloc.h"
#include "window.h"
#include "status.h"
#include "configure.h"
#include "tty.h"
#include "misc.h"


extern int AnsiColors;


extern window_t *status_window;
static wchar_t *status_message;
static char status_type;
static char status_alignment;
static wchar_t *status_buffer;
static wchar_t *status_default_message;

#ifdef HAVE_UTSNAME
static struct utsname u;
#endif


#define STATUSBAR_FIELDS        9

static char *StatusBarFields[STATUSBAR_FIELDS] =
{
    "StatusBarForeground",
    "StatusBarBackground",
    "StatusBarBrightness",
    "StatusBarWarningForeground",
    "StatusBarWarningBackground",
    "StatusBarWarningBrightness",
    "StatusBarErrorForeground",
    "StatusBarErrorBackground",
    "StatusBarErrorBrightness"
};

static int StatusBarColors[STATUSBAR_FIELDS] =
{
    BLACK, CYAN, OFF, BLACK, WHITE, OFF, WHITE, RED, ON
};

#define StatusBarForeground             StatusBarColors[0]
#define StatusBarBackground             StatusBarColors[1]
#define StatusBarBrightness             StatusBarColors[2]
#define StatusBarWarningForeground      StatusBarColors[3]
#define StatusBarWarningBackground      StatusBarColors[4]
#define StatusBarWarningBrightness      StatusBarColors[5]
#define StatusBarErrorForeground        StatusBarColors[6]
#define StatusBarErrorBackground        StatusBarColors[7]
#define StatusBarErrorBrightness        StatusBarColors[8]


void
status_init(default_message)
    wchar_t *default_message;
{
    use_section(AnsiColors ? color_section : monochrome_section);

    get_colorset_var(StatusBarColors, StatusBarFields, STATUSBAR_FIELDS);

    status_default_message = default_message;
    toprintable(status_default_message, wcslen(status_default_message));
    status_window = window_init(1, COLS, LINES-1, 0);

#ifdef HAVE_UTSNAME
    uname(&u);
#endif
}


void
status_end()
{
    window_end(status_window);
}


void
status_resize(columns, line)
    size_t columns, line;
{
    if (status_buffer)
	xfree(status_buffer);

    status_buffer = xmalloc(columns * sizeof(wchar_t));

    window_resize(status_window, 0, line, 1, columns);
}


static void
build_message()
{
    int i, j;
    struct tm *time;
    wchar_t date_str[32];
    char *ptr;
    wchar_t *temp_msg;
    size_t len, temp_msg_len;

    assert(status_message);

    wmemset(status_buffer, L' ', status_window->wcolumns);
    temp_msg_len = wcslen(status_message);
    temp_msg = xmalloc((temp_msg_len+1) * sizeof(wchar_t));

    for (i = 0, j = 0; status_message[i]; i++)
	if (status_message[i] == L'\\')
	    switch (status_message[i + 1])
	    {
		case 'h' :
#ifdef HAVE_UTSNAME
		    ptr = u.nodename;
#else
		    ptr = "";
#endif
		    goto get_system_info;

		case 's' :
#ifdef HAVE_UTSNAME
		    ptr = u.sysname;
#else
		    ptr = "";
#endif
		    goto get_system_info;

		case 'm' :
#ifdef HAVE_UTSNAME
		    ptr = u.machine;
#else
		    ptr = "";
#endif
		  get_system_info:
		    if (ptr[0])
		    {
			len = mbstowcs(NULL,ptr,0);
			temp_msg = xrealloc(temp_msg, ((temp_msg_len += len) * sizeof(wchar_t)));
			mbstowcs(&temp_msg[j], ptr, len);
		    }
		    else
		    {
			len = 6;
			temp_msg = xrealloc(temp_msg,  ((temp_msg_len += len) * sizeof(wchar_t)));
			wmemcpy(&temp_msg[j], L"(none)", len);
		    }

		    j += len;
		    i++;
		    break;

		case 'd' :
		    time = get_local_time();
		    swprintf(date_str, 31, L"%s %s %02d %d",
			    day_name[time->tm_wday], month_name[time->tm_mon],
			    time->tm_mday, time->tm_year + 1900);
		    len = wcslen(date_str);
		    temp_msg = xrealloc(temp_msg,  ((temp_msg_len += len) * sizeof(wchar_t)));
		    wmemcpy(&temp_msg[j], date_str, len);
		    j += len;
		    i++;
		    break;

		case '\\':
		    temp_msg[j++] = '\\';
		    i++;
		    break;

		case '\0':
		    temp_msg[j++] = '\\';
		    break;

		default:
		    temp_msg[j++] = '\\';
		    temp_msg[j++] = status_message[++i];
		    break;
	    }
	else
	{
	    if (status_message[i] == '\t')
	    {
		temp_msg = xrealloc(temp_msg, ((temp_msg_len += 8) * sizeof(wchar_t)));
		wmemcpy(&temp_msg[j], L"        ", (8*sizeof(wchar_t)));
		j += 8;
	    }
	    else
		temp_msg[j++] = status_message[i];
	}

    temp_msg[j] = 0;

    len = wcslen(temp_msg);

    if (status_alignment == STATUS_CENTERED &&
	(int)len < status_window->wcolumns)
	wmemcpy(status_buffer + ((status_window->wcolumns - len) >> 1),
		temp_msg, len);
    else
	wmemcpy(status_buffer, temp_msg, min((int)len, status_window->wcolumns));

    xfree(temp_msg);

    for (i = 0; i < status_window->wcolumns; i++)
	if (status_buffer[i] == L'\r' || status_buffer[i] == L'\n')
	    status_buffer[i] = L' ';
}


void
status_update()
{
    tty_status_t status;
    tty_save(&status);

    build_message();

    switch (status_type)
    {
	case STATUS_WARNING:
	    tty_colors(StatusBarWarningBrightness,
		       StatusBarWarningForeground,
		       StatusBarWarningBackground);
	    break;

	case STATUS_ERROR:
	    tty_colors(StatusBarErrorBrightness,
		       StatusBarErrorForeground,
		       StatusBarErrorBackground);
	    break;

	default:
	    tty_colors(StatusBarBrightness,
		       StatusBarForeground,
		       StatusBarBackground);
	    break;
    }

    window_goto(status_window, 0, 0);
    window_puts(status_window, status_buffer, status_window->wcolumns);

    tty_restore(&status);
}


void
status(message, type, alignment)
    wchar_t *message;
    int type, alignment;
{
    if (status_message)
	xfree(status_message);

    status_message = xwcsdup(message);
    toprintable(status_message, wcslen(status_message));
    status_type = type;
    status_alignment = alignment;

    status_update();
}


void
status_default()
{
    status(xwcsdup(status_default_message), STATUS_OK, STATUS_CENTERED);
}
