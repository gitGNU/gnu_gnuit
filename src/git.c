/* git.c -- The main git file.  */

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
#include <stdlib.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPE_H */
#include <stddef.h>

#include "file.h"
#include <ctype.h>
#include <signal.h>
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif /* HAVE_PWD_H */
#ifdef HAVE_GRP_H
#include <grp.h>
#endif /* HAVE_GRP_H */
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif /* HAVE_LIMITS_H */
#include <unistd.h>
#include <locale.h>
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif /* HAVE_ASSERT_H */
#include <wchar.h>
#include <wctype.h>

#include "stdc.h"
#include "common.h"
#include "git.h"
#include "xstring.h"
#include "xmalloc.h"
#include "getopt.h"
#include "xio.h"
#include "tty.h"
#include "window.h"
#include "inputline.h"
#include "status.h"
#include "title.h"
#include "panel.h"
#include "configure.h"
#include "signals.h"
#include "system.h"
#include "history.h"
#include "tilde.h"
#include "misc.h"

char *copyright =
"GNUIT is free software; you can redistribute it and/or modify it under the\n\
terms of the GNU General Public License as published by the Free Software\n\
Foundation; either version 3, or (at your option) any later version.\n\
Copyright (C) 1993-2001, 2007-2008 Free Software Foundation, Inc.\n\
Written by Tudor Hulubei and Andrei Pitis, Bucharest, Romania\n\n";


#define MAX_STATIC_SIZE 50

extern int AnsiColors;
int TypeSensitivity = ON;

/* These are the only possible values for `current_mode'. Used while
   resuming from suspended mode in order to correctly refresh the
   display. */
#define GIT_SCREEN_MODE         0
#define GIT_TERMINAL_MODE       1

char *g_home;
char *g_program;
/* for gnulib lib/error.c */
char *program_name;
char *version = VERSION;
int two_panel_mode = 1;
int current_mode = GIT_SCREEN_MODE;
int panel_no;
int wait_msg;

int UseLastScreenChar;

char color_section[]  = "[GITFM-Color]";
char monochrome_section[] = "[GITFM-Monochrome]";

wchar_t lock_bad[]   = L"Bad password, try again...";

wchar_t *exit_msg;

char *screen;
wchar_t PS1[4] = L" $ ";
panel_t *left_panel, *right_panel, *src_panel, *dst_panel, *tmp_panel;

static wchar_t *NormalModeHelp      = L"";
static wchar_t *CommandLineModeHelp = L"";
static int ConfirmOnExit;

/* Directory history stuff.  */
char **dir_history;
int dir_history_count;
int dir_history_point;

window_t *title_window, *status_window, *il_window;

#define BUILTIN_OPERATIONS                       89

#define BUILTIN_copy				 -1
#define BUILTIN_move				 -2
#define BUILTIN_make_directory			 -3
#define BUILTIN_delete				 -4
#define BUILTIN_exit				 -5
#define BUILTIN_previous_history_element	 -6
#define BUILTIN_tty_mode			 -7
#define BUILTIN_refresh				 -8
#define BUILTIN_switch_panels			 -9
#define BUILTIN_next_history_element		-10
#define BUILTIN_panel_enable_next_mode		-11
#define BUILTIN_panel_enable_owner_group	-12
#define BUILTIN_panel_enable_date_time		-13
#define BUILTIN_panel_enable_size		-14
#define BUILTIN_panel_enable_abbrevsize		-15
#define BUILTIN_panel_enable_mode		-16
#define BUILTIN_panel_enable_full_name		-17
#define BUILTIN_panel_sort_next_method		-18
#define BUILTIN_panel_sort_by_name		-19
#define BUILTIN_panel_sort_by_extension		-20
#define BUILTIN_panel_sort_by_size		-21
#define BUILTIN_panel_sort_by_date		-22
#define BUILTIN_panel_sort_by_mode		-23
#define BUILTIN_panel_sort_by_owner_id		-24
#define BUILTIN_panel_sort_by_group_id		-25
#define BUILTIN_panel_sort_by_owner_name	-26
#define BUILTIN_panel_sort_by_group_name	-27
#define BUILTIN_select_entry			-28
#define BUILTIN_entry_to_input_line		-29
#define BUILTIN_beginning_of_panel		-30
#define BUILTIN_end_of_panel			-31
#define BUILTIN_scroll_down			-32
#define BUILTIN_scroll_up			-33
#define BUILTIN_previous_line			-34
#define BUILTIN_next_line			-35
#define BUILTIN_other_panel			-36
#define BUILTIN_change_directory		-37
#define BUILTIN_select_files_matching_pattern	-38
#define BUILTIN_unselect_files_matching_pattern	-39
#define BUILTIN_adapt_current_directory		-40
#define BUILTIN_adapt_other_directory		-41
#define BUILTIN_other_path_to_input_line	-42
#define BUILTIN_selected_entries_to_input_line	-43
#define BUILTIN_backward_char			-44
#define BUILTIN_forward_char			-45
#define BUILTIN_backward_word			-46
#define BUILTIN_forward_word			-47
#define BUILTIN_beginning_of_line		-48
#define BUILTIN_end_of_line			-49
#define BUILTIN_delete_char			-50
#define BUILTIN_backward_delete_char		-51
#define BUILTIN_kill_word			-52
#define BUILTIN_backward_kill_word		-53
#define BUILTIN_kill_line			-54
#define BUILTIN_kill_to_beginning_of_line	-55
#define BUILTIN_kill_to_end_of_line		-56
#define BUILTIN_just_one_space			-57
#define BUILTIN_delete_horizontal_space		-58
#define BUILTIN_downcase_word			-59
#define BUILTIN_upcase_word			-60
#define BUILTIN_capitalize_word			-61
#define BUILTIN_action				-62
#define BUILTIN_set_mark			-63
#define BUILTIN_kill_region			-64
#define BUILTIN_kill_ring_save			-65
#define BUILTIN_yank				-66
#define BUILTIN_exchange_point_and_mark		-67
#define BUILTIN_set_scroll_step			-68
#define BUILTIN_isearch_backward		-69
#define BUILTIN_isearch_forward			-70
#define BUILTIN_previous_directory		-71
#define BUILTIN_next_directory			-72
#define BUILTIN_reset_directory_history		-73
#define BUILTIN_enlarge_panel			-74
#define BUILTIN_enlarge_other_panel		-75
#define BUILTIN_two_panels			-76
#define BUILTIN_lock				-77
#define BUILTIN_quick_compare_panels		-78
#define BUILTIN_thorough_compare_panels		-79
#define BUILTIN_name_downcase			-80
#define BUILTIN_name_upcase			-81
#define BUILTIN_up_one_dir			-82
#define BUILTIN_compare				-83
#define BUILTIN_bin_packing			-84
#define BUILTIN_horizontal_scroll_left		-85
#define BUILTIN_horizontal_scroll_right		-86
#define BUILTIN_select_extension		-87
#define BUILTIN_unselect_extension		-88
#define BUILTIN_apropos				-89

#define MAX_BUILTIN_NAME			 35


char builtin[BUILTIN_OPERATIONS][MAX_BUILTIN_NAME] =
{
    "copy",
    "move",
    "make-directory",
    "delete",
    "exit",
    "previous-history-element",
    "tty-mode",
    "refresh",
    "switch-panels",
    "next-history-element",
    "panel-enable-next-mode",
    "panel-enable-owner-group",
    "panel-enable-date-time",
    "panel-enable-size",
    "panel-enable-abbrevsize",
    "panel-enable-mode",
    "panel-enable-full-name",
    "panel-sort-next-method",
    "panel-sort-by-name",
    "panel-sort-by-extension",
    "panel-sort-by-size",
    "panel-sort-by-date",
    "panel-sort-by-mode",
    "panel-sort-by-owner-id",
    "panel-sort-by-group-id",
    "panel-sort-by-owner-name",
    "panel-sort-by-group-name",
    "select-entry",
    "entry-to-input-line",
    "beginning-of-panel",
    "end-of-panel",
    "scroll-down",
    "scroll-up",
    "previous-line",
    "next-line",
    "other-panel",
    "change-directory",
    "select-files-matching-pattern",
    "unselect-files-matching-pattern",
    "adapt-current-directory",
    "adapt-other-directory",
    "other-path-to-input-line",
    "selected-entries-to-input-line",
    "backward-char",
    "forward-char",
    "backward-word",
    "forward-word",
    "beginning-of-line",
    "end-of-line",
    "delete-char",
    "backward-delete-char",
    "kill-word",
    "backward-kill-word",
    "kill-line",
    "kill-to-beginning-of-line",
    "kill-to-end-of-line",
    "just-one-space",
    "delete-horizontal-space",
    "downcase-word",
    "upcase-word",
    "capitalize-word",
    "action",
    "set-mark",
    "kill-region",
    "kill-ring-save",
    "yank",
    "exchange-point-and-mark",
    "set-scroll-step",
    "isearch-backward",
    "isearch-forward",
    "previous-directory",
    "next-directory",
    "reset-directory-history",
    "enlarge-panel",
    "enlarge-other-panel",
    "two-panels",
    "lock",
    "quick-compare-panels",
    "thorough-compare-panels",
    "name-downcase",
    "name-upcase",
    "up-one-dir",
    "compare",
    "bin-packing",
    "horizontal-scroll-left",
    "horizontal-scroll-right",
    "select-extension",
    "unselect-extension",
    "apropos",
};


typedef struct
{
    char *name;         /* The command name.  */
    wchar_t *body;      /* The unexpanded command body.  */
    char *new_dir;      /* If exit code == 0, goto this directory.  */
    char  save_screen;  /* Save the screen contents (if possible).  */
    char  pause;        /* Wait for a key before restoring the panels.  */
    char  hide;         /* Hide the output, emulating a builtin command.  */
    char  builtin;      /* This is a builtin command.  */
    wchar_t *sequence;  /* The ascii representation of the key sequence on
			   which the command is bound; used for error
			   reporting and the apropos command.  */
    xstack_t *history;  /* The history of the strings used to expand the
			   command body.  */
} command_t;


#define MAX_KEYS        2048      /* enough ? :-)  */
#define KEYSDATA_FIELDS    8


/*
 * Return ON if there is enough space on the screen to display the panels.
 */
static int
panels_can_be_displayed()
{
    if (tty_lines >= 7)
    {
	if (two_panel_mode)
	{
	    if (tty_columns >= 6 * 2)
		return ON;
	}
	else
	    if (tty_columns >= 6)
		return ON;
    }

    return OFF;
}


/*
 * Return 1 if in GIT_TERMINAL_MODE.
 */
int
in_terminal_mode()
{
    return (current_mode == GIT_TERMINAL_MODE);
}


/*
 * Resize (if necessary) all git's components.
 */
void
resize(resize_required)
    int resize_required;
{
    int display_title = OFF;
    int display_status = OFF;
    int old_tty_lines = tty_lines;
    int old_tty_columns = tty_columns;

    tty_resize();

    /* Don't resize, unless absolutely necessary.  */
    if (!resize_required)
	if (tty_lines == old_tty_lines && tty_columns == old_tty_columns)
	    return;

    /* Watch out for special cases (tty_lines < 7) because some
     * components will no longer fit.
     *
     * The title needs one line.  The panels need four.  There is one
     * more line for the input line and one for the status.
     *
     * Cases (lines available):
     *      1 line:	input line
     *      2 lines:	input line + status
     *	  3-6 lines:	title + input line + status
     *	 >= 7 lines:	everything
     */

    if (tty_lines >= 2)
	display_status = ON;

    if (tty_lines >= 3)
	display_title = ON;

    if (panels_can_be_displayed())
    {
	if (two_panel_mode)
	{
	    int right_panel_columns = (tty_columns >> 1);
	    int left_panel_columns = right_panel_columns + (tty_columns & 1);

	    if (window_x(panel_window(src_panel)) <=
		window_x(panel_window(dst_panel)))
	    {
		panel_resize(src_panel, 0, 1,
			     tty_lines - 3, left_panel_columns);
		panel_resize(dst_panel, left_panel_columns, 1,
			     tty_lines - 3, right_panel_columns);
	    }
	    else
	    {
		panel_resize(src_panel, left_panel_columns, 1,
			     tty_lines - 3, right_panel_columns);
		panel_resize(dst_panel, 0, 1,
			     tty_lines - 3, left_panel_columns);
	    }
	}
	else
	{
	    panel_resize(src_panel, 0, 1, tty_lines - 3, tty_columns);
	    panel_resize(dst_panel, 0, 1, tty_lines - 3, tty_columns);
	}
    }
    else
    {
	panel_resize(src_panel, 0x10000, 0x10000, 2, 80);
	panel_resize(dst_panel, 0x10000, 0x10000, 2, 80);
    }

    title_resize(display_title ? tty_columns : 0, 0);
    status_resize(display_status ? tty_columns : 0, tty_lines - 1);
    il_resize(tty_columns, (tty_lines == 1) ? 0 : (tty_lines - 2));
}


/*
 * Resize and refresh all git's components.
 * `signum' is the number of the signal that forced the refresh
 * (SIGCONT or SIGWINCH) but can be 0 if unconditional refresh is
 * desired.
 */
void
screen_refresh(signum)
    int signum;
{
    tty_touch();
    resize(0);

    if (signum == SIGCONT)
    {
	/* We were suspended.  Switch back to noncanonical mode.  */
	tty_set_mode(TTY_NONCANONIC);
	tty_defaults();
    }

    if (wait_msg)
	return;

    panel_no_optimizations(src_panel);
    panel_no_optimizations(dst_panel);
    panel_center_current_entry(src_panel);
    panel_center_current_entry(dst_panel);

    if (current_mode == GIT_SCREEN_MODE)
    {
	if (!panels_can_be_displayed())
	{
	    /* We know that when this is the case, the panels will not
	       be displayed, and we don't want junk on the screen.  */
	    tty_defaults();
	    tty_clear();
	}

	title_update();
	panel_update(src_panel);

	if (two_panel_mode)
	    panel_update(dst_panel);
    }
    else
	tty_put_screen(screen);

    status_update();
    il_update();
    il_update_point();
    tty_update();

    if (signum == SIGCONT)
	tty_update_title(panel_get_wpath(src_panel));
}


static void
report_undefined_key(status_message)
    char *status_message;
{
    char *prev = tty_get_previous_key_seq();
    size_t length = strlen(prev);

    if (length && (prev[length - 1] != key_INTERRUPT))
    {
	char *str = (char *)tty_key_machine2human(prev);
	int len=128 + strlen(str);
	wchar_t *buf = xmalloc(len * sizeof(wchar_t));
	swprintf(buf, len, L"%s: not defined.", str);
	status(buf, STATUS_ERROR, STATUS_LEFT);
	xfree(buf);

	tty_beep();
	tty_update();
	sleep(1);
    }
    else
	tty_beep();

    if (status_message)
    {
	wchar_t *msg=mbsduptowcs(status_message);
	status(msg, STATUS_OK, STATUS_CENTERED);
	xfree(msg);
    }
    else
	status_default();
    il_update_point();
    tty_update();
}


/*****************************************/
/* The GIT interface to the input line.  */
/*****************************************/

#define IL_ISEARCH_BEGIN        0
#define IL_ISEARCH_BACKWARD     1
#define IL_ISEARCH_FORWARD      2
#define IL_ISEARCH_END          3


extern int il_dispatch_commands PROTO ((int, int));
extern wchar_t *il_fix_text PROTO ((wchar_t *));
extern wchar_t *il_build_help_from_string PROTO ((wchar_t *));
extern wchar_t *il_isearch PROTO ((wchar_t *, wchar_t **, int, int *));
extern wchar_t il_read_char PROTO ((wchar_t *, wchar_t *, int));
extern wchar_t *il_read_line PROTO ((wchar_t *, wchar_t **, wchar_t *, xstack_t *));


/*
 * Add a string to the history.
 */

static void
il_history_add_entry(history, text)
    xstack_t *history;
    wchar_t *text;
{
    wchar_t *history_text;

    /* Avoid duplicates.  */
    if (xstack_preview(history, &history_text, 1) &&
	wcscmp(history_text, text) == 0)
	return;

    history_text = xwcsdup(text);
    xstack_push(history, &history_text);
}


/*
 * Preview a history string.
 */
static wchar_t *
il_history_view_entry(history, offset)
    xstack_t *history;
    int offset;
{
    wchar_t *history_text;

    return xstack_preview(history, &history_text, offset) ?
	   history_text : NULL;
}


/*
 * Dispatch input line commands. key is the actual command while flags is
 * a set of IL_*s or-ed together, allowing us to customize the  behaviour
 * of the input line.  If IL_MOVE is not specified, the  IL_EDIT  flag is
 * ignored.  Returns 1 if key has been processed and 0 otherwise.
 */
int
il_dispatch_commands(key, flags)
    int key;
    int flags;
{
    if ((flags & IL_MOVE) == 0)
	return 0;

    switch (key)
    {
	case BUILTIN_backward_char:
	    il_backward_char();
	    break;

	case BUILTIN_forward_char:
	    il_forward_char();
	    break;

	case BUILTIN_backward_word:
	    il_backward_word();
	    break;

	case BUILTIN_forward_word:
	    il_forward_word();
	    break;

	case BUILTIN_beginning_of_line:
	    il_beginning_of_line();
	    break;

	case BUILTIN_end_of_line:
	    il_end_of_line();
	    break;

	case BUILTIN_delete_char:
	    if (flags & IL_EDIT)
		il_delete_char();
	    break;

	case BUILTIN_backward_delete_char:
	    if (flags & IL_EDIT)
		il_backward_delete_char();
	    break;

	case BUILTIN_kill_word:
	    if (flags & IL_EDIT)
		il_kill_word();
	    break;

	case BUILTIN_backward_kill_word:
	    if (flags & IL_EDIT)
		il_backward_kill_word();
	    break;

	case BUILTIN_kill_line:
	    if (flags & IL_EDIT)
		il_kill_line(IL_STORE);
	    break;

	case BUILTIN_kill_to_beginning_of_line:
	    if (flags & IL_EDIT)
		il_kill_to_beginning_of_line();
	    break;

	case BUILTIN_kill_to_end_of_line:
	    if (flags & IL_EDIT)
		il_kill_to_end_of_line();
	    break;

	case BUILTIN_just_one_space:
	    if (flags & IL_EDIT)
		il_just_one_space();
	    break;

	case BUILTIN_delete_horizontal_space:
	    if (flags & IL_EDIT)
		il_delete_horizontal_space();
	    break;

	case BUILTIN_downcase_word:
	    if (flags & IL_EDIT)
		il_downcase_word();
	    break;

	case BUILTIN_upcase_word:
	    if (flags & IL_EDIT)
		il_upcase_word();
	    break;

	case BUILTIN_capitalize_word:
	    if (flags & IL_EDIT)
		il_capitalize_word();
	    break;

	case BUILTIN_set_mark:
	    il_set_mark();
	    break;

	case BUILTIN_kill_region:
	    if (flags & IL_EDIT)
		il_kill_region();
	    break;

	case BUILTIN_kill_ring_save:
	    il_kill_ring_save();
	    break;

	case BUILTIN_yank:
	    if (flags & IL_EDIT)
		il_yank();
	    break;

	case BUILTIN_exchange_point_and_mark:
	    il_exchange_point_and_mark();
	    break;

	default:
	    if ((flags & IL_EDIT) && isprint(key))
		il_insert_char(key);
	    else
		return 0;
	    break;
    }

    return 1;
}


/*
 * Fix the text.  Replace non-printable characters with question marks
 * and expand tabs.  Return a malloc-ed pointer to the fixed text.
 * The caller should free the new text.
 */
wchar_t *
il_fix_text(text)
    wchar_t *text;
{
    int i, j;
    wchar_t *fixed_text;
    size_t fixed_text_length;


    if (text == NULL)
	return NULL;

    fixed_text_length = wcslen(text) + 1;
    fixed_text = xmalloc(fixed_text_length * sizeof(wchar_t));

    for (i = 0, j = 0; text[i]; i++)
	if (text[i] == L'\t')
	{
	    fixed_text_length += 8;
	    fixed_text = xrealloc(fixed_text, (fixed_text_length * sizeof(wchar_t)));
	    wmemcpy(&fixed_text[j], L"        ", 8);
	    j += 8;
	}
	else
	    if (iswprint(text[i]))
		fixed_text[j++] = text[i];
	    else
		fixed_text[j++] = L'?';

    fixed_text[j] = 0;

    return fixed_text;
}


wchar_t *
il_build_help_from_string(options)
    wchar_t *options;
{
    size_t len = 0;
    wchar_t *options_ptr = options;
    wchar_t *help = xmalloc((1 + wcslen(options) * 3 + 8) * sizeof(wchar_t));

    help[len++] = L'(';

    for (; *(options_ptr + 1); options_ptr++)
    {
	help[len++] = *options_ptr;
	help[len++] = L',';
	help[len++] = L' ';
    }

    help[len++] = *options_ptr;
    help[len++] = L')';
    help[len++] = L' ';
    help[len++] = L'\0';

    return help;
}


/*
 * Read only one char from the input line.  message is a string explaining
 * what is this all about.  options is a string containing only those
 * characters that are valid answers, NULL if any character is a valid
 * answer.  The default char is the first char in the options string.
 * Returns 0 if it was interrupted, a valid character otherwise.
 */
wchar_t
il_read_char(message, options, flags)
    wchar_t *message;
    wchar_t *options;
    int flags;
{
    wchar_t *help;
    tty_key_t *ks;
    int key, repeat_count;
    command_t *command;
    input_line_t *saved_il = NULL;

    if (flags & IL_SAVE)
	saved_il = il_save();

    il_reset_line();

    if (message)
    {
	wchar_t *text = il_fix_text(message);
	if (flags & IL_ERROR)
	{
	    il_insert_text(L"*** ");
	    il_set_error_flag(1);
	}

	il_insert_text(text);

	if (flags & IL_HOME)
	    il_beginning_of_line();

	xfree(text);

	if (options)
	{
	    help = il_build_help_from_string(options);
	    il_insert_text(help);
	    xfree(help);
	}
    }

    il_update();
    il_update_point();
    tty_update();

    if (flags & IL_BEEP)
	tty_beep();

    while (1)
    {
	while ((ks = tty_get_key(&repeat_count)) == NULL)
	{
	    tty_beep();
	    status_update();
	    il_update_point();
	}

	key = ks->key_seq[0];
	command = (command_t *)ks->aux_data;

	if (command && command->builtin)
	    key = - 1 - (command->name - builtin[0]) / MAX_BUILTIN_NAME;

	switch (key)
	{
	    case BUILTIN_refresh:
		screen_refresh(0);
		break;

	    case BUILTIN_action:
		if (options != NULL)
		    key = *options;

	    case key_INTERRUPT:
		goto done;

	    default:
		while (repeat_count--)
		    if (il_dispatch_commands(key, flags) == 0)
			goto il_error;

		il_update();
		break;

	      il_error:
		if (options == NULL)
		    goto done;

		if (options && wcschr(options, key))
			goto done;

		tty_beep();
		break;
	}

	status_update();
	il_update_point();
	tty_update();
    }

  done:
    il_set_error_flag(0);

    if ((flags & IL_SAVE) && saved_il)
    {
	il_restore(saved_il);
	il_update();
	il_update_point();
	tty_update();
    }

    return (key == key_INTERRUPT) ? 0 : key;
}


/*
 * WARNING: dest *must* be a pointer to a NULL pointer or a pointer
 * to a pointer allocated with xmalloc().  In the first case,
 * il_read_line() will return a string allocated with xmalloc().  In
 * the second case, it will reallocate the pointer as needed using
 * xrealloc().  The caller should free the memory allocated this way.
 */
wchar_t *
il_read_line(static_text, dest, default_string, history)
    wchar_t *static_text;
    wchar_t **dest;
    wchar_t *default_string;
    xstack_t *history;
{
    tty_key_t *ks;
    wchar_t *history_text;
    command_t *command;
    int key = 0, repeat_count, offset = 0;


    il_reset_line();

    if (static_text)
	il_set_static_text(static_text);

    if (default_string)
	il_insert_text(default_string);

    if (history && default_string)
    {
	il_history_add_entry(history, default_string);
	offset = 1;
    }

    il_update();
    il_update_point();
    tty_update();

    while (1)
    {
	while ((ks = tty_get_key(&repeat_count)) == NULL)
	{
	    tty_beep();
	    status_update();
	    il_update_point();
	}

	key = ks->key_seq[0];
	command = (command_t *)ks->aux_data;

	if (command && command->builtin)
	    key = - 1 - (command->name - builtin[0]) / MAX_BUILTIN_NAME;

	switch (key)
	{
	    case BUILTIN_previous_line:
	    case BUILTIN_previous_history_element:
		if (history == NULL)
		    break;

		history_text = il_history_view_entry(history, ++offset);

		if (history_text == NULL)
		{
		    offset--;
		    tty_beep();
		}
		else
		{
		    il_kill_line(IL_DONT_STORE);
		    il_insert_text(history_text);
		    il_update();
		    il_update_point();
		}
		break;

	    case BUILTIN_next_line:
	    case BUILTIN_next_history_element:
		if (history == NULL)
		    break;

		if (offset == 0)
		{
		    il_kill_line(IL_DONT_STORE);
		    il_update();
		    il_update_point();
		    break;
		}

		il_kill_line(IL_DONT_STORE);

		offset--;

		if (offset > 0)
		{
		    history_text = il_history_view_entry(history, offset);
		    il_insert_text(history_text);
		}

		il_update();
		il_update_point();
		break;

	    case BUILTIN_refresh:
		screen_refresh(0);
		break;

	    case BUILTIN_action:
		il_get_contents(dest);

	    case key_INTERRUPT:
		goto done;

	    default:
		while (repeat_count--)
		    if (il_dispatch_commands(key, IL_MOVE | IL_EDIT) == 0)
			tty_beep();

		il_update();
		break;
	}

	status_update();
	il_update_point();
	tty_update();
    }

  done:
    if (key == BUILTIN_action)
    {
	if (history)
	    il_history_add_entry(history, *dest);
	return *dest;
    }
    else
	return NULL;
}


/*
 * status = IL_ISEARCH_BEGIN    -> we are beginning to isearch; initialize
 * status = IL_ISEARCH_BACKWARD -> we are in the middle of an isearch-backward
 * status = IL_ISEARCH_FORWARD  -> we are in the middle of an isearch-forward
 * status = IL_ISEARCH_END      -> isearch complete; clean up
 *
 * *action = IL_ISEARCH_ACTION_DECREASE -> the user pressed the backspace key
 *					   so if there is no matching element
 *					   in the panel stack, we should delete
 *					   the last character in the input line
 * *action = IL_ISEARCH_ACTION_RETRY    -> the user pressed the isearch-*
 *					   character again, so we should try to
 *					   find a new match for the current
 *					   string
 * *action = IL_ISEARCH_ACTION_INCREASE -> a new character has been inserted
 *					   into the input line so we should try
 *					   to find a match for the new string
 */
wchar_t *
il_isearch(static_text, dest, status, action)
    wchar_t *static_text;
    wchar_t **dest;
    int status;
    int *action;
{
    int key;
    int keycmd;
    tty_key_t *ks;
    command_t *command;
    static input_line_t *saved_il;

    if (status == IL_ISEARCH_BEGIN)
    {
	saved_il = il_save();
	il_reset_line();

	if (static_text)
	    il_set_static_text(static_text);

	return NULL;
    }

    if (status == IL_ISEARCH_END)
    {
	il_restore(saved_il);
	il_update();
	il_update_point();
	tty_update();
	return NULL;
    }

    if (action == NULL)
	return NULL;

    *action = IL_ISEARCH_ACTION_NONE;

    il_update();
    il_update_point();
    tty_update();

  restart:
    if ((ks = tty_get_key(NULL)) == NULL)
    {
	status_update();
	il_update_point();
	tty_update();
	return NULL;
    }

    keycmd = key = ks->key_seq[0];

    command = (command_t *)ks->aux_data;

    if (command && command->builtin)
	keycmd = - 1 - (command->name - builtin[0]) / MAX_BUILTIN_NAME;

    switch (keycmd)
    {
	case key_INTERRUPT:
	case BUILTIN_action:
	    break;

	case BUILTIN_refresh:
	    screen_refresh(0);
	    goto restart;

	case BUILTIN_backward_delete_char:
	    /* If the input line is empty, just beep.  */
	    if (il_is_empty())
		tty_beep();
	    else
	    {
		*action = IL_ISEARCH_ACTION_DECREASE;
		/* Don't call il_backward_delete_char().  There might be
		   several history elements in the panel stack that match
		   the current string so we have to delay the call to
		   il_backward_delete_char() until all the matching elements
		   have been pop-ed.  */
	    }

	    break;

	default:
	    if ((keycmd == BUILTIN_isearch_backward &&
		 status == IL_ISEARCH_BACKWARD)  ||
		(keycmd == BUILTIN_isearch_forward  &&
		 status == IL_ISEARCH_FORWARD))
	    {
		if (il_is_empty())
		    *action = IL_ISEARCH_ACTION_INCREASE;
		else
		    *action = IL_ISEARCH_ACTION_RETRY;
		break;
	    }

	    if (iswprint(key))
	    {
		il_insert_char(key);
		*action = IL_ISEARCH_ACTION_INCREASE;
	    }
	    else
	    {
		/* Force a NULL return value when detecting non printable
		   characters (or functional keys).  */
		keycmd = key_INTERRUPT;
	    }

	    break;
    }

    status_update();
    il_update();
    il_update_point();
    tty_update();
    il_get_contents(dest);

    return (keycmd == BUILTIN_action || keycmd == key_INTERRUPT) ? NULL : *dest;
}


/****************************************/
/* The directory history function set.  */
/****************************************/

static void
dir_history_reset()
{
    if (dir_history)
    {
	int i;

	for (i = 0; i < dir_history_count; i++)
	    xfree(dir_history[i]);

	xfree(dir_history);
	dir_history = NULL;
    }

    dir_history_count = 0;
    dir_history_point = 0;
}


static void
dir_history_add(directory)
    char *directory;
{
    dir_history_point = dir_history_count;
    dir_history = (char **)xrealloc(dir_history,
				    ++dir_history_count * sizeof(char *));
    dir_history[dir_history_point] = xstrdup(directory);
}


static void
dir_history_next(this, link)
    panel_t *this;
    panel_t *link;
{
    if (dir_history_point < dir_history_count - 1)
	panel_action(this, act_CHDIR, link,
		     dir_history[++dir_history_point], 1);
    else
	tty_beep();
}


static void
dir_history_prev(this, link)
    panel_t *this;
    panel_t *link;
{
    if (dir_history_point)
	panel_action(this, act_CHDIR, link,
		     dir_history[--dir_history_point], 1);
    else
	tty_beep();
}


void
clean_up()
{
    tty_end(NULL);

    /* It is better not to do this here.  It can lead to an endless loop
       if xmalloc fails in write_history because xmalloc will call fatal
       and fatal will call clean_up again...  */
#if 0
    if (il)
	il_end();
#endif

    status_end();
    remove_log();
}


void
fatal(postmsg)
    char *postmsg;
{
    if (tty_get_mode() == TTY_NONCANONIC)
	clean_up();

    fprintf(stderr, "%s: fatal error: %s.\n", g_program, postmsg);
    exit(1);
}


/*
 * This function is a mess. Don't try to understand what it does
 * ... :-(  Basically, it expands a configuration line macro.  The
 * return value is 0 on error, -1 if some condition failed (the
 * command contains a %d but the current entry is not a directory), 1
 * if everything is ok, 2 if the command was correctly expanded and it
 * contains a '%i' and 3 if it contains a '%I'.  Easy, isn't it?
 */
static int
command_expand(command, dest, p, l)
    command_t *command;
    wchar_t **dest;
    panel_t *p, *l;
{
    wchar_t c;
    uid_t uid;
    gid_t gid;
    int retval;
    panel_t *t;
    size_t len, qlen;
    struct group *grp;
    struct passwd *pwd;
    static int busy = 0;
    wchar_t *answer = NULL;
    wchar_t *question = NULL;
    int i_flag = 0, entry;
    size_t oldtmplen, tmplen;
    char *sptr;
    wchar_t *ptr, *tmp = NULL, *d, *flag;
    wchar_t *src = command->body, *save_body;

    len = wcslen(src) + 1;
    d = *dest = xmalloc(len * sizeof(wchar_t));

    while (*src)
    {
	if (*src != '%')
	    *d++ = *src++;
	else
	{
	    t = iswlower(*++src) ? p : l;

	    switch (*src)
	    {
		case L'?':
		    if (busy)
		    {
			busy = 0;
			goto bad_command;
		    }

		    if (*++src != L'{')
			goto bad_command;

		    if ((ptr = wcschr(++src, L'}')) == NULL)
			goto bad_command;

		    *ptr = 0;
		    c = il_read_char(src, L"yn", IL_MOVE);
		    *ptr = L'}';

		    if (c != L'y')
			goto strings_dont_match;

		    src = ptr;

		    break;

		case 's':
		    if (busy)
		    {
			busy = 0;
			goto bad_command;
		    }

		    if (*++src != L'{')
			goto bad_command;

		    if ((ptr = wcschr(++src, L',')) == NULL)
			goto bad_command;

		    *ptr = 0;
		    busy = 1;

		    save_body = command->body;
		    command->body = src;
		    retval = command_expand(command, &answer, p, l);
		    command->body = save_body;

		    busy = 0;

		    if (retval < 1)
		    {
			*ptr = L',';
			if (retval == 0)
			    goto bad_command;
			else
			    goto strings_dont_match;
		    }

		    qlen=16 + strlen(command->name) + wcslen(answer) + 1;
		    question = xmalloc(qlen * sizeof(wchar_t));
		    swprintf(question, qlen, L"%s: %ls", command->name, answer);
		    xfree(answer);
		    answer =  NULL;
		    *ptr++ = L',';

		    if ((src = wcschr(ptr, L'}')) == NULL)
			goto bad_command;

		    *src = 0;

		    if (wcslen(question) > MAX_STATIC_SIZE)
			question[MAX_STATIC_SIZE] = 0;

		    busy = 1;

		    save_body = command->body;
		    command->body = ptr;
		    retval = command_expand(command, &answer, p, l);
		    command->body = save_body;

		    busy = 0;

		    if (retval < 1)
		    {
			*src = L'}';
			xfree(question);
			question = NULL;
			if (retval == 0)
			    goto bad_command;
			goto strings_dont_match;
		    }

		    flag = il_read_line(question, &tmp, answer,
					command->history);

		    xfree(question);
		    xfree(answer);
		    question = answer = NULL;

		    if (flag == NULL)
		    {
			*src = L'}';
			goto strings_dont_match;
		    }

		    *src = L'}';
		    break;

		case 'f':
		case 'F':
		    if (panel_get_current_file_type(t) != FILE_ENTRY)
			goto strings_dont_match;

		  get_file_name:
		    sptr = panel_get_current_file_name(t);
		    tmplen=1 + strlen(sptr) + 1 + 1;
		    tmp = xmalloc(tmplen * sizeof(wchar_t));
		    swprintf(tmp, tmplen, L"\"%s\"", sptr);
		    break;

		case 'd':
		case 'D':
		    if (panel_get_current_file_type(t) != DIR_ENTRY)
			goto strings_dont_match;
		    goto get_file_name;

		case 'l':
		case 'L':
		    if (panel_get_current_file_type(t) != SYMLINK_ENTRY)
			goto strings_dont_match;
		    goto get_file_name;

		case 't':
		case 'T':
		    if (panel_get_current_file_type(t) != FIFO_ENTRY)
			goto strings_dont_match;
		    goto get_file_name;

		case 'z':
		case 'Z':
		    if (panel_get_current_file_type(t) != SOCKET_ENTRY)
			goto strings_dont_match;
		    goto get_file_name;

		case 'a':
		case 'A':
		    goto get_file_name;

		case 'm':
		case 'M':
		    tmplen=16;
		    tmp = xmalloc(tmplen * sizeof(wchar_t));
		    swprintf(tmp, tmplen, L"%o",
			    (int)panel_get_current_file_mode(t) & 07777);
		    break;

		case 'o':
		case 'O':
		    uid = panel_get_current_file_uid(t);
		    pwd = getpwuid(uid);

		    if (pwd)
			tmp = mbsduptowcs(pwd->pw_name);
		    else
		    {
			tmplen=16;
			tmp = xmalloc(tmplen * sizeof(wchar_t));
			swprintf(tmp, tmplen, L"%o", (int)uid);
		    }

		    break;

		case 'g':
		case 'G':
		    gid = panel_get_current_file_gid(t);
		    grp = getgrgid(gid);

		    if (grp)
			tmp = mbsduptowcs(grp->gr_name);
		    else
		    {
			tmplen=16;
			tmp = xmalloc(tmplen * sizeof(wchar_t));
			swprintf(tmp, tmplen, L"%o", (int)gid);
		    }

		    break;

		case 'p':
		case 'P':
		    tmplen=1 + strlen(t->path) + 1 + 1;
		    tmp = xmalloc(tmplen * sizeof(wchar_t));
		    swprintf(tmp, tmplen, L"\"%s\"", t->path);
		    break;

		case 'b':
		case 'B':
		    sptr = strrchr(t->path, '/');
		    sptr = (*++sptr) ? sptr : "/root";
		    tmplen = 1 + strlen(sptr) + 1 + 1;
		    tmp = xmalloc(tmplen * sizeof(wchar_t));
		    swprintf(tmp, tmplen, L"\"%s\"", sptr);
		    break;

		case 'i':
		case 'I':
		    i_flag = (*src == L'i') ? 1 : 2;

		    if (busy && t->selected_entries)
		    {
			tmplen = 21;
			tmp = xmalloc(tmplen * sizeof(wchar_t));
			wcscpy(tmp, L"selected entries");
			break;
		    }

		    tmp = NULL;
		    tmplen = 0;

		    panel_init_iterator(t);

		    while ((entry = panel_get_next(t)) != -1)
		    {
			oldtmplen = tmplen;
			tmplen += 1 + wcslen(t->dir_entry[entry].wname) + 1 + 1;
			tmp = xrealloc(tmp, ((tmplen + 1) * sizeof(wchar_t)));
			tmp[oldtmplen] = L'"';
			wcscpy(tmp + oldtmplen + 1, t->dir_entry[entry].wname);
			tmp[tmplen - 2] = L'"';
			tmp[tmplen - 1] = L' ';
			tmp[tmplen    ] = 0;
		    }

		    /* This can happen when there is no selected entry
		       and the current file is "..".  */
		    if (tmplen == 0)
			goto strings_dont_match;

		    break;

		default:
		    goto bad_command;
	    }

	    src++;
	    *d = 0;

	    if (tmp)
	    {
		len += wcslen(tmp);
		*dest = xrealloc(*dest, (len * sizeof(wchar_t)));
		wcscat(*dest, tmp);
		d = *dest + wcslen(*dest);
		xfree(tmp);
		tmp = NULL;
	    }
	}
    }

    *d = 0;
    return 1 + i_flag;

  bad_command:
    xfree(*dest);
    *dest = NULL;
    return 0;

  strings_dont_match:
    if (tmp)
	xfree(tmp);

    *dest = NULL;
    return -1;
}


static void
add_to_environment(variable, alternate_variable, value)
    char *variable;
    char *alternate_variable;
    char *value;
{
    char *alternate_value;

    if (getenv(variable) == NULL)
    {
	if (alternate_variable && (alternate_value=getenv(alternate_variable)))
	    xsetenv(variable, alternate_value);
	else
	    xsetenv(variable, value);
    }
}


/*
 * Read keys from the current section ([GITFM-Keys] is supposed to be in
 * use when read_keys() is called).  Return the number of keys read.
 */
static int
read_keys(keys, errors)
    int keys;
    int *errors;
{
    int i, j;
    command_t *command;
    int need_conversion;
    char key_seq[80];
    char *contents[KEYSDATA_FIELDS - 2];

    *errors = 0;

    for (i = keys; i < MAX_KEYS; i++)
    {
	configuration_getvarinfo(key_seq, contents,
				 KEYSDATA_FIELDS - 2, NO_SEEK);

	if (*key_seq == '\0')
	    break;

	if (*key_seq != '^')
	{
	    char *key_seq_ptr = tty_get_symbol_key_seq(key_seq);

	    if (key_seq_ptr)
	    {
		/* Ignore empty/invalid key sequences.  */
		if (*key_seq_ptr == '\0')
		    continue;

		/* We got the key sequence in the correct form, as
		   returned by tgetstr, so there is no need for
		   further conversion.  */
		strcpy(key_seq, key_seq_ptr);
		need_conversion = 0;
	    }
	    else
	    {
		/* This is not a TERMCAP symbol, it is a key sequence
		   that we will have to be converted with
		   tty_key_human2machine() into a machine usable form
		   before being used.  */
		need_conversion = 1;
	    }
	}
	else
	    need_conversion = 1;

	command = (command_t *)xcalloc(1, sizeof(command_t));

	if (contents[0])
	    command->name = xstrdup(contents[0]);
	else
	{
	    xfree(command);
	    continue;
	}

	command->history = xstack_init(sizeof(char *));

	if (contents[2])
	    command->new_dir = xstrdup(contents[2]);

	if (contents[1])
	    command->body = mbsduptowcs(contents[1]);
	else
	    goto insert;

	if (contents[3])
	    command->save_screen = ((tolower((int)contents[3][0])=='y')?1:0);
	else
	    command->save_screen = 1;

	if (contents[4])
	    command->pause = ((tolower((int)contents[4][0])=='y')?1:0);

	if (contents[5])
	    command->hide = ((tolower((int)contents[5][0])=='y')?1:0);

      insert:
	/* This speeds up the process.  It is not a limitation because,
	   by convention, all the build-in command names contain only
	   lowercase letters.  Avoid searching through the list of
	   built-in command names.  */
	if (islower((int)command->name[0]))
	    for (j = 0; j < BUILTIN_OPERATIONS; j++)
	    {
		if (strcmp(command->name, builtin[j]) == 0)
		{
		    xfree(command->name);
		    command->name = builtin[j];
		    command->builtin = 1;
		    break;
		}
	    }

	command->sequence = mbsduptowcs(key_seq);

	if (command->builtin || command->body || command->new_dir)
	{
	    if (need_conversion)
	    {
		if (tty_key_human2machine((unsigned char *)key_seq))
		    tty_key_list_insert((unsigned char *)key_seq,
					(void *)command);
		else
		{
		    fprintf(stderr, "%s: warning: invalid key sequence '%s'\n",
			    g_program, key_seq);
		    (*errors)++;
		}
	    }
	    else
		tty_key_list_insert((unsigned char *)key_seq, (void *)command);
	}
    }

    return i;
}


/*
 * Last moment settings before suspending git.
 */
void
hide()
{
    tty_set_mode(TTY_CANONIC);
    tty_defaults();
    tty_put_screen(screen);
}


/*
 * Set the git prompt.
 */
static void
set_prompt()
{
    wchar_t temp[MAX_STATIC_SIZE + 1];
    wchar_t *prompt=wcscat(truncate_string(panel_get_wpath(src_panel), temp,
					   MAX_STATIC_SIZE-wcslen(PS1)+1), PS1);
    il_set_static_text(prompt);
}


static void
reread()
{
    /* Note that the order is important.  `src_panel' should be
       updated last, because we want its current directory to be git's
       current directory.  */
    panel_action(dst_panel, act_REGET, src_panel, (void *)-1, 1);
    panel_action(src_panel, act_REGET, dst_panel, (void *)-1, 1);
}


static void
usage()
{
    printf("usage: %s [-hvcblp] [path1] [path2]\n", g_program);
    printf(" -h         print this help message\n");
    printf(" -v         print the version number\n");
    printf(" -c         use ANSI colors\n");
    printf(" -b         don't use ANSI colors\n");
    printf(" -l         don't use the last screen character\n");
    printf(" -p         output final path at exit\n");
}


int
main(argc, argv)
    int argc;
    char *argv[];
{
    tty_key_t *ks;
    char *final_path;
    command_t *command;
    size_t len = 0;
    char *temporary_directory;
    int previous_isearch_failed;
    int resuming_previous_isearch;
    int output_final_path = OFF;
    input_line_t *saved_il = NULL;
    char *panel_path, *current_path;
    wchar_t *lock_password, *unlock_password;
    wchar_t *aproposstr;
    int child_exit_code, repeat_count, keys;
    int action_status, i, retval, to_case, cmp_mode;
    int c, ansi_colors = -1, use_last_screen_character = ON;
    int entry, key, app_end = 0, first_time = 1, errors = 0;
    char *left_panel_path, *right_panel_path;
    char *scmdln=NULL, *soutput_string;
    wchar_t *cmdln = NULL, *output_string=L"", *wptr, *wsrcptr, *input = NULL;
    wchar_t *search_string = NULL;
    int exit_msg_len, wptrlen;
    int left_panel_columns, right_panel_columns;

    /* Make sure we don't get signals before we are ready to handle
       them.  */
    signals_init();

#ifdef HAVE_SETLOCALE
    setlocale(LC_ALL,"");
#endif

    program_name = g_program = argv[0];
    g_home = getenv("HOME");
    if (g_home == NULL)
	g_home = ".";

    compute_directories();
    update_path();

    get_login_name();

    exit_msg_len=strlen(PRODUCT) + 16;
    exit_msg = xmalloc(exit_msg_len * sizeof(wchar_t));
    swprintf(exit_msg, exit_msg_len, L"Exit %s? ", PRODUCT);

    /* Parse the command line.  */
    while ((c = getopt(argc, argv, "hvcblp")) != -1)
	switch (c)
	{
	    case 'h':
		/* Help request.  */
		usage();
		return 0;

	    case 'v':
		/* Version number request.  */
		printf("%s %s\n", PRODUCT, VERSION);
		return 0;

	    case 'c':
		/* Force git to use ANSI color sequences.  */
		ansi_colors = ON;
		break;

	    case 'b':
		/* Prevent git from using ANSI color sequences.  */
		ansi_colors = OFF;
		break;

	    case 'l':
		/* Prevent git from using the last character on the
		   screen.  */
		use_last_screen_character = OFF;
		break;

	    case 'p':
		/* Output the final path at exit.  Useful to be able
		   to `cd' to that directory provided that a wrapper
		   script is used.  */
		output_final_path = ON;
		break;

	    case '?':
		return 1;

	    default:
		fprintf(stderr, "%s: unknown error\n", g_program);
		return 1;
	}

    left_panel_path = right_panel_path = ".";

    if (optind < argc)
    {
	left_panel_path = xstrdup(argv[optind++]);

	if (optind < argc)
	    right_panel_path = xstrdup(argv[optind++]);
	else
	    right_panel_path = left_panel_path;
    }

    if (optind < argc)
    {
	fprintf(stderr, "%s: warning: invalid extra options ignored\n",
		g_program);
	wait_msg++;
    }

    printf("\n");

#ifdef HAVE_GCC
    printf("%s %s (%s), %s %s\n",
	   PRODUCT, VERSION, HOST, __TIME__, __DATE__);
#else
    printf("%s %s (%s)\n", PRODUCT, VERSION, HOST);
#endif /* !HAVE_GCC */

    printf(copyright);

#ifdef DEBIAN
    add_to_environment("GIT_EDITOR",    "EDITOR",     "sensible-editor");
    add_to_environment("GNUIT_EDITOR",  "GIT_EDITOR", "sensible-editor");
    add_to_environment("GIT_PAGER",     "PAGER",      "sensible-pager");
    add_to_environment("GNUIT_PAGER",   "GIT_PAGER",  "sensible-pager");
    add_to_environment("GIT_BROWSER",   (char *)NULL, "sensible-browser");
    add_to_environment("GNUIT_BROWSER", "GIT_BROWSER",  "sensible-browser");
#else /* !DEBIAN */
    add_to_environment("GIT_EDITOR",    "EDITOR",     "vi");
    add_to_environment("GNUIT_EDITOR",  "GIT_EDITOR", "vi");
    add_to_environment("GIT_PAGER",     "PAGER",      "more");
    add_to_environment("GNUIT_PAGER",   "GIT_PAGER",  "more");
    add_to_environment("GIT_BROWSER",   (char *)NULL, "lynx");
    add_to_environment("GNUIT_BROWSER", "GIT_BROWSER",  "lynx");
#endif /* !DEBIAN */
    add_to_environment("GIT_SHELL",     "SHELL",      "/bin/sh");
    add_to_environment("GNUIT_SHELL",   "GIT_SHELL",  "/bin/sh");
    add_to_environment("GIT_RMAIL",     (char *)NULL, "mail");
    add_to_environment("GNUIT_RMAIL",   "GIT_RMAIL",    "mail");
    add_to_environment("GIT_VMSTAT",    (char *)NULL, "free");
    add_to_environment("GNUIT_VMSTAT",  "GIT_VMSTAT",   "free");

    tty_init(TTY_RESTRICTED_INPUT);

    common_configuration_init();

    use_section("[GITFM-FTI]");
    get_file_type_info();

    /* Even though we just set up curses, drop out of curses
       mode so error messages from config show up ok*/
    tty_end_cursorapp();

    use_section("[GITFM-Keys]");
    keys = read_keys(0, &errors);
    wait_msg += errors;
    configuration_end();

    wait_msg += (specific_configuration_init() == 0);

    temporary_directory = getenv("TMPDIR");

    if (temporary_directory == NULL)
	temporary_directory = "/tmp";

    stdout_log_template = xmalloc(32 + strlen(temporary_directory) + 1);
    stderr_log_template = xmalloc(32 + strlen(temporary_directory) + 1);
    stdout_log_name     = xmalloc(32 + strlen(temporary_directory) + 1);
    stderr_log_name     = xmalloc(32 + strlen(temporary_directory) + 1);
    sprintf(stdout_log_template, "%s/gnuit.1.XXXXXX", temporary_directory);
    sprintf(stderr_log_template, "%s/gnuit.2.XXXXXX", temporary_directory);
    *stdout_log_name    = 0;
    *stderr_log_name    = 0;
    use_section("[Setup]");

    tty_init_colors(ansi_colors, get_flag_var("AnsiColors", OFF));

    if (use_last_screen_character)
	UseLastScreenChar = get_flag_var("UseLastScreenChar",  OFF);
    else
	UseLastScreenChar = OFF;

    tty_set_last_char_flag(UseLastScreenChar);

    use_section("[GITFM-Setup]");

    if (AnsiColors == ON)
	TypeSensitivity = get_flag_var("TypeSensitivity", ON);
    else
	TypeSensitivity = OFF;

    ConfirmOnExit       = get_flag_var("ConfirmOnExit", OFF);
    NormalModeHelp      = mbsduptowcs(get_string_var("NormalModeHelp", ""));
    CommandLineModeHelp = mbsduptowcs(get_string_var("CommandLineModeHelp", ""));

    use_section(AnsiColors ? color_section : monochrome_section);

    get_colorset_var(TitleColors, TitleFields, TITLE_FIELDS);

    use_section("[GITFM-FTI]");
    get_file_type_info();

    use_section("[GITFM-Keys]");
    keys = read_keys(keys, &errors);
    wait_msg += errors;

    if (keys == MAX_KEYS)
    {
	fprintf(stderr, "%s: too many key sequences; only %d are allowed.\n",
		g_program, MAX_KEYS);
	wait_msg++;
    }

#ifndef HAVE_LONG_FILE_NAMES
    fprintf(stderr,
	    "%s: warning: your system doesn't support long file names.\n",
	    g_program);
    wait_msg++;
#endif /* !HAVE_LONG_FILE_NAMES */

    if (getuid() == 0)
	PS1[1] = L'#';

    current_path = xgetcwd();

    if (current_path == NULL)
	fatal("`getcwd' failed: permission denied");

    if (wait_msg)
    {
	tty_wait_for_keypress();
	wait_msg = 0;
    }

    tty_start_cursorapp();
    title_init();
    il_init();
    status_init(NormalModeHelp);

    if (left_panel_path[0] == '/')
	panel_path = xstrdup(left_panel_path);
    else
    {
	panel_path = xmalloc(strlen(current_path) + 1 +
			     strlen(left_panel_path) + 1);
	sprintf(panel_path, "%s/%s", current_path, left_panel_path);
    }

    right_panel_columns = (COLS >> 1);
    left_panel_columns = right_panel_columns + (COLS & 1);

    left_panel = panel_init(panel_path, (LINES-4), left_panel_columns, 1, 0);
    xfree(panel_path);

    if (right_panel_path[0] == '/')
	panel_path = xstrdup(right_panel_path);
    else
    {
	panel_path = xmalloc(strlen(current_path) + 1 +
			     strlen(right_panel_path) + 1);
	sprintf(panel_path, "%s/%s", current_path, right_panel_path);
    }

    right_panel = panel_init(panel_path, (LINES-4), right_panel_columns,
			     1, left_panel_columns);

    xfree(panel_path);
    xfree(current_path);

    configuration_end();

    src_panel = left_panel;
    dst_panel = right_panel;

    resize(0);

    dir_history       = NULL;
    dir_history_count = 0;
    dir_history_point = 0;

    signal_handlers(ON);

    tty_update_title(panel_get_wpath(src_panel));

  restart:
    if (wait_msg)
    {
	tty_wait_for_keypress();
	wait_msg = 0;
    }

    tty_set_mode(TTY_NONCANONIC);
    tty_defaults();
    tty_update();

    tty_update_title(panel_get_wpath(src_panel));
    alarm(60 - get_local_time()->tm_sec);

    src_panel = panel_no ? right_panel : left_panel;
    dst_panel = panel_no ? left_panel  : right_panel;

    if (tty_lines < 7)
    {
	/* We know that when this is the case, the panels will not
	   be displayed, and we don't want junk on the screen.  */
	tty_defaults();
	tty_clear();
    }

    title_update();
    status_default();
    il_restore(saved_il);
    tty_update();

    /* Save the input line contents.  */
    saved_il = il_save();

    reread();
    screen_refresh(0);

    /* Restore the input line contents.  */
    il_restore(saved_il);

    panel_set_focus(src_panel, ON);

    if (first_time)
    {
	dir_history_add(panel_get_path(src_panel));
	first_time = 0;
    }

    set_prompt();
    saved_il = il_save();

    while(!app_end)
    {
	il_restore(saved_il);
	saved_il = il_save();
	il_update();
	il_update_point();
	tty_update();
	il_get_contents(&cmdln);

	user_heart_attack = 0;
	while ((ks = tty_get_key(&repeat_count)) == NULL)
	    report_undefined_key(NULL);
	status_update();

	key = ks->key_seq[0];
	command = (command_t *)ks->aux_data;

	if (command)
	{
	    if (command->builtin)
		key = - 1 - (command->name-builtin[0]) / MAX_BUILTIN_NAME;
	    else
	    {
		if (command->name)
		{
		    panel_no_optimizations(src_panel);
		    panel_no_optimizations(dst_panel);

		    if (command->body)
		    {
			wchar_t *cmd = NULL;
			char *scmd;

			retval = command_expand(command, &cmd,
						src_panel, dst_panel);

			if (retval)
			{
			    if (retval > 0)
			    {
				size_t msglen = 32 + strlen(command->name) +
				    wcslen(cmd) + 1;
				wchar_t *msg = xmalloc(msglen * sizeof(wchar_t));

				swprintf(msg,msglen, L"%s: %ls", command->name, cmd);
				status(msg, STATUS_WARNING, STATUS_LEFT);
				tty_update();
				xfree(msg);

				if (command->hide)
				{
				    int msglen=64+strlen(command->name)+1;
				    wchar_t *msg = xmalloc(msglen*sizeof(wchar_t));
				    swprintf(msg,msglen,
					    L"Wait, running %s command %s...",
					    "user-defined", command->name);
				    il_message(msg);
				    tty_update();
				    xfree(msg);
				}

				if (!is_a_bg_command(cmd))
				    tty_update_title(cmd);
				scmd=wcsduptombs(cmd);
				child_exit_code = start(scmd, command->hide);
				xfree(scmd);
				xfree(cmd);

				if (command->hide)
				{
				    if(WIFSIGNALED(child_exit_code))
				    {
					il_read_char(L"Command interrupted by signal",
						     NULL,
						     IL_BEEP|IL_ERROR);
				    }
				    else if(WIFEXITED(child_exit_code) &&
					    (WEXITSTATUS(child_exit_code) != 0))
				    {
					tty_beep();
					display_errors(command->name);
				    }
				}
				else
				{
				    tty_touch();

				    if (command->pause)
					wait_msg = 1;
				}

				if (WIFEXITED(child_exit_code) &&
				    (WEXITSTATUS(child_exit_code) == 0) &&
				    command->new_dir)
				{
				    char *expanded_dir =
					tilde_expand(command->new_dir);

				    panel_action(src_panel, act_CHDIR,
						 dst_panel, expanded_dir, 1);

				    dir_history_add(panel_get_path(src_panel));
				    xfree(expanded_dir);
				}

				if (WIFEXITED(child_exit_code) &&
				    (WEXITSTATUS(child_exit_code) == 0))
				{
				    if (retval == 2)
					panel_unselect_all(src_panel);
				    else
					if (retval == 3)
					    panel_unselect_all(dst_panel);
				}

				goto restart;
			    }
			    else
				continue;
			}
			else
			{
			    wchar_t *key=mbsduptowcs((char *)ks->key_seq);
			    int len=80+wcslen(key)+1;
			    wchar_t *msg=xmalloc(len * sizeof(wchar_t));
			    /* FIXME: command->sequence? shouldnt it be ks->key_seq? */
			    swprintf(msg, len,
				    L"%s: invalid command on key sequence %ls!",
				    command->name, command->sequence);
			    il_read_char(msg, NULL,
					 IL_FREEZED|IL_BEEP|IL_SAVE|IL_ERROR);
			    xfree(msg);
			    xfree(key);
			    continue;
			}
		    }
		    else
		    {
			if (command->new_dir)
			{
			    char *expanded_dir=tilde_expand(command->new_dir);

			    panel_action(src_panel, act_CHDIR, dst_panel,
					 expanded_dir, 1);

			    dir_history_add(panel_get_path(src_panel));
			    xfree(expanded_dir);
			}

			goto restart;
		    }
		}
	    }
	}

	switch (key)
	{
	    case key_INTERRUPT:
		il_free(saved_il);
		il_kill_line(IL_DONT_STORE);
		saved_il = il_save();
		break;

	    case BUILTIN_other_panel:
		if (!two_panel_mode)
		    goto one_panel_mode;

		if ((repeat_count & 1) == 0)
		    break;

		panel_set_focus(src_panel, OFF);

		tmp_panel = src_panel;
		src_panel = dst_panel;
		dst_panel = tmp_panel;

		panel_no = !panel_no;
		panel_set_focus(src_panel, ON);

		il_free(saved_il);
		set_prompt();
		saved_il = il_save();
		tty_update_title(panel_get_wpath(src_panel));
		break;

	    case BUILTIN_previous_line:
		panel_action(src_panel,act_UP,dst_panel,NULL,repeat_count);
		break;

	    case BUILTIN_next_line:
		panel_action(src_panel,act_DOWN,dst_panel,NULL,repeat_count);
		break;

	    case BUILTIN_action:
		action_status = 0;
		il_free(saved_il);
		il_get_contents(&cmdln);
		scmdln=wcsduptombs(cmdln);
		/* Remove the trailing spaces.  */
		for (i = strlen(scmdln) - 1; i >= 0; i--)
		    if (scmdln[i] == ' ')
			scmdln[i] = '\0';
		    else
			break;

		switch (scmdln[0])
		{
		    case '+':
			if (scmdln[1] == '\0')
			    panel_action(src_panel, act_SELECT_ALL,
					 dst_panel, NULL, 1);
			else
			    panel_action(src_panel, act_PATTERN_SELECT,
					 dst_panel, scmdln + 1, 1);

			il_kill_line(IL_DONT_STORE);
			break;

		    case '-':
			if (scmdln[1] == '\0')
			    panel_action(src_panel, act_UNSELECT_ALL,
					 dst_panel, NULL, 1);
			else
			    panel_action(src_panel, act_PATTERN_UNSELECT,
					 dst_panel, scmdln + 1, 1);

			il_kill_line(IL_DONT_STORE);
			break;

		    case '\0':
			action_status = panel_action(src_panel, act_ENTER,
						     dst_panel, screen, 1);
			tty_update_title(panel_get_wpath(src_panel));
			il_kill_line(IL_DONT_STORE);
			set_prompt();
			break;

		    case '*':
			if (scmdln[1] == '\0')
			{
			    panel_action(src_panel, act_TOGGLE,
					 dst_panel, NULL, 1);
			    il_kill_line(IL_DONT_STORE);
			    break;
			}
			/* Fall through... */

		    default:
			if (history_expand(scmdln, &soutput_string) >= 0)
			{
			    int bg_cmd;

			    output_string=mbsduptowcs(soutput_string);
			    if (is_an_empty_command(output_string))
			    {
				saved_il = il_save();
				il_read_char(L"Void command.", NULL,
					     IL_FREEZED | IL_BEEP |
					     IL_SAVE    | IL_ERROR);
				xfree(output_string);
				break;
			    }

			    bg_cmd = is_a_bg_command(output_string);
			    if (!bg_cmd)
				tty_update_title(output_string);
			    il_kill_line(IL_DONT_STORE);
			    il_insert_text(output_string);
			    start(soutput_string, bg_cmd);
			    xfree(soutput_string);
			    xfree(output_string);
			    il_history(IL_RECORD);
			    il_kill_line(IL_DONT_STORE);
			    /* HACK: Do not call tty_update(); here!  */

			    if (!bg_cmd)
			    {
				panel_no_optimizations(src_panel);
				panel_no_optimizations(dst_panel);
				tty_touch();
				action_status = 1;
				wait_msg = 1;
			    }
			}
			else
			    il_read_char(output_string, NULL,
					 IL_FREEZED | IL_BEEP |
					 IL_SAVE    | IL_ERROR);
			break;
		}

		saved_il = il_save();
		xfree(scmdln);

		if (action_status)
		    goto restart;

		break;

	    case BUILTIN_select_entry:
		for (i = 0; i < repeat_count; i++)
		    panel_action(src_panel, act_SELECT, dst_panel, NULL, 1);
		break;

	    case BUILTIN_scroll_down:
		for (i = 0; i < repeat_count; i++)
		    panel_action(src_panel, act_PGUP, dst_panel, NULL, 1);
		break;

	    case BUILTIN_scroll_up:
		for (i = 0; i < repeat_count; i++)
		    panel_action(src_panel, act_PGDOWN, dst_panel, NULL, 1);
		break;

	    case BUILTIN_beginning_of_panel:
		panel_action(src_panel, act_HOME, dst_panel, NULL, 1);
		break;

	    case BUILTIN_end_of_panel:
		panel_action(src_panel, act_END, dst_panel, NULL, 1);
		break;

	    case BUILTIN_refresh:
		reread();
		tty_update_title(panel_get_wpath(src_panel));
		screen_refresh(0);
		break;

	    case BUILTIN_tty_mode:
		if ((repeat_count & 1) == 0)
		    break;

		alarm(0);
		tty_put_screen(screen);
		tty_end_cursorapp();
		status(CommandLineModeHelp, STATUS_OK, STATUS_CENTERED);
		tty_update();

		while (1)
		{
		    il_restore(saved_il);
		    saved_il = il_save();
		    il_update();
		    il_update_point();
		    tty_update();
		    il_get_contents(&cmdln);

		    current_mode = GIT_TERMINAL_MODE;

		    while ((ks = tty_get_key(&repeat_count)) == NULL)
			report_undefined_key(CommandLineModeHelp);

		    key = ks->key_seq[0];
		    command = (command_t *)ks->aux_data;

		    if (command && command->builtin)
			key = - 1 - (command->name - builtin[0]) /
				     MAX_BUILTIN_NAME;

		    if (key == BUILTIN_tty_mode && (repeat_count & 1))
		    {
			il_free(saved_il);
			saved_il = il_save();
			break;
		    }

		    switch (key)
		    {
			case key_INTERRUPT:
			    il_free(saved_il);
			    il_kill_line(IL_DONT_STORE);
			    saved_il = il_save();
			    break;

			case BUILTIN_action:
			    if (cmdln[0])
			    {
				il_free(saved_il);
				scmdln=wcsduptombs(cmdln);
				if (history_expand(scmdln, &soutput_string) < 0)
				{
				    output_string=mbsduptowcs(soutput_string);
				    il_read_char(output_string, NULL,
						 IL_FREEZED | IL_BEEP |
						 IL_SAVE    | IL_ERROR);
				    saved_il = il_save();
				    xfree(scmdln);
				    xfree(soutput_string);
				    xfree(output_string);
				    break;
				}
				xfree(scmdln);
				output_string=mbsduptowcs(soutput_string);
				tty_put_screen(screen);
				il_kill_line(IL_DONT_STORE);
				il_insert_text(output_string);
				tty_update_title(output_string);
				start(soutput_string, 0);
				xfree(soutput_string);
				xfree(output_string);
				il_history(IL_RECORD);
				status(CommandLineModeHelp,
				       STATUS_OK, STATUS_CENTERED);
				il_kill_line(IL_DONT_STORE);
				saved_il = il_save();
				tty_update_title(panel_get_wpath(src_panel));
				tty_update();
			    }

			    break;

			case BUILTIN_previous_history_element:
			case BUILTIN_previous_line:
			    il_free(saved_il);

			    for (i = 0; i < repeat_count; i++)
			    {
				il_history(IL_PREVIOUS);
				tty_update();
			    }

			    saved_il = il_save();
			    break;

			case BUILTIN_next_history_element:
			case BUILTIN_next_line:
			    il_free(saved_il);

			    for (i = 0; i < repeat_count; i++)
			    {
				il_history(IL_NEXT);
				tty_update();
			    }

			    saved_il = il_save();
			    break;

			case BUILTIN_refresh:
			    screen_refresh(0);
			    tty_put_screen(screen);
			    status(CommandLineModeHelp,
				   STATUS_OK, STATUS_CENTERED);
			    tty_update();
			    break;

			case BUILTIN_exit:
			    if (ConfirmOnExit == OFF ||
				il_read_char(exit_msg,L"yn",IL_FREEZED) == 'y')
			    {
				app_end = 1;
				goto end_tty_mode;
			    }

			    status(CommandLineModeHelp,
				   STATUS_OK, STATUS_CENTERED);
			    tty_update();
			    break;

			default:
			    if (key)
			    {
				il_free(saved_il);

				while (repeat_count--)
				    il_dispatch_commands(key, IL_MOVE|IL_EDIT);

				saved_il = il_save();
			    }

			    break;
		    }

		    status_update();
		}

	      end_tty_mode:
		tty_start_cursorapp();
		panel_no_optimizations(src_panel);
		panel_no_optimizations(dst_panel);
		tty_touch();
		status_default();
		tty_update();
		alarm(60 - get_local_time()->tm_sec);

		current_mode = GIT_SCREEN_MODE;

		if (app_end)
		    continue;

		goto restart;

	    case BUILTIN_copy:
		panel_action(src_panel, act_COPY, dst_panel, NULL, 1);
		break;

	    case BUILTIN_move:
		panel_action(src_panel, act_MOVE, dst_panel, NULL, 1);
		break;

	    case BUILTIN_make_directory:
		panel_action(src_panel, act_MKDIR, dst_panel, NULL, 1);
		break;

	    case BUILTIN_delete:
		panel_action(src_panel, act_DELETE, dst_panel, NULL, 1);
		break;

	    case BUILTIN_panel_enable_next_mode:

	    case BUILTIN_panel_enable_owner_group:
	    case BUILTIN_panel_enable_date_time:
	    case BUILTIN_panel_enable_abbrevsize:
	    case BUILTIN_panel_enable_size:
	    case BUILTIN_panel_enable_mode:
	    case BUILTIN_panel_enable_full_name:
		panel_action(src_panel,
			     act_ENABLE_NEXT_MODE -
			     (key - BUILTIN_panel_enable_next_mode),
			     NULL, NULL, 1);
		break;

	    case BUILTIN_panel_sort_next_method:

	    case BUILTIN_panel_sort_by_name:
	    case BUILTIN_panel_sort_by_extension:
	    case BUILTIN_panel_sort_by_size:
	    case BUILTIN_panel_sort_by_date:
	    case BUILTIN_panel_sort_by_mode:
	    case BUILTIN_panel_sort_by_owner_id:
	    case BUILTIN_panel_sort_by_group_id:
	    case BUILTIN_panel_sort_by_owner_name:
	    case BUILTIN_panel_sort_by_group_name:
		panel_action(src_panel,
			     act_SORT_NEXT_METHOD -
			     (key - BUILTIN_panel_sort_next_method),
			     NULL, NULL, 1);
		break;

	    case BUILTIN_exit:
		if (ConfirmOnExit == OFF ||
		    il_read_char(exit_msg, L"yn", IL_FREEZED) == 'y')
		    app_end = 1;

		break;

	    case BUILTIN_entry_to_input_line:
		wsrcptr = xwcsdup(panel_get_current_file_wname(src_panel));
		wptrlen=1 + 1 + wcslen(wsrcptr) + 1 + 1 + 1;
		wptr = xmalloc(wptrlen * sizeof(wchar_t));

	      copy_to_cmdln:
		len = wcslen(cmdln);
		il_free(saved_il);
		/* FIXME: I removed the quotes since git is not able to
		   handle non-printable characters in the command line
		   anyway.  We just convert them to question marks.  */
		/* FIXME: put them back? see 4.3.16 */

		/* If the character before the position where we want
		   to insert the file name is ' ' or '/', we don't
		   insert a space before the file name, to avoid
		   redundant spaces and allow for easy path
		   concatenation.  */
		if ((len != 0) &&
		    ((cmdln[il_point() - 1] == L'/') ||
		     (cmdln[il_point() - 1] == L' ')))
		{
		    if (needs_quotes(wsrcptr, wcslen(wsrcptr)))
			swprintf(wptr, wptrlen, L"\"%ls\" ", wsrcptr);
		    else
			swprintf(wptr, wptrlen, L"%ls ", wsrcptr);
		}
		else
		{
		    if (needs_quotes(wsrcptr, wcslen(wsrcptr)))
			swprintf(wptr, wptrlen, L" \"%ls\" ", wsrcptr);
		    else
			swprintf(wptr, wptrlen, L" %ls ", wsrcptr);
		}

		wptrlen = wcslen(wptr);
		toprintable(wptr, wptrlen);
		il_insert_text(wptr);
		xfree(wptr);
		xfree(wsrcptr);
		saved_il = il_save();
		break;

	    case BUILTIN_other_path_to_input_line:
		wsrcptr = mbsduptowcs(dst_panel->path);
		wptrlen = 1 + 1 + wcslen(wsrcptr) + 1 + 1 + 1;
		wptr = xmalloc(wptrlen * sizeof(wchar_t));

		goto copy_to_cmdln;

	    case BUILTIN_selected_entries_to_input_line:
		len = wcslen(cmdln);
		il_free(saved_il);

		panel_init_iterator(src_panel);

		while ((entry = panel_get_next(src_panel)) != -1)
		{
		    int wptrstrlen;
		    wsrcptr = src_panel->dir_entry[entry].wname;
		    wptrlen=1 + 1 + wcslen(wsrcptr) + 1 + 1 + 1;
		    wptr = xmalloc(wptrlen * sizeof(wchar_t));

		    if (needs_quotes(wsrcptr, wcslen(wsrcptr)))
			swprintf(wptr, wptrlen, L" \"%ls\"", wsrcptr);
		    else
			swprintf(wptr, wptrlen, L" %ls", wsrcptr);

		    wptrstrlen = wcslen(wptr);
		    len += wptrstrlen;
		    toprintable(wptr, wptrstrlen);
		    il_insert_text(wptr);
		    xfree(wptr);
		}

		il_insert_text(L" ");
		saved_il = il_save();
		break;

	    case BUILTIN_previous_history_element:
		il_free(saved_il);

		for (i = 0; i < repeat_count; i++)
		{
		    il_history(IL_PREVIOUS);
		    tty_update();
		}

		saved_il = il_save();
		break;

	    case BUILTIN_next_history_element:
		il_free(saved_il);

		for (i = 0; i < repeat_count; i++)
		{
		    il_history(IL_NEXT);
		    tty_update();
		}

		saved_il = il_save();
		break;

	    case BUILTIN_switch_panels:
		if ((repeat_count & 1) == 0)
		    break;

		if (!two_panel_mode)
		    break;

		panel_no_optimizations(src_panel);
		panel_no_optimizations(dst_panel);
		panel_action(src_panel, act_SWITCH, dst_panel, NULL, 1);
		panel_update(src_panel);
		panel_update(dst_panel);
		break;

	    case BUILTIN_change_directory:
		if (il_read_line(L"Directory: ", &input, NULL,
				 command->history))
		{
		    char *expanded_input;
		    char *sinput;

		    if (input[0] == 0)
			break;
		    sinput=wcsduptombs(input);
		    panel_action(src_panel, act_CHDIR, dst_panel,
				 expanded_input = tilde_expand(sinput), 1);

		    dir_history_add(panel_get_path(src_panel));

		    xfree(expanded_input);
		    xfree(sinput);
		    xfree(input);
		    input = NULL;

		    il_restore(saved_il);
		    set_prompt();
		    saved_il = il_save();
		    tty_update_title(panel_get_wpath(src_panel));
		}

		break;

	    case BUILTIN_select_files_matching_pattern:
		if (il_read_line(
		    L"Select files matching one of the patterns: ",
		    &input, NULL, command->history))
		{
		    if (input[0] == 0)
			break;

		    panel_action(src_panel, act_PATTERN_SELECT,
				 dst_panel, input, 1);

		    xfree(input);
		    input = NULL;
		}
		break;

	    case BUILTIN_unselect_files_matching_pattern:
		if (il_read_line(
		    L"Unselect files matching one of the patterns: ",
		    &input, NULL, command->history))
		{
		    if (input[0] == 0)
			break;

		    panel_action(src_panel, act_PATTERN_UNSELECT,
				 dst_panel, input, 1);

		    xfree(input);
		    input = NULL;
		}
		break;

	    case BUILTIN_adapt_current_directory:
		panel_action(src_panel, act_CHDIR, dst_panel,
			     dst_panel->path, 1);

		dir_history_add(panel_get_path(src_panel));

		il_free(saved_il);
		set_prompt();
		saved_il = il_save();
		tty_update_title(panel_get_wpath(src_panel));
		break;

	    case BUILTIN_adapt_other_directory:
		panel_action(dst_panel, act_CHDIR, src_panel,
			     src_panel->path, 1);

		dir_history_add(panel_get_path(dst_panel));
		break;

	    case BUILTIN_set_scroll_step:
		if (il_read_line(L"Scroll step: ", &input, NULL,
				 command->history))
		{
		    if (input[0] == 0)
			break;

		    panel_action(src_panel, act_SET_SCROLL_STEP,
				 dst_panel, input, 1);

		    xfree(input);
		    input = NULL;
		}

		break;

	    case BUILTIN_isearch_backward:
		previous_isearch_failed = 0;
		resuming_previous_isearch = 0;
		il_isearch(L"I-search backward: ", NULL,
			   IL_ISEARCH_BEGIN, (int *)NULL);
		panel_action(src_panel, act_ISEARCH_BEGIN,
			     dst_panel, NULL, 1);

		for(;;)
		{
		    isearch_aux_t iai;

		    if (il_isearch(NULL, &input, IL_ISEARCH_BACKWARD,
				   &iai.action) == NULL)
			break;

		    /* If the wcslen(input) == 0, we typed ^R twice,
		       so we must search for the string we searched
		       previously.  */
		    if (wcslen(input) == 0 &&
			search_string && wcslen(search_string))
		    {
			xfree(input);
			input = xwcsdup(search_string);
			il_insert_text(input);
			resuming_previous_isearch = 1;
			previous_isearch_failed = 0;
		    }

		    /* Wrap around.  */
		    if (iai.action == IL_ISEARCH_ACTION_RETRY &&
			previous_isearch_failed)
		    {
			panel_set_wrapped_isearch_flag(src_panel, 1);
			previous_isearch_failed = 0;
		    }

		    iai.string = input;
		    panel_action(src_panel, act_ISEARCH_BACKWARD,
				 dst_panel, &iai, 1);

		    if (iai.action == IL_ISEARCH_ACTION_FAILED)
		    {
			previous_isearch_failed = 1;
			tty_beep();
		    }
		    else
		    {
			int update = 0;

			if (resuming_previous_isearch)
			    if (iai.length < wcslen(search_string))
			    {
				il_kill_line(0);
				resuming_previous_isearch = 0;
				update = 1;
			    }

			if (iai.length < wcslen(input))
			{
			    il_backward_delete_char();
			    update = 1;
			}

			if (update)
			{
			    il_update();
			    il_update_point();
			    tty_update();
			}
		    }
		}

		if (search_string)
		    xfree(search_string);
		if (input == NULL)
		    break;
		search_string = xwcsdup(input);
		panel_action(src_panel, act_ISEARCH_END, dst_panel, NULL, 1);
		il_isearch(NULL, NULL,
			   IL_ISEARCH_END, (int *)NULL);
		break;

	    case BUILTIN_isearch_forward:
		previous_isearch_failed = 0;
		resuming_previous_isearch = 0;
		il_isearch(L"I-search: ", NULL,
			   IL_ISEARCH_BEGIN, (int *)NULL);
		panel_action(src_panel, act_ISEARCH_BEGIN, dst_panel, NULL, 1);

		for(;;)
		{
		    isearch_aux_t iai;

		    if (il_isearch(NULL, &input, IL_ISEARCH_FORWARD,
				   &iai.action) == NULL)
			break;

		    /* If the strlen(input) == 0, we typed ^S twice,
		       so we must search for the string we searched
		       previously.  */
		    if (wcslen(input) == 0 &&
			search_string && wcslen(search_string))
		    {
			xfree(input);
			input = xwcsdup(search_string);
			il_insert_text(input);
			resuming_previous_isearch = 1;
			previous_isearch_failed = 0;
		    }

		    /* Wrap around.  */
		    if (iai.action == IL_ISEARCH_ACTION_RETRY &&
			previous_isearch_failed)
		    {
			tty_beep();
			panel_set_wrapped_isearch_flag(src_panel, 1);
			previous_isearch_failed = 0;
		    }

		    iai.string = input;
		    panel_action(src_panel, act_ISEARCH_FORWARD,
				 dst_panel, &iai, 1);

		    if (iai.action == IL_ISEARCH_ACTION_FAILED)
		    {
			previous_isearch_failed = 1;
			tty_beep();
		    }
		    else
		    {
			int update = 0;

			if (resuming_previous_isearch)
			    if (iai.length < wcslen(search_string))
			    {
				il_kill_line(0);
				resuming_previous_isearch = 0;
				update = 1;
			    }

			if (iai.length < wcslen(input))
			{
			    il_backward_delete_char();
			    update = 1;
			}

			if (update)
			{
			    il_update();
			    il_update_point();
			    tty_update();
			}
		    }
		}

		if (search_string)
		    xfree(search_string);
		if (input == NULL)
		    break;
		search_string = xwcsdup(input);
		panel_action(src_panel, act_ISEARCH_END, dst_panel, NULL, 1);
		il_isearch(NULL, NULL, IL_ISEARCH_END, (int *)NULL);
		break;

	    case BUILTIN_reset_directory_history:
		dir_history_reset();
		dir_history_add(panel_get_path(src_panel));
		break;

	    case BUILTIN_previous_directory:
		dir_history_prev(src_panel, dst_panel);
		il_restore(saved_il);
		set_prompt();
		saved_il = il_save();
		tty_update_title(panel_get_wpath(src_panel));
		break;

	    case BUILTIN_next_directory:
		dir_history_next(src_panel, dst_panel);
		il_restore(saved_il);
		set_prompt();
		saved_il = il_save();
		tty_update_title(panel_get_wpath(src_panel));
		break;

	    case BUILTIN_enlarge_other_panel:
	      one_panel_mode:
		panel_set_focus(src_panel, OFF);

		tmp_panel = src_panel;
		src_panel = dst_panel;
		dst_panel = tmp_panel;

		panel_no = !panel_no;
		panel_set_focus(src_panel, ON);
		panel_activate(src_panel);

		il_free(saved_il);
		set_prompt();
		saved_il = il_save();
		tty_update_title(panel_get_wpath(src_panel));

	    case BUILTIN_enlarge_panel:
		panel_no_optimizations(src_panel);
		panel_no_optimizations(dst_panel);

		tty_touch();
		panel_deactivate(dst_panel);
		two_panel_mode = 0;
		resize(1);

		panel_action(src_panel, act_ENABLE_ALL, NULL, NULL, 1);
		panel_action(dst_panel, act_ENABLE_ALL, NULL, NULL, 1);
		panel_update(src_panel);
		break;

	    case BUILTIN_two_panels:
		panel_no_optimizations(src_panel);
		panel_no_optimizations(dst_panel);

		tty_touch();
		panel_activate(dst_panel);
		two_panel_mode = 1;
		resize(1);

		if (tty_columns < 6 * 2)
		    screen_refresh(1);

		panel_action(src_panel, act_ENABLE_SIZE, NULL, NULL, 1);
		panel_action(dst_panel, act_ENABLE_SIZE, NULL, NULL, 1);
		panel_update(src_panel);
		panel_update(dst_panel);
		break;

	    case BUILTIN_lock:
		/* Turn echo off.  */
		il_echo(0);

		lock_password = NULL;
		il_read_line(L"Enter a password: ", &lock_password,
			     NULL, (xstack_t *)NULL);

		if (lock_password == NULL || *lock_password == '\0')
		{
		    il_echo(1);
		    break;
		}

		for (unlock_password = NULL;;)
		{
		    il_read_line(L"Enter password to unlock: ",
				 &unlock_password, NULL,
				 (xstack_t *)NULL);
		    tty_update();

		    if (unlock_password &&
			wcscmp(lock_password, unlock_password) == 0)
			break;

		    il_message(lock_bad);
		    tty_beep();
		    tty_update();
		    sleep(2);
		}

		tty_update();
		xfree(lock_password);
		xfree(unlock_password);

		/* Turn echo back on.  */
		il_echo(1);
		break;

	    case BUILTIN_quick_compare_panels:
		cmp_mode = CMPDIR_QUICK;
		panel_action(src_panel, act_CMPDIR, dst_panel, &cmp_mode, 1);
		break;

	    case BUILTIN_thorough_compare_panels:
		cmp_mode = CMPDIR_THOROUGH;
		panel_action(src_panel, act_CMPDIR, dst_panel, &cmp_mode, 1);
		break;

	    case BUILTIN_name_downcase:
		to_case = CASE_DOWN;
		panel_action(src_panel, act_CASE, dst_panel, &to_case, 1);
		break;

	    case BUILTIN_name_upcase:
		to_case = CASE_UP;
		panel_action(src_panel, act_CASE, dst_panel, &to_case, 1);
		break;

	    case BUILTIN_up_one_dir:
		panel_action(src_panel, act_UP_ONE_DIR, dst_panel, NULL, 1);
		il_restore(saved_il);
		set_prompt();
		saved_il = il_save();
		tty_update_title(panel_get_wpath(src_panel));
		break;

	    case BUILTIN_compare:
		panel_action(src_panel, act_COMPARE, dst_panel, NULL, 1);
		break;

	    case BUILTIN_bin_packing:
		if (il_read_line(L"Bin size (in Kb): ", &input, L"0",
				 command->history))
		{
		    if (input[0] == 0)
			break;

		    panel_action(src_panel, act_BIN_PACKING,
				 dst_panel, input, 1);

		    xfree(input);
		    input = NULL;
		}
		break;

	    case BUILTIN_horizontal_scroll_left:
		panel_action(src_panel, act_HORIZONTAL_SCROLL_LEFT,
			     dst_panel, input, repeat_count);
		break;

	    case BUILTIN_horizontal_scroll_right:
		panel_action(src_panel, act_HORIZONTAL_SCROLL_RIGHT,
			     dst_panel, input, repeat_count);
		break;

	    case BUILTIN_select_extension:
		panel_action(src_panel, act_SELECT_EXTENSION, dst_panel,
			     NULL, 1);
		break;

	    case BUILTIN_unselect_extension:
		panel_action(src_panel, act_UNSELECT_EXTENSION, dst_panel,
			     NULL, 1);
		break;

	    case BUILTIN_apropos:
		aproposstr=NULL;
		il_read_line(L"Apropos: ", &aproposstr,
			     NULL, command->history);

		if (aproposstr)
		{
		    if(*aproposstr != L'\0')
		    {
			extern tty_key_t *key_list_head;
			tty_key_t *key;
			int fd;
			FILE *fp=NULL;
			int gotmatch=0;
			char *template="gnuit-apropos-XXXXXX";
			char *tmpfn;
			tmpfn=xmalloc(strlen(temporary_directory)+strlen(template)+1+2);
			sprintf(tmpfn,"%s/%s",temporary_directory,template);
			fd=mkstemp(tmpfn);
			if(fd != -1)
			    fp=fdopen(fd,"w");
			if(!fp)
			{
			    il_read_char(L"Error opening temporary file", NULL, IL_ERROR|IL_BEEP);
			    break;
			}
			for(key=key_list_head;key;key=key->next)
			{
			    command_t *command=(command_t *)key->aux_data;
			    char *mbsapropos=wcsduptombs(aproposstr);
			    if(strcasestr(command->name, mbsapropos))
			    {
				gotmatch=1;
				fprintf(fp,"%s: %ls\n",command->name, command->sequence);
			    }
			    xfree(mbsapropos);
			}
			fclose(fp);
			if(gotmatch)
			{
			    char *cmd, *pager;
			    pager=getenv("GNUIT_PAGER");
			    if(!pager)
				pager="more";
			    cmd=xmalloc(strlen(pager)+strlen(tmpfn)+1+1);
			    sprintf(cmd,"%s %s",pager,tmpfn);
			    tty_end_cursorapp();
			    start(cmd,0);
			    xfree(cmd);
			    wait_msg=1;
			}
			else
			    il_read_char(L"No matches", NULL, 0);
			unlink(tmpfn);
			xfree(tmpfn);
		    }
		    xfree(aproposstr);
		    goto restart;
		}

		break;

	    default:
		if (key)
		{
		    il_free(saved_il);

		    while (repeat_count--)
			il_dispatch_commands(key, IL_MOVE | IL_EDIT);

		    saved_il = il_save();
		}
		break;
	}
    }

    final_path = panel_get_path(src_panel);
    panel_end(left_panel);
    panel_end(right_panel);
    tty_set_mode(TTY_CANONIC);
    tty_defaults();

    if (il)
	il_end();

    status_end();
    remove_log();

    tty_end(screen);

    if (output_final_path)
	write(3, final_path, strlen(final_path));

    return 0;
}
