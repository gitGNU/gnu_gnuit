/* tty.c -- The tty management file.  */

/* Copyright (C) 1993-2000, 2006-2009 Free Software Foundation, Inc.

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

#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif

#ifdef HAVE_NCURSESW_CURSES_H
#include <ncursesw/curses.h>
#else
#ifdef HAVE_CURSES_H
#include <curses.h>
#endif	/* HAVE_CURSES_H */
#endif	/* HAVE_NCURSESW_CURSES_H */

#ifdef HAVE_TERM_H
#include <term.h>
#endif	/* HAVE_TERM_H */
#include <stdlib.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPE_H */
#include <stddef.h>
#include <ctype.h>
#include "file.h"
#include <sys/ioctl.h>
#include <unistd.h>

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif /* HAVE_ASSERT_H */
#include "stdc.h"
#include "xstring.h"
#include "xmalloc.h"
#include "xio.h"
#include "tty.h"
#include "signals.h"
#include "misc.h"

/* Stolen from GNU Emacs.  */
#ifdef _POSIX_VDISABLE
#define CDISABLE _POSIX_VDISABLE
#else /* not _POSIX_VDISABLE */
#ifdef CDEL
#undef CDISABLE
#define CDISABLE CDEL
#else /* not CDEL */
#define CDISABLE 255
#endif /* not CDEL */
#endif /* not _POSIX_VDISABLE */

#define MAX_TTY_COLUMNS         1024
#define MAX_TTY_LINES           1024

extern window_t *title_window, *status_window;

#define TTY_INPUT       0
#define TTY_OUTPUT      1

/* If tty_kbdmode == TTY_FULL_INPUT, single character key sequences
   are inserted into the linked list.  This feature is used by gitps
   which has no command line.  */
static int tty_kbdmode;

#ifdef HAVE_POSIX_TTY
static struct termios old_term;
#else
#ifdef HAVE_SYSTEMV_TTY
static struct termio old_term;
#else
static struct sgttyb  old_arg;
static struct tchars  old_targ;
static struct ltchars old_ltarg;

/* NextStep doesn't define TILDE.  */
#ifndef TILDE
#define TILDE 0
#endif

#endif /* HAVE_SYSTEMV_TTY */
#endif /* HAVE_POSIX_TTY */

int tty_lines;
int tty_columns;
wchar_t *tty_device;
static char *tty_device_str;

static unsigned char *tty_key_seq;

static int tty_device_length;
static int tty_last_char_flag;

/*
 * tty_*_current_attribute:
 *   bit 7:		reverse video
 *   bit 6:		brightness
 *   bits 5,4,3:	background color
 *   bits 2,1,0:	foreground color
 */
static int tty_current_attribute;
static int tty_current_fg;
static int tty_current_bg;
static int tty_next_free_color_pair;

/* The current interrupt character.  We need to restore when resuming
   after a suspend.  */
static int tty_interrupt_char = key_INTERRUPT;

/* These variable tells us if we should use standard ANSI color sequences.
   Its value is taken from the configuration file.  */
int AnsiColors = ON;

/* Structures for keys management.  */
tty_key_t *key_list_head;
tty_key_t *current_key;
tty_key_t default_key;

#ifndef HAVE_LINUX
static char term_buf[2048];
#endif
static char vt100[] = "vt100";

/* The terminal mode. TTY_CANONIC at the begining.  */
int tty_mode = TTY_CANONIC;

char *tty_type;

/* A structure describing some attributes we need to know about each
   capability. See below for greater detail.  */
typedef struct
{
    char *name;		/* the capability name.  */

    /* These ones should be in an union, but the HP-UX non ANSI compiler
       complains about union initialization being an ANSI feature and I
       care more for portability than for the memory used.  */
    char *string;	/* The string  value of the capability.  */
    int  integer;	/* The integer value of the capability.  */

    int required;	/* This capability is required.  */
    char *symbol;	/* The human readable form of the key name.  */
} tty_capability_t;


#define TTY_CAPABILITIES_USED   21
#define TTY_FIRST_SYMBOL_KEY    0

static tty_capability_t tty_capability[TTY_CAPABILITIES_USED] =
{
    { "ku", NULL, 0, 0, "UP" },		/* (UP) */
    { "kd", NULL, 0, 0, "DOWN" },	/* (DOWN) */
    { "kr", NULL, 0, 0, "RIGHT" },	/* (RIGHT) */
    { "kl", NULL, 0, 0, "LEFT" },	/* (LEFT) */
    { "kI", NULL, 0, 0, "INS" },	/* (INS) */
    { "kD", NULL, 0, 0, "DEL" },	/* (DEL) */
    { "kh", NULL, 0, 0, "HOME" },	/* (HOME) */
    { "@7", NULL, 0, 0, "END" },	/* (END) */
    { "kP", NULL, 0, 0, "PGUP" },	/* (PGUP) */
    { "kN", NULL, 0, 0, "PGDOWN" },	/* (PGDOWN) */
    { "k0", NULL, 0, 0, "F0" },		/* (F0) */
    { "k1", NULL, 0, 0, "F1" },		/* (F1) */
    { "k2", NULL, 0, 0, "F2" },		/* (F2) */
    { "k3", NULL, 0, 0, "F3" },		/* (F3) */
    { "k4", NULL, 0, 0, "F4" },		/* (F4) */
    { "k5", NULL, 0, 0, "F5" },		/* (F5) */
    { "k6", NULL, 0, 0, "F6" },		/* (F6) */
    { "k7", NULL, 0, 0, "F7" },		/* (F7) */
    { "k8", NULL, 0, 0, "F8" },		/* (F8) */
    { "k9", NULL, 0, 0, "F9" },		/* (F9) */
    { "k;", NULL, 0, 0, "F10" },	/* (F10) */
};

static char term_database[] = "terminfo";
static char term_env[]      = "TERMINFO";

static int  tty_is_xterm PROTO ((char *));
static int tty_get_color_pair PROTO ((short, short));
static void tty_update_attributes PROTO ((void));

extern void fatal PROTO ((char *));


/* ANSI colors. OFF & ON are here because we need them when we read the
   configuration file. Don't bother...  */
static char *colors[10] =
{
    "BLACK",
    "RED",
    "GREEN",
    "YELLOW",
    "BLUE",
    "MAGENTA",
    "CYAN",
    "WHITE",
    "OFF",
    "ON"
};


/* Control keys description. C-a, C-b, C-c and so on...  */
unsigned char key_ctrl_table[0x5f] =
{
    0x20,                       /* 0x20 ( ) */
    0x21,                       /* 0x21 (!) */
    0x22,                       /* 0x22 (") */
    0x23,                       /* 0x23 (#) */
    0xff,                       /* 0x24 ($) */
    0x25,                       /* 0x25 (%) */
    0x26,                       /* 0x26 (&) */
    0x07,                       /* 0x27 (') */
    0x28,                       /* 0x28 (() */
    0x29,                       /* 0x29 ()) */
    0x2a,                       /* 0x2a (*) */
    0x2b,                       /* 0x2b (+) */
    0x2c,                       /* 0x2c (,) */
    0x2d,                       /* 0x2d (-) */
    0x2e,                       /* 0x2e (.) */
    0x2f,                       /* 0x2f (/) */
    0x20,                       /* 0x30 (0) */
    0x20,                       /* 0x31 (1) */
    0xff,                       /* 0x32 (2) */
    0x1b,                       /* 0x33 (3) */
    0x1c,                       /* 0x34 (4) */
    0x1d,                       /* 0x35 (5) */
    0x1e,                       /* 0x36 (6) */
    0x1f,                       /* 0x37 (7) */
    0x7f,                       /* 0x38 (8) */
    0x39,                       /* 0x39 (9) */
    0x3a,                       /* 0x3a (:) */
    0x3b,                       /* 0x3b (;) */
    0x3c,                       /* 0x3c (<) */
    0x20,                       /* 0x3d (=) */
    0x3e,                       /* 0x3e (>) */
    0x20,                       /* 0x3f (?) */
    0x20,                       /* 0x40 (@) */
    0x01,                       /* 0x41 (A) */
    0x02,                       /* 0x42 (B) */
    0x03,                       /* 0x43 (C) */
    0x04,                       /* 0x44 (D) */
    0x05,                       /* 0x45 (E) */
    0x06,                       /* 0x46 (F) */
    0x07,                       /* 0x47 (G) */
    0x08,                       /* 0x48 (H) */
    0x09,                       /* 0x49 (I) */
    0x0a,                       /* 0x4a (J) */
    0x0b,                       /* 0x4b (K) */
    0x0c,                       /* 0x4c (L) */
    0x0d,                       /* 0x4d (M) */
    0x0e,                       /* 0x4e (N) */
    0x0f,                       /* 0x4f (O) */
    0x10,                       /* 0x50 (P) */
    0x11,                       /* 0x51 (Q) */
    0x12,                       /* 0x52 (R) */
    0x13,                       /* 0x53 (S) */
    0x14,                       /* 0x54 (T) */
    0x15,                       /* 0x55 (U) */
    0x16,                       /* 0x56 (V) */
    0x17,                       /* 0x57 (W) */
    0x18,                       /* 0x58 (X) */
    0x19,                       /* 0x59 (Y) */
    0x1a,                       /* 0x5a (Z) */
    0x1b,                       /* 0x5b ([) */
    0x1c,                       /* 0x5c (\) */
    0x1d,                       /* 0x5d (]) */
    0x5e,                       /* 0x5e (^) */
    0x7f,                       /* 0x5f (_) */
    0x20,                       /* 0x60 (`) */
    0x01,                       /* 0x61 (a) */
    0x02,                       /* 0x62 (b) */
    0x03,                       /* 0x63 (c) */
    0x04,                       /* 0x64 (d) */
    0x05,                       /* 0x65 (e) */
    0x06,                       /* 0x66 (f) */
    0x07,                       /* 0x67 (g) */
    0x08,                       /* 0x68 (h) */
    0x09,                       /* 0x69 (i) */
    0x0a,                       /* 0x6a (j) */
    0x0b,                       /* 0x6b (k) */
    0x0c,                       /* 0x6c (l) */
    0x0d,                       /* 0x6d (m) */
    0x0e,                       /* 0x6e (n) */
    0x0f,                       /* 0x6f (o) */
    0x10,                       /* 0x70 (p) */
    0x11,                       /* 0x71 (q) */
    0x12,                       /* 0x72 (r) */
    0x13,                       /* 0x73 (s) */
    0x14,                       /* 0x74 (t) */
    0x15,                       /* 0x75 (u) */
    0x16,                       /* 0x76 (v) */
    0x17,                       /* 0x77 (w) */
    0x18,                       /* 0x78 (x) */
    0x19,                       /* 0x79 (y) */
    0x1a,                       /* 0x7a (z) */
    0x20,                       /* 0x7b ({) */
    0x20,                       /* 0x7c (|) */
    0x20,                       /* 0x7d (}) */
    0x20,                       /* 0x7e (~) */
};


#define NO      0
#define YES     1

#define MAX_KEY_LENGTH	15

static int  keyno    = 0;
static int  keyindex = 0;
static char keybuf[1024];
static unsigned char keystr[MAX_KEY_LENGTH * 20];
static int partial = 0;
static int key_on_display = 0;

/* Hooks that get called when we are going idle and when we stop
   being idle.  */
void (* tty_enter_idle_hook)();
void (* tty_exit_idle_hook)();


void
tty_set_last_char_flag(last_char_flag)
    int last_char_flag;
{
    tty_last_char_flag = last_char_flag;
}

int
tty_get_last_char_flag()
{
    return tty_last_char_flag;
}


/*
 * Set up term settings for keys in noncanonic mode that aren't
 * handled by curses.  Also turn off canonical mode so we can have
 * single-char input outside curses (for "press almost any key to
 * continue" message)
 */
void
tty_noncanonic()
{
#ifdef HAVE_POSIX_TTY
    struct termios current_term;
#else
#ifdef HAVE_SYSTEMV_TTY
    struct termio current_term;
#else
    struct sgttyb current_arg;
    struct tchars  current_targ;
    struct ltchars current_ltarg;
#endif /* HAVE_SYSTEMV_TTY */
#endif /* HAVE_POSIX_TTY */

#ifdef HAVE_POSIX_TTY
    tcgetattr(TTY_OUTPUT, &current_term);
    current_term.c_lflag &= ~(ICANON | ECHO);
    current_term.c_cc[VINTR] = key_INTERRUPT;		/* Ctrl-G */
    current_term.c_cc[VQUIT] = CDISABLE;
#ifdef VSTART
    current_term.c_cc[VSTART] = CDISABLE;		/* START (^Q) */
#endif
#ifdef VSTOP
    current_term.c_cc[VSTOP] = CDISABLE;		/* STOP (^S) */
#endif
#ifdef VSUSP
    current_term.c_cc[VSUSP] = key_SUSPEND;             /* Ctrl-Z */
#endif
    tcsetattr(TTY_OUTPUT, TCSADRAIN, &current_term);
#else

#ifdef HAVE_SYSTEMV_TTY
    ioctl(TTY_OUTPUT, TCGETA, &current_term);
    current_term.c_lflag &= ~(ICANON | ECHO);
    current_term.c_cc[VINTR] = key_INTERRUPT;	/* Ctrl-G */
    current_term.c_cc[VQUIT] = CDISABLE;
#ifdef VSTART
    current_term.c_cc[VSTART] = CDISABLE;	/* START (^Q) */
#endif
#ifdef VSTOP
    current_term.c_cc[VSTOP] = CDISABLE;	/* STOP (^S) */
#endif
    ioctl(TTY_OUTPUT, TCSETAW, &current_term);
#else
    ioctl(TTY_OUTPUT, TIOCGETC, &current_targ);
    ioctl(TTY_OUTPUT, TIOCGLTC, &current_ltarg);
    current_arg.sg_flags   &= ~(ECHO | CBREAK);
    current_targ.t_intrc   = key_INTERRUPT;     /* Ctrl-G */
    current_targ.t_quitc   = CDISABLE;
    current_targ.t_stopc   = CDISABLE;
    current_targ.t_startc  = CDISABLE;
    current_ltarg.t_suspc  = key_SUSPEND;	/* Ctrl-Z */
    ioctl(TTY_OUTPUT, TIOCSETN, &current_arg);
    ioctl(TTY_OUTPUT, TIOCSETC, &current_targ);
    ioctl(TTY_OUTPUT, TIOCSLTC, &current_ltarg);
#endif /* HAVE_SYSTEMV_TTY */
#endif /* HAVE_POSIX_TTY */

    /* Make sure we restore the interrupt character that was in
       use last time when we used NONCANONICAL mode.  */
    tty_set_interrupt_char(tty_interrupt_char);
}

/*
 * This function is used to switch between canonical and noncanonical
 * terminal modes.
 */
void
tty_set_mode(mode)
    int mode;
{
    if (mode == TTY_NONCANONIC)
    {
	cbreak();
	tty_noncanonic();
    }
    else
    {
	nocbreak();
    }
    tty_mode = mode;
}


int
tty_get_mode()
{
    return tty_mode;
}

/* restore original tty settings */
void tty_restore_term()
{
#ifdef HAVE_POSIX_TTY
        tcsetattr(TTY_OUTPUT, TCSADRAIN, &old_term);
#else
#ifdef HAVE_SYSTEMV_TTY
        ioctl(TTY_OUTPUT, TCSETAW, &old_term);
#else
        ioctl(TTY_OUTPUT, TIOCSETN, &old_arg);
        ioctl(TTY_OUTPUT, TIOCSETC, &old_targ);
        ioctl(TTY_OUTPUT, TIOCSLTC, &old_ltarg);
#endif /* HAVE_SYSTEMV_TTY */
#endif /* HAVE_POSIX_TTY */
}



/*
 * Set the interrupt character.
 */
void
tty_set_interrupt_char(c)
    int c;
{
#ifdef HAVE_POSIX_TTY
    struct termios current_term;

    tcgetattr(TTY_OUTPUT, &current_term);
    current_term.c_cc[VINTR] = c;
    current_term.c_cc[VQUIT] = CDISABLE;
    tcsetattr(TTY_OUTPUT, TCSADRAIN, &current_term);
#else
#ifdef HAVE_SYSTEMV_TTY
    struct termio current_term;

    ioctl(TTY_OUTPUT, TCGETA, &current_term);
    current_term.c_cc[VINTR] = c;
    current_term.c_cc[VQUIT] = CDISABLE;
    ioctl(TTY_OUTPUT, TCSETAW, &current_term);
#else
    struct tchars current_targ;

    ioctl(TTY_OUTPUT, TIOCGETC, &current_targ);
    current_targ.t_intrc = c;
    current_targ.t_quitc = CDISABLE;
    ioctl(TTY_OUTPUT, TIOCSETC, &current_targ);
#endif /* HAVE_SYSTEMV_TTY */
#endif /* HAVE_POSIX_TTY */

    tty_interrupt_char = c;
}

void
tty_flush()
{
    refresh();
}

void
tty_start_cursorapp()
{
    tty_update();
    tty_noncanonic();
}

void
tty_end_cursorapp()
{
    endwin();
    tty_restore_term();
}


/*
 * This function is called to restore the canonic mode at exit.  It also
 * clears the screen (or restore it, if possible) and sets the default
 * attributes.  If screen is NULL, there was an error, so don't try to
 * restore it.
 */
void
tty_end(screen)
    char *screen;
{
    if (tty_mode == TTY_NONCANONIC)
	tty_set_mode(TTY_CANONIC);

    tty_defaults();
    tty_end_cursorapp();
    tty_flush();
    endwin();
    printf("\n");
}


/*
 * Converts a key sequence from the human readable, '^' based form into a
 * machine usable form.
 */
char *
tty_key_human2machine(key_seq)
    unsigned char *key_seq;
{
    unsigned char *first;
    unsigned char *second;

    first = second = key_seq;

    if (tty_kbdmode == TTY_RESTRICTED_INPUT && *key_seq != '^')
	return NULL;

    while (*second)
    {
	if (*second == '^')
	{
	    if (*++second)
	    {
		/* ^G is the interrupt character and ^Z the suspend one.
		   They are not allowed in key sequences.  */
		if (toupper(*second) == 'G' || toupper(*second) == 'Z')
		    return NULL;

		*first++ = key_ctrl_table[(*second++ & 0x7F) - ' '];
	    }
	    else
		return NULL;
	}
	else
	    *first++ = *second++;
    }

    *first = 0;
    return (char *)key_seq;
}


/*
 * Converts a partial key sequence from the machine form into the human
 * readable, '^' based form.
 */
unsigned char *
tty_key_machine2human(key_seq)
    char *key_seq;
{
    unsigned char *ptr;

    keystr[0] = '\0';

    for (ptr = (unsigned char *)key_seq; *ptr; ptr++)
    {
	if (ptr != (unsigned char *)key_seq)
	    strcat((char *)keystr, " ");

	if (*ptr == key_ESC)
	    strcat((char *)keystr, "escape");
	else if (*ptr == ' ')
	    strcat((char *)keystr, "space");
	else if (*ptr == key_BACKSPACE)
	    strcat((char *)keystr, "backspace");
	else if (*ptr == key_CTRL_SPACE)
	    strcat((char *)keystr, "^space");
	else if (iscntrl(*ptr))
	{
	    char x[3];
	    x[0] = '^';
	    x[1] = *ptr + 'A' - 1;
	    x[2] = '\0';
	    strcat((char *)keystr, x);
	}
	else
	{
	    char x[2];
	    x[0] = *ptr;
	    x[1] = '\0';
	    strcat((char *)keystr, x);
	}
    }

    return (unsigned char *)keystr;
}

/*
 * Update the tty screen.
 */
void
tty_update()
{
    refresh();
}

/*
 * Write a string to the screen, at the current cursor position.
 * If the string is too long to fit between the current cursor
 * position and the end of the line, it is truncated (i.e. it doesn't
 * wrap around).  Return the number of characters written.
 */
int
tty_puts(buf, length)
    wchar_t *buf;
    int length;
{
#ifdef REMOVEME
    /* FIXME: what about bounds checking? */
    if (x >= tty_columns)
	return 0;

    if (tty_cursor_y >= tty_lines)
	return 0;

    /* If the string doesn't fit completely, adjust the length.  */
    if (x + length > tty_columns)
	length = tty_columns - x;

    tty_offset = (tty_cursor_y * tty_columns) + x;
#endif
    addnwstr(buf, length);
    return length;
}


/*
 * Write a character to the screen.
 */
int
tty_putc(c)
    wchar_t c;
{
    wchar_t character = c;
    int ret=tty_puts(&character, 1);
    return ret;
}


/*
 * Read data from the terminal.
 */
int
tty_read(buf, length)
    char *buf;
    int length;
{
    int bytes;

    if (tty_enter_idle_hook)
	(*tty_enter_idle_hook)();

    bytes = xread(TTY_INPUT, buf, length);

    if (tty_exit_idle_hook)
	(*tty_exit_idle_hook)();

    return bytes;
}


/*
 * Clear the screen using the current color attributes.
 */
void
tty_clear()
{
    clear();
}


/*
 * Fill the terminal screen with spaces & the current attribute.
 */
void
tty_fill()
{
    tty_clear();
    tty_touch();
    tty_update();
}


void
tty_touch()
{
    clearok(curscr,1);
}

/*
 * Move the cursor.
 */
void
tty_goto(y, x)
    int y, x;
{
    move(y, x);
}

/*
 * Set the foreground color.
 */
void
tty_foreground(color)
    int color;
{
    tty_current_fg=color;
    tty_update_attributes();
}


/*
 * Set the background color.
 */
void
tty_background(color)
    int color;
{
    tty_current_bg = color;
    tty_update_attributes();
}


/*
 * Set the brightness status.
 */
void
tty_brightness(status)
    int status;
{
    if(status)
	tty_current_attribute |= A_BOLD;
    else
	tty_current_attribute &= ~A_BOLD;
    tty_update_attributes();
}

/*
 * Set the brightness, foreground and background all together.
 */
void
tty_colors(brightness, foreground, background)
    int brightness, foreground, background;
{
    tty_current_attribute=A_NORMAL;
    if(brightness)
	    tty_current_attribute |= A_BOLD;
    tty_current_fg=foreground;
    tty_current_bg=background;
    tty_update_attributes();
}

static void
tty_update_attributes()
{
    int cp;
    if(AnsiColors == ON)
    {
	cp=tty_get_color_pair(tty_current_fg, tty_current_bg);
	attrset(tty_current_attribute);
	color_set(cp, NULL);
    }
    else
    {
	if(((tty_current_fg != WHITE) || (tty_current_bg != BLACK)))
	    tty_current_attribute |= A_REVERSE;
	attrset(tty_current_attribute);
    }
}

static int
tty_get_color_pair(short fg, short bg)
{
    int i;
    int pairnum=-1;
    short thisfg, thisbg;

    /* FIXME: This is horribly inefficient, although hopefully not
       enough to be noticeable. Can we use PAIR_NUMBER instead? */
    for(i=0; i < tty_next_free_color_pair; i++)
    {
	pair_content(i, &thisfg, &thisbg);
	if((fg == thisfg) && (bg == thisbg))
	{
	    pairnum=i;
	    break;
	}
    }
    if(pairnum == -1)
    {
	pairnum=tty_next_free_color_pair++;
	init_pair(pairnum, fg, bg);
    }
    return pairnum;
}

/*
 * Beep :-)
 */
void
tty_beep()
{
    beep();
}


/*
 * Set the cursor status (where possible). Make it invisible during screen
 * refreshes and restore it afterward.
 */
void
tty_cursor(status)
    int status;
{
    if (status)
	curs_set(1);
    else
	curs_set(0);
}


/*
 * Store the software terminal status in a tty_status_t structure.
 */
void
tty_save(status)
    tty_status_t *status;
{
    status->attribute = tty_current_attribute;
    status->fg = tty_current_fg;
    status->bg = tty_current_bg;
}


/*
 * Restore the software terminal status from a tty_status_t structure.
 */
void
tty_restore(status)
    tty_status_t *status;
{
    tty_current_attribute = status->attribute;
    tty_current_fg = status->fg;
    tty_current_bg = status->bg;
    tty_update_attributes();
}


/*
 * Restore the terminal to its default state.
 */
void
tty_defaults()
{
    short fg, bg;
    tty_current_attribute=A_NORMAL;
    if(AnsiColors == ON)
    {
	/* pair 0 == terminal default, at least under ncurses */
	pair_content(0, &fg, &bg);
	tty_current_fg=fg;
	tty_current_bg=bg;
    }
    else
    {
	tty_current_fg=WHITE;
	tty_current_bg=BLACK;
    }
    tty_update_attributes();
}


/*
 * Extract the first key in the buffer.  If the 8th bit is set, return
 * an ESC char first, then the key without the 8th bit.
 *
 * OLD-FIX-ME: It seems that the 8th bit can be used for parity as well.
 * This case is not handled yet.  It should be configurable.
 */
static int
tty_extract_key()
{
    int key = keybuf[keyindex];

    if (key & 0x80)
    {
	keybuf[keyindex] &= 0x7F;
	return key_ESC;
    }

    keyno--;
    keyindex++;
    return key;
}


/*
 * Get a character from the terminal.  For better performance on
 * highly loaded systems, read all the data available.
 */
int
tty_getc()
{
    service_pending_signals();

    if (keyno)
	return tty_extract_key();

    /* No interrupt/quit character.  */
    tty_set_interrupt_char(-1);

    /* Allow signals to suspend/resize git.  */
    signals(ON);

    keyindex = 0;
    do /* FIXME: reinstate */
	/*tty_update()*/; 
    while ((keyno = tty_read(keybuf, 1024)) < 0);

    /* Prevent signals from suspending/resizing git.  */
    signals(OFF);

    /* Restore ^G as the interrupt/quit character.  */
    tty_set_interrupt_char(key_INTERRUPT);

    return keyno ? tty_extract_key() : -1;
}


/*
 * Insert a key sequence into the list.
 */
static void
tty_key_list_insert_sequence(key, key_seq, aux_data)
    tty_key_t **key;
    unsigned char *key_seq;
    void *aux_data;
{
    tty_key_t *new_key;

    new_key = (tty_key_t *)xmalloc(sizeof(tty_key_t));
    new_key->key_seq = (unsigned char *)xstrdup((char *)key_seq);
    new_key->aux_data = aux_data;
    new_key->next = *key;
    *key = new_key;
}


/*
 * Parse the key list, inserting the new key sequence in the proper
 * position.
 */
void
tty_key_list_insert(key_seq, aux_data)
    unsigned char *key_seq;
    void *aux_data;
{
    static tty_key_t **key = NULL;

    if (*key_seq == 0)
	return;               /* bad key sequence !  */

    /* Try to optimize by checking if the current entry should be added
       after the current position.  Avoid re-parsing the entire list if
       so.  */
    if (key == NULL || strcmp((char *)key_seq, (char *)(*key)->key_seq) <= 0)
	key = &key_list_head;

    for (; *key; key = &(*key)->next)
	if (strcmp((char *)key_seq, (char *)(*key)->key_seq) <= 0)
	{
	    tty_key_list_insert_sequence(key, key_seq, aux_data);
	    return;
	}

    tty_key_list_insert_sequence(key, key_seq, aux_data);
}


/*
 * Force the next key search to start from the begining of the list.
 */
void
tty_key_search_restart()
{
    current_key = key_list_head;
}


/*
 * Incremental searches a key in the list. Returns a pointer to the first
 * sequence that matches, -1 if there is no match and NULL if there can
 * still be a match.
 */
tty_key_t *
tty_key_search(key_seq)
    char *key_seq;
{
    int cmp;

    if (current_key == NULL)
	return NULL;

    for (; current_key; current_key = current_key->next)
    {
	cmp = strcmp(key_seq, (char *)current_key->key_seq);

	if (cmp == 0)
	    return current_key;

	if (cmp  < 0)
	    break;
    }

    if (current_key == NULL ||
	strncmp(key_seq, (char *)current_key->key_seq, strlen(key_seq)) != 0)
	return (tty_key_t *)-1;
    else
	return NULL;
}


#if 0
/*
 * Delete a key from the list. Don't remove this function, God only knows
 * when I'm gonna need it...
 */
void
tty_key_list_delete()
{
    tty_key_t *key, *next_key;

    for (key = key_list_head; key; key = next_key)
    {
	next_key = key->next;
	xfree(key->key_seq);
	xfree(key);
    }
}
#endif


/*
 * Print the key sequence on the last line of the screen.
 */
void
tty_key_print(key_seq)
    char *key_seq;
{
    tty_status_t tty_status;
    wchar_t *typed = L"Keys typed so far: ";
    wchar_t *incomplete = L" ";
    wchar_t *spaces;
    wchar_t *wkey;

    tty_save(&tty_status);
    tty_goto(tty_lines - 1, 0);
    tty_background(WHITE);
    tty_foreground(BLACK);

    spaces = xmalloc( (tty_columns+1) * sizeof(wchar_t));
    wmemset(spaces, L' ', tty_columns);
    spaces[tty_columns] = '\0';
    tty_puts(spaces, tty_columns);
    xfree(spaces);
    tty_goto(tty_lines - 1, 0);

    tty_key_machine2human(key_seq);

    tty_puts(typed, wcslen(typed));
    wkey=mbsduptowcs((char *)keystr);
    tty_puts(wkey, wcslen(wkey));
    xfree(wkey);
    tty_puts(incomplete, wcslen(incomplete));

    tty_update();
    tty_restore(&tty_status);
}


/*
 * Get the first key available, returning also the repeat count, that
 * is, the number of times the key has been pressed.  These feature is
 * mainly used by the calling routines to perform optimizations.  For
 * example, if you press the down arrow several times, the caller can
 * display only the final position, saving a lot of time. If you have
 * ever worked with git on highly loaded systems, I'm sure you know what
 * I mean.
 *
 * If tty_get_key() returns NULL, the key sequence is invalid.
 */
tty_key_t *
tty_get_key(repeat_count)
    int *repeat_count;
{
    int i, c;
    tty_key_t *key = NULL;

    while ((c = tty_getc()) == -1)
	;

    if (repeat_count)
	*repeat_count = 1;

    /* Handle ^SPC.  */
    if (c == 0)
	c = 0xff;

    if (tty_kbdmode == TTY_RESTRICTED_INPUT)
    {
	if (c == '\n' || c == '\r')
	    c = key_ENTER;

	if (isprint(c) || c == key_INTERRUPT)
	{
	    default_key.key_seq[0] = c;
	    default_key.key_seq[1] = '\0';
	    return &default_key;
	}
    }

    partial = 0;
    key_on_display = 0;

    tty_key_search_restart();

    for (i = 0; i < MAX_KEY_LENGTH; i++)
    {
	/* Kludge to handle brain-damaged key sequences.  If a 0
	   (^SPC) is found, change it into 0xff.  I don't know how
	   smart this is, but right know I don't feel like doing it
	   otherwise.  */
	if (c == 0)
	    c = 0xff;

	tty_key_seq[i] = c;
	tty_key_seq[i + 1] = 0;

	key = tty_key_search((char *)tty_key_seq);

	if (key == (tty_key_t *)-1)
	{
	    alarm(1);
	    partial = 0;
	    return NULL;
	}

	if (key)
	{
	    if (partial)
	    {
		/* tty_key_print((char *)tty_key_seq); */
		/* tty_update(); */
		/* Small delay for displaying the selected sequence.  */
		/* OLD-FIX-ME: 1 second is way too much!  */
		/* sleep(1);  */
	    }
	    break;
	}

	if (keyno == 0)
	{
	    /* Schedule an alarm in 1 second, to display the key
	       sequence if still incomplete by that time.  */
	    if (key_on_display)
		tty_key_print((char *)tty_key_seq);
	    else
		alarm(1);
	    partial = 1;
	}

	while ((c = tty_getc()) == -1)
	    ;
    }

    if (i == MAX_KEY_LENGTH)
    {
	alarm(1);
	partial = 0;
	return NULL;
    }

    if (repeat_count)
	while (keyno > i &&
	       (memcmp(tty_key_seq, &keybuf[keyindex], i + 1) == 0))
	{
	    keyindex += i + 1;
	    keyno -= i + 1;
	    (*repeat_count)++;
	}

    alarm(1);
    partial = 0;
    return key;
}


void
tty_key_print_async()
{
    if (partial)
    {
	tty_key_print((char *)tty_key_seq);
	key_on_display = 1;
    }
}


char *
tty_get_previous_key_seq()
{
    return (char *)tty_key_seq;
}


#define columns_ok(x) (((x) > 0) && ((x) <= MAX_TTY_COLUMNS))
#define lines_ok(x)   (((x) > 0) && ((x) <= MAX_TTY_LINES))

void
tty_resize()
{
    endwin();
    refresh();
    tty_set_mode(tty_mode);
    tty_columns=COLS;
    tty_lines=LINES;
}


/*
 * Restore the screen.
 */

void
tty_put_screen(buf)
    char *buf;
{
    tty_defaults();
    tty_clear();
}

/*
 * Get the color index from the colors[] index table.
 */
int
tty_get_color_index(colorname)
    char *colorname;
{
    int i;

    for (i = 0; i < 10; i++)
	if (strcmp(colors[i], colorname) == 0)
	    return (i < 8) ? i : (i - 8);

    return -1;
}

/*
 * Get the capability of a given termcap symbol.  Return NULL if there
 * is no capability for it.
 */
char *
tty_get_symbol_key_seq(symbol)
    char *symbol;
{
    int i;

    for (i = TTY_FIRST_SYMBOL_KEY; i < TTY_CAPABILITIES_USED; i++)
	if (strcmp(tty_capability[i].symbol, symbol) == 0)
	    return tty_capability[i].string;

    return NULL;
}


/*
 * Get the entire set of required termcap/terminfo capabilities. It performs
 * consistency checkings trying to recover as well as possible.
 */
static void
tty_get_capabilities()
{
    char *capability_buf;
    int err, i, term_errors = 0;
    char *termtype = getenv("TERM");

    if (termtype == NULL)
    {
	fprintf(stderr, "%s: can't find the TERM environment variable, ",
		g_program);
	goto switch_to_vt100;
    }

    if (strlen(termtype) > 63)
    {
	fprintf(stderr, "%s: the TERM environment variable is too long, ",
		g_program);
      switch_to_vt100:
	fprintf(stderr, "trying vt100 ...\n");
	termtype = vt100;
    }

  retry:
#ifdef HAVE_LINUX
    err = tgetent(NULL, termtype);
#else
    err = tgetent(term_buf, termtype);
#endif

    if (err == -1)
    {
	fprintf(stderr, "%s: can't find the %s database.\n",
		g_program, term_database);
	fprintf(stderr, "%s: check your %s environment variable ...\n",
		g_program, term_env);
	exit(1);
    }

    if (err == 0)
    {
	fprintf(stderr,
		"%s: can't find the terminal type %s in the %s database.\n",
		g_program, termtype, term_database);

	if (strcmp(termtype, "iris-ansi") == 0)
	{
	    fprintf(stderr, "%s: trying ansi...\n", g_program);
	    termtype = "ansi";
	    goto retry;
	}

	if (tty_is_xterm(termtype))
	{
	    fprintf(stderr, "%s: trying xterm...\n", g_program);
	    termtype = "xterm";
	    goto retry;
	}

	if (strcmp(termtype, "vt220") == 0 ||
	    strcmp(termtype, "vt320") == 0)
	{
	    fprintf(stderr, "%s: trying vt100...\n", g_program);
	    termtype = "ansi";
	    goto retry;
	}

	exit(1);
    }

    tty_type = xstrdup(termtype);

    capability_buf = xmalloc(2048);

    for (i = TTY_FIRST_SYMBOL_KEY; i < TTY_CAPABILITIES_USED; i++)
	tty_capability[i].string = tgetstr(tty_capability[i].name,
					   &capability_buf);

    for (i = 0; i < TTY_CAPABILITIES_USED; i++)
	if (tty_capability[i].string == NULL)
	{
	    if (tty_capability[i].required)
	    {
		term_errors++;
		fprintf(stderr,
			"%s: can't find the '%s' terminal capability.\n",
			g_program, tty_capability[i].name);
	    }
	    else
	    {
		/* If this capability does not exist but its presence
		   is not mandatory then make it be the null string.  */
		tty_capability[i].string = "";
	    }
	}

    if (term_errors)
    {
	fprintf(stderr, "%s: %d errors. Your terminal is too dumb :-< .\n",
		g_program, term_errors);
	exit(1);
    }
}


/*
 * Initialize the tty driver.
 */
void
tty_init(kbd_mode)
    int kbd_mode;
{
    /* Fail if stdin or stdout are not real terminals.  */
    if (!isatty(TTY_INPUT) || !isatty(TTY_OUTPUT))
    {
	fprintf(stderr, "%s: only stderr can be redirected.\n", g_program);
	exit(1);
    }

    if ((tty_device_str = ttyname(1)) == NULL)
    {
	fprintf(stderr, "%s: can't get terminal name.\n", g_program);
	exit(1);
    }
    tty_device=mbsduptowcs(tty_device_str);


    /* Store the terminal settings in old_term. it will be used to restore
       them later.  */
#ifdef HAVE_POSIX_TTY
    tcgetattr(TTY_OUTPUT, &old_term);
#else
#ifdef HAVE_SYSTEMV_TTY
    ioctl(TTY_OUTPUT, TCGETA, &old_term);
#else
    ioctl(TTY_OUTPUT, TIOCGETP, &old_arg);
    ioctl(TTY_OUTPUT, TIOCGETC, &old_targ);
    ioctl(TTY_OUTPUT, TIOCGLTC, &old_ltarg);
#endif /* HAVE_SYSTEMV_TTY */
#endif /* HAVE_POSIX_TTY */

    default_key.key_seq  = tty_key_seq = (unsigned char *)xmalloc(64);
    default_key.aux_data = NULL;
    default_key.next = NULL;
    tty_kbdmode = kbd_mode;

    tty_device_length = wcslen(tty_device);
    tty_get_capabilities();

    /* init curses */
    initscr();
    keypad(stdscr, FALSE);
    nonl();
    echo();
    tty_next_free_color_pair=1;
}


/*
 * Update the title of xterm-like X terminal emulators.
 */
void
tty_update_title(string)
    wchar_t *string;
{
    if (tty_is_xterm(tty_type))
    {
	size_t len = wcslen(string);
	size_t buflen = 128 + len + 1;
	wchar_t *temp = (wchar_t *)xmalloc(buflen * sizeof(wchar_t));
	wchar_t *printable_string = xwcsdup(string);

	toprintable(printable_string, len);
	swprintf(temp, buflen, L"%c]2;%s - %ls%c", 0x1b, PRODUCT, printable_string, 0x07);
	/* I don't know what can be considered a resonable limit here,
	   I just arbitrarily chosed to truncate the length of the
	   title to twice the number of columns.  Longer strings seem
	   to make fvwm2 issue error messages.  */
	if (128 + (int)len > 2 * tty_columns)
	{
	    temp[2 * tty_columns    ] = L'\07';
	    temp[2 * tty_columns + 1] = L'\0';
	}

	wxwrite(TTY_OUTPUT, temp, wcslen(temp));
	xfree(printable_string);
	xfree(temp);
	fflush(stdout);
    }
}

static int
tty_is_xterm(term)
    char *term;
{
    if (strncmp(term, "xterm", 5)     == 0 ||
	strncmp(term, "rxvt", 4)      == 0 ||
	strncmp(term, "iris-ansi", 9) == 0 ||
	strcmp(term, "aixterm")       == 0 ||
	strcmp(term, "Eterm")         == 0 ||
	strcmp(term, "dtterm")        == 0)
    {
	return 1;
    }
    return 0;
}

void
tty_init_colors(cmdline, configfile)
    int cmdline, configfile;
{
    if(!has_colors())
	AnsiColors=OFF;
    else if (cmdline == -1)
	AnsiColors = configfile;
    else
	AnsiColors = cmdline;
    if(AnsiColors)
	start_color();
}

void
tty_wait_for_keypress()
{
    char dummy;
    alarm(0);
    fprintf(stdout, "Press almost any key to continue\n");
    fflush(stdout);
    tty_noncanonic();
    tty_read(&dummy,1);
}

/* tty mode (^O) in gitfm can't use curses so
   needs to handle terminfo stuff directly */

void
ttymode_colors(brightness, fg, bg)
    int brightness, fg, bg;
{
    int cp;
    int attr=WA_NORMAL;
    if(brightness)
	attr |= WA_BOLD;
    if(AnsiColors == ON)
    {
	cp=tty_get_color_pair(fg,bg);
	vid_puts(attr, cp, NULL, putchar);
    }
    else
    {
	if(((fg != WHITE) || (bg != BLACK)))
	    attr |= WA_REVERSE;
	vidputs(attr, putchar);
    }
}

void
ttymode_defaults()
{
    if(AnsiColors == ON)
	vid_puts(WA_NORMAL, COLOR_PAIR(0), NULL, putchar);
    else
	vidputs(WA_NORMAL, putchar);
}

void
ttymode_puts(str, len)
    wchar_t *str;
    int len;
{
    wchar_t *msg;
    msg=xmalloc((len+1) * sizeof(wchar_t));
    wmemcpy(msg, str, len);
    msg[len]='\0';
    printf("%ls", msg);
    ttymode_defaults();
}

void
ttymode_goto(x,y)
{
    mvcur(-1, -1, y, x);
}

void
ttymode_clrscr()
{
    int i;
    wchar_t *buf;
    buf=xmalloc((tty_columns+1) * sizeof(wchar_t));
    wmemset(buf, L' ', tty_columns);
    buf[tty_columns]='\0';
    mvcur(-1,-1,0,0);
    ttymode_defaults();
    for(i=0; i < tty_lines; i++)
    {
	printf("%ls", buf);
    }
    fflush(stdout);
}
