# $Id: Makefile.am,v 1.7 1999/05/31 03:44:29 tudor Exp $
EXTRA_DIST = gitaction gitmkdirs gitmount gitrgrep gitunpack gitxgrep\
	     .gitaction .gdbinit

bin_PROGRAMS = git gitps gitview gitwipe gitkeys gitwhich
bin_SCRIPTS = gitaction gitmkdirs gitmount gitrgrep gitunpack gitxgrep\
	      .gitaction
# We're not including tilde.c & tilde.h here because they are
# automagically detected (configure.in includes them in LIBOBJS).
EXTRA_git_SOURCES = ansi_stdlib.h file.h stat.h stdc.h xtime.h\
		    tilde.h history.h
git_SOURCES = git.c tty.c tty.h window.c window.h inputline.c inputline.h\
	      status.c status.h panel.c panel.h title.c title.h\
	      configure.c configure.h misc.c misc.h signals.c signals.h\
	      xmalloc.c xmalloc.h xstring.c xstring.h xio.c xio.h\
	      xid.c xid.h xstack.c xstack.h system.c system.h\
	      fsusage.c fsusage.h getopt.c getopt.h fnmatch.c fnmatch.h
gitps_SOURCES = gitps.c tty.c tty.h window.c window.h misc.c misc.h\
		configure.c configure.h signals.c signals.h xio.c xio.h\
		xmalloc.c xmalloc.h xstring.c xstring.h getopt.c getopt.h
gitview_SOURCES = gitview.c tty.c tty.h window.c window.h misc.c misc.h\
		  configure.c configure.h signals.c signals.h xio.c xio.h\
		  xmalloc.c xmalloc.h xstring.c xstring.h getopt.c getopt.h
gitwipe_SOURCES = gitwipe.c
gitkeys_SOURCES = gitkeys.c
gitwhich_SOURCES = gitwhich.c xmalloc.c xmalloc.h

git_LDADD = @LIBOBJS@
gitps_LDADD = @LIBOBJS@
gitview_LDADD = @LIBOBJS@

INCLUDES = -I$(srcdir)/.. -I..
